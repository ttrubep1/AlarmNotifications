/**
 * @file alarmstatusentry.cpp
 *
 * @author Tobias Triffterer
 *
 * @brief Alarm entry for the AlarmServerConnector
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

#include "alarmstatusentry.h"

using namespace AlarmNotifications;

AlarmStatusEntry::AlarmStatusEntry ( const std::string& pvname, const std::string& severity, const std::string& status ) noexcept
:
_pvname ( pvname ),
        _severity ( severity ),
        _status ( status ),
        _triggertime ( std::time ( nullptr ) ),
        _desktopNotificationSent ( false ),
        _emailNotificationSent ( false )
{

}

AlarmStatusEntry::~AlarmStatusEntry() noexcept
{

}

AlarmStatusEntry::AlarmStatusEntry ( const AlarmStatusEntry& other ) noexcept
:
_pvname ( other._pvname ),
_severity ( other._severity ),
_status ( other._status ),
_triggertime ( other._triggertime ),
_desktopNotificationSent ( other._desktopNotificationSent ),
_emailNotificationSent ( other._emailNotificationSent )
{

}

AlarmStatusEntry::AlarmStatusEntry ( AlarmStatusEntry&& other ) noexcept
:
_pvname ( std::move ( other._pvname ) ),
_severity ( std::move ( other._severity ) ),
_status ( std::move ( other._status ) ),
_triggertime ( other._triggertime ),
_desktopNotificationSent ( other._desktopNotificationSent ),
_emailNotificationSent ( other._emailNotificationSent )
{

}

AlarmStatusEntry& AlarmStatusEntry::operator= ( const AlarmStatusEntry& other ) noexcept
{
    if ( this != &other )
    {
        _pvname = other._pvname;
        _severity = other._severity;
        _status = other._status;
        _triggertime = other._triggertime;
        _desktopNotificationSent = other._desktopNotificationSent;
        _emailNotificationSent = other._emailNotificationSent;
    }
    return *this;
}

AlarmStatusEntry& AlarmStatusEntry::operator= ( AlarmStatusEntry&& other ) noexcept
{
    if ( this != &other )
    {
        _pvname = std::move ( other._pvname );
        _severity = std::move ( other._severity );
        _status = std::move ( other._status );
        _triggertime = other._triggertime;
        _desktopNotificationSent = other._desktopNotificationSent;
        _emailNotificationSent = other._emailNotificationSent;
    }
    return *this;
}

bool AlarmStatusEntry::operator== ( const AlarmStatusEntry& other ) noexcept
{
    return ( _pvname == other._pvname ) && ( _severity == other._severity ) && ( _status == other._status ) && ( _triggertime == other._triggertime ) && ( _desktopNotificationSent == other._desktopNotificationSent ) && ( _emailNotificationSent == other._emailNotificationSent );
}

const std::string& AlarmStatusEntry::getPVName() const noexcept
{
    return _pvname;
}

const std::string& AlarmStatusEntry::getSeverity() const noexcept
{
    return _severity;
}

void AlarmStatusEntry::setSeverity ( const std::string& severity ) noexcept
{
    _severity = severity;
}

const std::string& AlarmStatusEntry::getStatus() const noexcept
{
    return _status;
}

void AlarmStatusEntry::setStatus ( const std::string& status ) noexcept
{
    _status = status;
}

time_t AlarmStatusEntry::getTriggerTime() const noexcept
{
    return _triggertime;
}

void AlarmStatusEntry::setTriggerTime ( const time_t triggertime ) noexcept
{
    _triggertime = triggertime;
}

void AlarmStatusEntry::update ( const AlarmStatusEntry& newdata ) noexcept
{
    if ( _triggertime < newdata._triggertime )
    {
        _severity = newdata._severity;
        _status = newdata._status;
    }
}

bool AlarmStatusEntry::getDesktopNotificationSent() const noexcept
{
    return _desktopNotificationSent;
}

void AlarmStatusEntry::setDesktopNotificationSent ( const bool desktopNotificationSent ) noexcept
{
    _desktopNotificationSent = desktopNotificationSent;
}

bool AlarmStatusEntry::getEmailNotificationSent() const noexcept
{
    return _emailNotificationSent;
}

void AlarmStatusEntry::setEmailNotificationSent ( const bool emailNotificationSent ) noexcept
{
    _emailNotificationSent = emailNotificationSent;
}

std::ostream& AlarmNotifications::operator<< ( std::ostream& os, const AlarmNotifications::AlarmStatusEntry& ase )
{
    os << "PV: " << ase.getPVName() << "  Severity: " << ase.getSeverity() << "  Status: " << ase.getStatus() << "  Time: " << ase.getTriggerTime();
    return os;
}
