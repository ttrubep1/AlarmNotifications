/**
 * @file alarmstatusentry.h
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

#ifndef ALARMSTATUSENTRY_H
#define ALARMSTATUSENTRY_H

#include "oldgcccompat.h" // Compatibilty macros for GCC < 4.7

#include <ctime>
#include <ostream>
#include <string>

namespace AlarmNotifications
{

/**
 * @brief Entry in the AlarmServerConnector statusmap
 *
 * This class is a container for the information regarding the alarm status of a single PV. The class AlarmServerConnector manages a map of these entries. This class does not operate on or manage this information, it is a pure data container,
 */
class AlarmStatusEntry final
{
private:
    /**
     * @brief Name of the PV
     *
     * The name of the PV that triggered the alarm. The pseudo-protocol prefix "epics://" used within the CSS Alarm Server is removed by CMSClient
     */
    std::string _pvname;
    /**
     * @brief The severity of the alarm
     *
     * This severity differs from the severity notation in EPICS because the CSS Alarm Server adds the functionality to acknowledge an alarm, a concept that does not exist in pure EPICS. An acknowledged alarm severity has "_ACK" added to the normal severity description.
     */
    std::string _severity;
    /**
     * @brief The alarm status of the PV
     *
     * The status of the PV within the CSS Alarm Server. This status can differ from the status within EPICS as the Alarm Server has the ability to latch alarms.
     */
    std::string _status;
    /**
     * @brief Time the alarm message was received
     *
     * The constructor records the time when the entry is created. The timestamp is used to calculate if the alarm is due to send out a notification, e.g. via e-mail.
     */
    time_t _triggertime;
    /**
     * @brief Flag for sent desktop notification
     *
     * Indicator to show if a desktop notification has already been sent out to avoid double messaging.
     */
    bool _desktopNotificationSent;
    /**
     * @brief Flag for sent e-mail notification
     *
     * Indicator to show id an e-mail has already been sent out to avoid double messaging.
     */
    bool _emailNotificationSent;

public:
    /**
     * @brief Constructor
     *
     * Writes the arguments to the internal storage and takes a timestamp. The notification flags are initialized as false.
     * @param pvname Name of the PV triggering the alarm
     * @param severity The Alarm Server severity string
     * @param status The Alarm Server status string
     */
    AlarmStatusEntry ( const std::string& pvname, const std::string& severity, const std::string& status ) noexcept;
    /**
     * @brief Destructor
     *
     * Has nothing to do...
     */
    ~AlarmStatusEntry() noexcept;
    /**
     * @brief Equality operator
     *
     * Checks if all parameters within two instances are equal,
     * @param other Another instance of AlarmStatusEntry
     * @return Boolean to indicate equality
     */
    bool operator== ( const AlarmStatusEntry& other ) noexcept;
    /**
     * @brief Copy constructor
     * 
     * Creates a non-destructive copy of another status object.
     * @param other Another instance of AlarmStatusEntry
     */
    AlarmStatusEntry ( const AlarmStatusEntry& other ) noexcept;
    /**
     * @brief Move constructor (C++11)
     * 
     * Creates a destructive copy of another status object. Accessing the other object afterwards may result in undefined behaviour.
     * @param other Another instance of AlarmStatusEntry
     */
    AlarmStatusEntry ( AlarmStatusEntry&& other ) noexcept;
    /**
     * @brief Copy assignment
     * 
     * Overwrites the data of this instance with data copied from the other instance.
     * @param other Another instance of AlarmStatusEntry
     * @return Reference to this instance
     */
    AlarmStatusEntry& operator= ( const AlarmStatusEntry& other ) noexcept;
    /**
     * @brief Move assignment (C++11)
     * 
     * Overwrites the data of this instance with data moved from the other object. Accessing the other object afterwards may result in undefined behaviour.
     * @param other Another instance of AlarmStatusEntry
     * @return Reference to this instance
     */
    AlarmStatusEntry& operator= ( AlarmStatusEntry&& other ) noexcept;
    /**
     * @brief Query PV name
     * 
     * The name of the PV that triggerd the alarm.
     * @return Read-only reference to standard string
     */
    const std::string& getPVName() const noexcept;
    /**
     * @brief Query the severity
     * 
     * The severity differs from the severity notation in EPICS because the CSS Alarm Server adds the functionality to acknowledge an alarm, a concept that does not exist in pure EPICS. An acknowledged alarm severity has "_ACK" added to the normal severity description.
     * @return Read-only reference to standard string
     */
    const std::string& getSeverity() const noexcept;
    /**
     * @brief Change the severity
     * 
     * The severity differs from the severity notation in EPICS because the CSS Alarm Server adds the functionality to acknowledge an alarm, a concept that does not exist in pure EPICS. An acknowledged alarm severity has "_ACK" added to the normal severity description.
     * @param severity The new severity string
     * @return Nothing
     */
    void setSeverity ( const std::string& severity ) noexcept;
    /**
     * @brief Query the status
     * 
     * The status of the PV within the CSS Alarm Server. This status can differ from the status within EPICS as the Alarm Server has the ability to latch alarms.
     * @return Read-only reference to standard string
     */
    const std::string& getStatus() const noexcept;
    /**
     * @brief Change the status
     * 
     * The status of the PV within the CSS Alarm Server. This status can differ from the status within EPICS as the Alarm Server has the ability to latch alarms.
     * @param status The new status string
     * @return Nothing
     */
    void setStatus ( const std::string& status ) noexcept;
    /**
     * @brief Query the trigger time
     * 
     * The timestamp saved in the constructor. The timestamp is used to calculate if the alarm is due to send out a notification, e.g. via e-mail.
     * @return POSIX time value
     */
    time_t getTriggerTime() const noexcept;
    /**
     * @brief Change the trigger time_t
     * 
     * Changes the timestamp of the message reception. Should only be used when there is a really good reason for.
     * @param triggertime POSIX time value
     * @return Nothing
     */
    void setTriggerTime ( const time_t triggertime ) noexcept;
    /**
     * @brief Update severity and status data
     * 
     * The severity and status values are copied from the other instance of AlarmStatusEntry, all other values are left untouched.
     * @param newdata Another instance of AlarmStatusEntry
     * @return Nothing
     */
    void update ( const AlarmStatusEntry& newdata ) noexcept;
    /**
     * @brief Query desktop notification flag
     * 
     * Flag to show if a desktop notification has already been sent out to avoid double messaging.
     * @return True if desktop notification has already been sent.
     */
    bool getDesktopNotificationSent() const noexcept;
    /**
     * @brief Change desktop notification flag
     * 
     * Flag to show if a desktop notification has already been sent out to avoid double messaging.
     * @param desktopNotificationSent Set to true of notification has been sent
     * @return Nothing
     */
    void setDesktopNotificationSent ( const bool desktopNotificationSent ) noexcept;
    /**
     * @brief Query e-mail notification flag
     * 
     * Flag to show if an e-mail notification has already been sent out to avoid double messaging.
     * @return True if e-mail notification has already been sent.
     */
    bool getEmailNotificationSent() const noexcept;
    /**
     * @brief Change e-mail notification flag
     * 
     * Flag to show if an e-mail notification has already been sent out to avoid double messaging.
     * @param emailNotificationSent Set to true of notification has been sent
     * @return Nothing
     */
    void setEmailNotificationSent ( const bool emailNotificationSent ) noexcept;
};

/**
 * @brief Stream output operator for AlarmStatusEntry
 * 
 * Print an instance of AlarmStatusEntry to any output stream, e.g. to stdout for debug purposes.
 * @param os Standard output stream
 * @param ase An instance of AlarmStatusEntry
 * @return The supplied output stream
 */
std::ostream& operator<< ( std::ostream& os, const AlarmNotifications::AlarmStatusEntry& ase );

}

#endif // ALARMSTATUSENTRY_H
