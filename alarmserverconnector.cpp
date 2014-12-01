/**
 * @file alarmserverconnector.cpp
 *
 * @author Tobias Triffterer
 *
 * @brief Provides connectivity to the CSS Alarm Server
 *
 * @version 1.0.0
 *
 * AlarmNotifications - Laboratory and desktop notification framework to
 * be used with EPICS and Control System Studio
 *
 * Copyright © 2014 by Tobias Triffterer <tobias@ep1.ruhr-uni-bochum.de>
 * for Institut für Experimentalphysik I der Ruhr-Universität Bochum
 * (http://ep1.ruhr-uni-bochum.de)
 *
 * The latest source code is here: https://github.com/ttrubep1/AlarmNotifications
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 **/

#include "alarmserverconnector.h"

#include <iostream>
#include <stdexcept>
#include <vector>

#ifndef NOTUSELIBNOTIFY
#include <glib-2.0/glib.h>
#include <libnotify/notification.h>
#include <libnotify/notify.h>
#endif

#include "alarmconfiguration.h"
#include "beedo.h"
#include "cmsclient.h"
#include "emailsender.h"
#include "exceptionhandler.h"
#include "flashlight.h"

using namespace AlarmNotifications;

AlarmServerConnector::AlarmServerConnector ( const bool desktopVersion, const bool activateBeedo )
    : _desktopVersion ( desktopVersion ),
      _activateBeedo ( activateBeedo ),
      _cmsclient ( *this ),
      _runwatcher ( true ),
      _flashlighton ( false ),
      _watcher ( boost::bind ( &AlarmServerConnector::startWatcher, this ) ),
      _flashlightthread ( boost::bind ( &AlarmServerConnector::operateFlashLight, this ) ),
      _oldestAlarm ( noAlarmActive )
{
    if ( !_desktopVersion && _activateBeedo )
        throw std::logic_error ( "The \"beedo\" optoacoustic alarm can only be used in desktop mode!" );
#ifndef NOTUSELIBNOTIFY
    notify_init ( "DCS Alarm System" );
#endif
}

AlarmServerConnector::~AlarmServerConnector()
{
    _runwatcher = false;
    _watcher.join();
    _flashlightthread.join();
#ifndef NOTUSELIBNOTIFY
    notify_uninit();
#endif
}

void AlarmServerConnector::notifyStatusChange ( const AlarmStatusEntry status )
{
    boost::lock_guard<boost::mutex> concurrencylock ( _statusmapmutex );
    const std::string& pvname = status.getPVName();
    auto entry = _statusmap.find ( pvname );
    if ( checkSeverityString ( status.getSeverity() ) )
    {
        if ( entry != _statusmap.end() )
            _statusmap.erase ( entry );
    }
    else
    {
        if ( entry == _statusmap.end() )
            _statusmap.insert ( std::pair<std::string, AlarmStatusEntry> ( pvname, status ) );
        else
            ( *entry ).second.update ( status );
        if ( _oldestAlarm == noAlarmActive )
            _oldestAlarm = status.getTriggerTime();
    }
}

bool AlarmServerConnector::checkSeverityString ( const std::string& severity )
{
    if ( severity == "OK" )
        return true;
    if ( severity.substr ( severity.length()-4, 4 ) == "_ACK" )
        return true;
    return false;
}

void AlarmServerConnector::startWatcher()
{
    while ( _runwatcher )
    {
        sleep ( 1 );
        checkStatusMap();
    }
}

void AlarmServerConnector::checkStatusMap()
{
    boost::lock_guard<boost::mutex> concurrencylock ( _statusmapmutex );
    if ( _statusmap.size() == 0 && _oldestAlarm != noAlarmActive )
    {
        _oldestAlarm = noAlarmActive;
        if ( _activateBeedo )
            Beedo::stop();
    }
    /*for ( auto i = _statusmap.begin(); i != _statusmap.end(); i++ )
        std::cout << ( *i ).second << std::endl;*/
    if (
        _statusmap.size() != 0
        && _oldestAlarm + AlarmConfiguration::instance().getDesktopNotificationTimeout() <= std::time ( nullptr )
    )
    {
        prepareDesktopNotification();
        if ( _activateBeedo )
            Beedo::start();
    }
    if (
        _statusmap.size() != 0
        && _oldestAlarm + AlarmConfiguration::instance().getEMailNotificationTimeout() <= std::time ( nullptr )
    )
        prepareEMailNotification();
}

