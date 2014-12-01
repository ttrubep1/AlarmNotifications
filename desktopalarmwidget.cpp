/**
 * @file desktopalarmwidget.cpp
 *
 * @author Tobias Triffterer
 *
 * @brief Provide a tray icon and desktop notifications in case of an alarm
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

#include "desktopalarmwidget.h"

#include <limits>

#include <QApplication>
#include <QIcon>
#include <QInputDialog>
#include <QMessageBox>
#include <QtConcurrentRun>

#include "alarmconfiguration.h"
#include "alarmserverconnector.h"
#include "beedo.h"
#include "oldgcccompat.h"

using namespace AlarmNotifications;

DesktopAlarmWidget::DesktopAlarmWidget ( const bool activateBeedo )
    : _activateBeedo ( activateBeedo ),
      _asc ( nullptr ),
      _run ( true ),
      _alarmActive ( false )
{
    if ( _activateBeedo )
        Beedo::instance(); // Initialize Beedo instance from main thread
    _asc = new AlarmServerConnector ( true, _activateBeedo );
    _iconThread = QtConcurrent::run ( this, &DesktopAlarmWidget::observeAlarmStatus );
    connect ( this, SIGNAL ( alarmStatusChanged() ), this, SLOT ( changeTrayIcon() ) );
    connect ( this, SIGNAL ( notificationSwitchChanged ( bool ) ), this, SLOT ( notificationSwitchChange ( bool ) ) );
}

DesktopAlarmWidget::~DesktopAlarmWidget()
{
    _run = false;
    _iconThread.waitForFinished();
    delete _asc; // Do not need to check for nullptr, deleting nullptr is always safe in C++
    if ( _activateBeedo )
        Beedo::instance().destroy();
}

DesktopAlarmWidget::DesktopAlarmWidgetStatus DesktopAlarmWidget::getStatus() const noexcept
{
    // No access to the object _asc points to, therefore no locking of the mutex
    // This avoids dead-locks if getStatus() is called within a method already protected by the mutex
    if ( _asc == nullptr )
    {
        return Disabled;
    }
    else
    {
        if ( _alarmActive )
            return ActiveAlarm;
        else
            return ActiveOK;
    }
}

void DesktopAlarmWidget::toggleNotifications()
{
    boost::lock_guard<boost::mutex> concurrency_lock ( _ascmutex );
    if ( _asc == nullptr )
    {
        _asc = new AlarmServerConnector ( true, _activateBeedo );
        emit notificationSwitchChanged ( true );
    }
    else
    {
        delete _asc;
        _asc = nullptr;
        emit notificationSwitchChanged ( false );
    }
}

void DesktopAlarmWidget::configureNotificationTimeout()
{
    bool ok = false;
    const int rawanswer = QInputDialog::getInteger (
                              nullptr,
                              QString::fromUtf8 ( "Configure notification timeout" ),
                              QString::fromUtf8 ( "Plese enter the time in seconds that should pass between\nthe reception of an alarm from the CSS Alarm Server\nand the display of a desktop notification:" ),
                              AlarmConfiguration::instance().getDesktopNotificationTimeout(),
                              1, // AlarmServerConnector would read a value of 0 as disabling desktop notifications
                              std::numeric_limits<int>::max(),
                              1,
                              &ok
                          );
    if ( !ok )
        return;
    if ( rawanswer < 1 )
        return;
    const unsigned int answer = ( unsigned int ) rawanswer; // Any non-negative int will fit into unsigned int and negative ints are excluded above
    AlarmConfiguration::instance().setDesktopNotificationTimeout ( answer );
    AlarmConfiguration::instance().WriteConfiguration();
    AlarmConfiguration::instance().ReReadConfiguration();
}

void DesktopAlarmWidget::showStatusMessage ( )
{
    QString messagetext;
    QString messagetitle = QString::fromUtf8 ( "Alarm notifications desktop widget" );
    unsigned short messagetype; // 0 = info, 1 = warning, 2 = critical
    {
        boost::lock_guard<boost::mutex> concurrency_lock ( _ascmutex );
        if ( _asc == nullptr )
        {
            messagetype = 1;
            messagetext = QString::fromUtf8 ( "The Alarm notifications desktop widget is currently disabled, so informations about alarms are not recevied from the alarm server!" );
        }
        else
        {
            if ( _asc->getNumberOfAlarms() == 0 )
            {
                messagetype = 0;
                messagetext = QString::fromUtf8 ( "No alarms are known to the Alarm notifications desktop widget.\n\nPlease note that alarms triggered before the start or re-activation of this widget have not been received, so if you want to be sure that nothing is wrong, look at the alarm display in CSS." );
            }
            else
            {
                messagetype = 2;
                messagetext = QString::fromUtf8 ( "ATTENTION!\n\nThere are %1 alarm(s) active in the Detector Control System! For detailed information look at the alarm display in CSS!" ).arg ( _asc->getNumberOfAlarms() );
            }
        }
    }
    switch ( messagetype )
    {
    case 2:
        QMessageBox::critical (
            nullptr,
            messagetitle,
            messagetext
        );
        break;
    case 1:
        QMessageBox::warning (
            nullptr,
            messagetitle,
            messagetext
        );
        break;
    case 0:
    default:
        QMessageBox::information (
            nullptr,
            messagetitle,
            messagetext
        );
    }
}

void DesktopAlarmWidget::exitApplication()
{
    QApplication::exit ( 0 );
}

void DesktopAlarmWidget::observeAlarmStatus()
{
    while ( _run )
    {
        {
            boost::lock_guard<boost::mutex> concurrency_lock ( _ascmutex );
            if ( _asc != nullptr )
            {
                if ( _alarmActive && _asc->getNumberOfAlarms() == 0 )
                {
                    _alarmActive = false;
                    emit alarmStatusChanged();
                }
                if ( !_alarmActive && _asc->getNumberOfAlarms() != 0 )
                {
                    _alarmActive = true;
                    emit alarmStatusChanged();
                }
            }
        }
        usleep ( 500*1000 ); // Check status every 0.5 seconds
    }
}



#include "desktopalarmwidget.moc"
