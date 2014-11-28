/**
 * @file alarmconfiguration.cpp
 *
 * @author Tobias Triffterer
 *
 * @brief Singleton to read and change the configuration of this application
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

#include <iostream>
#include <stdexcept>
#include <QtCore/QList>

#include "alarmconfiguration.h"
#include "exceptionhandler.h"
#include "configfilelocation.inc"

using namespace AlarmNotifications;

AlarmConfiguration& AlarmConfiguration::instance() noexcept
{
    try {
        static AlarmConfiguration global_instance;
        return global_instance;
    }
    catch ( std::exception& e )
    {
        ExceptionHandler ( e, "while instanciating AlarmConfiguration.", true );
        exit ( 1 ); // ExceptionHandler will also call exit(), but g++ does not know when compiling this
        // so it will complain about a missing return value
    }
    catch ( ... )
    {
        ExceptionHandler ( "while instanciating AlarmConfiguration.", true );
        exit ( 1 ); // ExceptionHandler will also call exit(), but g++ does not know when compiling this
        // so it will complain about a missing return value
    }
}

AlarmConfiguration::AlarmConfiguration()
    : _configfilelocation ( CreateConfigFileLocation() ),
      _backend ( KSharedConfig::openConfig ( QString::fromUtf8 ( _configfilelocation.c_str() ) , KConfig::SimpleConfig ) ),
      _skeleton ( _backend )
{
    CreateActiveMQConnectivitySettings();
}

AlarmConfiguration::~AlarmConfiguration()
{
    _backend->sync();
}

const std::string& AlarmConfiguration::getConfigFileLocation() const noexcept
{
    return _configfilelocation;
}

void AlarmConfiguration::ReReadConfiguration()
{
    _backend->markAsClean();
    _backend->reparseConfiguration();
    _skeleton.readConfig();
}

void AlarmConfiguration::WriteConfiguration()
{
    _skeleton.writeConfig();
    _backend->sync();
}

void AlarmConfiguration::CreateActiveMQConnectivitySettings()
{
    _skeleton.setCurrentGroup ( QString::fromUtf8 ( "ActiveMQConnectivity" ) );
    _activemquriitem = _skeleton.addItemString ( "ActiveMQURI", _activemquri, "failover:(tcp://localhost:61616)" );
    _activemqusernameitem = _skeleton.addItemString ( "ActiveMQUsername", _activemqusername );
    _activemqpassworditem = _skeleton.addItemString ( "ActiveMQPassword", _activemqpassword );
    _activemqtopicnameitem = _skeleton.addItemString ( "ActiveMQTopicName", _activemqtopicname );
    _laboratorynotificationtimeoutitem = _skeleton.addItemUInt ( "LaboratoryNotificationTimeout", _laboratorynotificationtimeout, 0 );
    _desktopnotificationtimeoutitem = _skeleton.addItemUInt ( "DesktopNotificationTimeout", _desktopnotificationtimeout, 0 );
    _emailnotificationtimeoutitem = _skeleton.addItemUInt ( "EMailNotificationTimeout", _emailnotificationtimeout, 0 );
    _emailnotificationfromitem = _skeleton.addItemString ( "EMailNotificationFrom", _emailnotificationfrom );
    _emailnotificationtoitem = _skeleton.addItemString ( "EMailNotificationTo", _emailnotificationto );
    _emailnotificationservernameitem = _skeleton.addItemString ( "EMailNotificationServerName", _emailnotificationservername );
    _emailnotificationserverportitem = _skeleton.addItemUInt ( "EMailNotificationServerPort", _emailnotificationserverport );
    _emailnotificationserverportitem->setMinValue ( 0 ); // Minimum port number in TCP standard
    _emailnotificationserverportitem->setMaxValue ( 65535 ); // Maximum port number in TCP standard
    _flashlightrelaisdevicenodeitem = _skeleton.addItemString ( "FlashLightRelaisDeviceNode", _flashlightrelaisdevicenode );
}

std::string AlarmConfiguration::getActiveMQURI() const noexcept
{
    return std::string ( _activemquri.toUtf8().data() );
}

void AlarmConfiguration::setActiveMQURI ( const std::string& newSetting )
{
    _activemquriitem->setValue ( QString::fromUtf8 ( newSetting.c_str() ) );
}

std::string AlarmConfiguration::getActiveMQUsername() const noexcept
{
    return std::string ( _activemqusername.toUtf8().data() );
}

void AlarmConfiguration::setActiveMQUsername ( const std::string& newSetting )
{
    _activemqusernameitem->setValue ( QString::fromUtf8 ( newSetting.c_str() ) );
}

std::string AlarmConfiguration::getActiveMQPassword() const noexcept
{
    return std::string ( _activemqpassword.toUtf8().data() );
}

void AlarmConfiguration::setActiveMQPassword ( const std::string& newSetting )
{
    _activemqpassworditem->setValue ( QString::fromUtf8 ( newSetting.c_str() ) );
}

std::string AlarmConfiguration::getActiveMQTopicName() const noexcept
{
    return std::string ( _activemqtopicname.toUtf8().data() );
}

void AlarmConfiguration::setActiveMQTopicName ( const std::string& newSetting )
{
    _activemqtopicnameitem->setValue ( QString::fromUtf8 ( newSetting.c_str() ) );
}

unsigned int AlarmConfiguration::getLaboratoryNotificationTimeout() const noexcept
{
    return _laboratorynotificationtimeout;
}

void AlarmConfiguration::setLaboratoryNotificationTimeout ( const unsigned int newSetting )
{
    _laboratorynotificationtimeoutitem->setValue ( newSetting );
}

unsigned int AlarmConfiguration::getDesktopNotificationTimeout() const noexcept
{
    return _desktopnotificationtimeout;
}

void AlarmConfiguration::setDesktopNotificationTimeout ( const unsigned int newSetting )
{
    _desktopnotificationtimeoutitem->setValue ( newSetting );
}

unsigned int AlarmConfiguration::getEMailNotificationTimeout() const noexcept
{
    return _emailnotificationtimeout;
}

void AlarmConfiguration::setEMailNotificationTimeout ( const unsigned int newSetting )
{
    _emailnotificationtimeoutitem->setValue ( newSetting );
}

std::string AlarmConfiguration::getEMailNotificationFrom() const noexcept
{
    return std::string ( _emailnotificationfrom.toUtf8().data() );
}

void AlarmConfiguration::setEMailNotificationFrom ( const std::string& newSetting )
{
    _emailnotificationfromitem->setValue ( QString::fromUtf8 ( newSetting.c_str() ) );
}

std::string AlarmConfiguration::getEMailNotificationTo() const noexcept
{
    return std::string ( _emailnotificationto.toUtf8().data() );
}

void AlarmConfiguration::setEMailNotificationTo ( const std::string& newSetting )
{
    _emailnotificationtoitem->setValue ( QString::fromUtf8 ( newSetting.c_str() ) );
}

std::string AlarmConfiguration::getEMailNotificationServerName() const noexcept
{
    return std::string ( _emailnotificationservername.toUtf8().data() );
}

void AlarmConfiguration::setEMailNotificationServerName ( const std::string& newSetting )
{
    _emailnotificationservernameitem->setValue ( QString::fromUtf8 ( newSetting.c_str() ) );
}

unsigned int AlarmConfiguration::getEMailNotificationServerPort() const noexcept
{
    return _emailnotificationserverport;
}

void AlarmConfiguration::setEMailNotificationServerPort ( const unsigned int newSetting )
{
    _emailnotificationserverportitem->setValue ( newSetting );
}

std::string AlarmConfiguration::getFlashLightRelaisDeviceNode() const noexcept
{
    return std::string ( _flashlightrelaisdevicenode.toUtf8().data() );
}

void AlarmConfiguration::setFlashLightRelaisDevideNode ( const std::string& newSetting )
{
    _flashlightrelaisdevicenodeitem->setValue ( QString::fromUtf8 ( newSetting.c_str() ) );
}

KSharedConfigPtr AlarmConfiguration::internal()
{
    return _backend;
}

KConfigSkeleton* AlarmConfiguration::internal_skel()
{
    return &_skeleton;
}

std::string AlarmConfiguration::CreateConfigFileLocation()
{
    const char*const envvar = getenv ( "ALARMNOTIFICATIONSCONFIG" );
    if ( envvar == nullptr )
        if ( getenv ( "HOME" ) != nullptr )
        {
            std::string conffile ( CONFIGFILELOCATION );
            if ( conffile.find ( "~" ) != std::string::npos )
                conffile.replace ( conffile.find ( "~" ), 1, getenv ( "HOME" ) );
            return conffile;
        }
        else
        {
            return std::string ( CONFIGFILELOCATION );
        }
    else
        return std::string ( envvar );
}