void AlarmServerConnector::operateFlashLight()
{
    if ( _desktopVersion )
        return; // Desktop version does not have a flashlight
    while ( _runwatcher )
    {
        sleep ( 1 );
        if (
            !_flashlighton
            && _statusmap.size() != 0
            && _oldestAlarm + AlarmConfiguration::instance().getLaboratoryNotificationTimeout() <= std::time ( nullptr )
        )
            switchFlashLightOn();
        if ( _flashlighton && _statusmap.size() == 0 )
            switchFlashLightOff();
    }
}

void AlarmServerConnector::switchFlashLightOn()
{
    if ( AlarmConfiguration::instance().getLaboratoryNotificationTimeout() == 0 )
        return; // A value of 0 disables the notification via flash light
    _flashlighton = true;
    std::cout << "Flash light on!" << std::endl;
    FlashLight::switchOn();
}

void AlarmServerConnector::switchFlashLightOff()
{
    if ( AlarmConfiguration::instance().getLaboratoryNotificationTimeout() == 0 )
        return; // A value of 0 disables the notification via flash light
    _flashlighton = false;
    std::cout << "Flash light off!" << std::endl;
    FlashLight::switchOff();
}

void AlarmServerConnector::prepareDesktopNotification()
{
    if ( AlarmConfiguration::instance().getDesktopNotificationTimeout() == 0 )
        return; // A timeout of 0 disables desktop notifications
    std::vector<AlarmStatusEntry> alarmsToUse;
    for ( auto i = _statusmap.begin(); i != _statusmap.end(); i++ )
    {
        if ( ( *i ).second.getTriggerTime() + AlarmConfiguration::instance().getDesktopNotificationTimeout() <= std::time ( nullptr ) )
        {
            if ( ! ( *i ).second.getDesktopNotificationSent() )
            {
                ( *i ).second.setDesktopNotificationSent ( true );
                alarmsToUse.push_back ( AlarmStatusEntry ( ( *i ).second ) );
            }
        }
    }
    if ( alarmsToUse.size() > 0 )
    {
        boost::thread send ( boost::bind ( &AlarmServerConnector::sendDesktopNotification, this, std::move ( alarmsToUse ) ) );
        send.detach();
    }
}

void AlarmServerConnector::sendDesktopNotification ( const std::vector<AlarmStatusEntry> alarm )
{
    std::string alarmtext = "Alarm on this/these PV(s):\n";
    for ( auto i = alarm.begin(); i != alarm.end(); i++ )
    {
        alarmtext += ( *i ).getPVName() + "\n";
    }
#ifndef NOTUSELIBNOTIFY
    NotifyNotification* n = notify_notification_new (
                                "Detector Alarm",
                                alarmtext.c_str(),
                                "dialog-warning"
                            );
    notify_notification_set_timeout ( n, NOTIFY_EXPIRES_NEVER );
    notify_notification_set_urgency ( n, NOTIFY_URGENCY_CRITICAL );
    GError* showerror = nullptr;
    notify_notification_show ( n, &showerror );
    g_object_unref ( G_OBJECT ( n ) );
#else
    std::string command = "notify-send -u critical -t 0 -i dialog-warning 'Detector Alarm' ";
    command += std::string ( "'" );
    command += alarmtext;
    command += std::string ( "'" );
    system ( command.c_str() );
#endif
}

void AlarmServerConnector::prepareEMailNotification()
{
    if ( _desktopVersion )
        return; // The desktop version does not send e-mails
    if ( AlarmConfiguration::instance().getEMailNotificationTimeout() == 0 )
        return; // A timeout of 0 disables e-mail notifications
    std::vector<AlarmStatusEntry> alarmsToUse;
    for ( auto i = _statusmap.begin(); i != _statusmap.end(); i++ )
    {
        //if ( ( *i ).second.getTriggerTime() + AlarmConfiguration::instance().getEMailNotificationTimeout() <= std::time ( nullptr ) )
        //{
        if ( ! ( *i ).second.getEmailNotificationSent() )
        {
            ( *i ).second.setEmailNotificationSent ( true );
            alarmsToUse.push_back ( AlarmStatusEntry ( ( *i ).second ) );
        }
        //}
    }
    if ( alarmsToUse.size() > 0 )
    {
        boost::thread send ( boost::bind ( &AlarmServerConnector::sendEMailNotification, this, std::move ( alarmsToUse ) ) );
        send.detach();
    }
}

void AlarmServerConnector::sendEMailNotification ( const std::vector<AlarmStatusEntry> alarm )
{
    EMailSender::sendAlarmNotification ( alarm );
}

size_t AlarmServerConnector::getNumberOfAlarms() const noexcept
{
    return _statusmap.size();
}
