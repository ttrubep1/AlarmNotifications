/**
 * @file alarmserverconnector.h
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

#ifndef ALARMSERVERCONNECTOR_H
#define ALARMSERVERCONNECTOR_H

#include "oldgcccompat.h" // Compatibilty macros for GCC < 4.7

#include <map>
#include <limits>
#include <string>
#include <thread>

#include <boost/thread.hpp>

#include "alarmstatusentry.h"
#include "cmsclient.h"

#if ( __WORDSIZE < 64 ) || ( LONG_MAX < 9223372036854775807L )
#warning Using this application on non-64bit architecture may cause it suffer from the year-2038-bug on 19 Jan 2038 03:14:07 UTC. Linux on 64bit is not affected as time_t is a long int and long int is 64bit wide there.
#endif

namespace AlarmNotifications
{

/**
 * @brief Connect to a CSS Alarm Server
 *
 * This class connects to the CSS Alarm Server using the class CMSClient and evaluates the messages of the alarm server received by the client class. This class will create a map of all active alarms and - if the alarms are active long enough - it will initiate notifications to alert the user about the alarm in the laboratory. Three different types of notifications are available: The first one is the laboratory notification, where a red flashlight in the lab will alert the people working there. The second one is a desktop notification on the office computers while the third one sends an e-mail to a mailing list.
 *
 * This class can operate in either server or desktop mode. The server mode instance will run in the lab and provide the flashlight and e-mail notifications - while a desktop notification is still possible, e.g. on the server terminal. The desktop version will run on an office PC and do only desktop notifications. In addition, the desktop version can use the Beedo engine where in addition to the notification an opto-acoustic alarm will be shown on the desktop. This feature is intended to be used on a control room computer during beamtime.
 */
class AlarmServerConnector
{
private:
#if ( OLDGCC46COMPAT == 0 )
    /**
     * @brief Time of last alarm if no alarm is active.
     *
     * The parameter _oldestAlarm holds a Unix timestamp of the oldest alarm in statusmap. This is used to determine if a notification should be set off or not. To indicate that there isn't any alarm currently active, this class uses the minimum Unix timestamp value.
     *
     * According to Linux Standard Base and <time.h>, time_t is a typedef to long int. This may cause problems on 32bit machines, so this header file will cause the compiler to issue a warning (see above). On 32bit systems, long int is 32 bit wide (as is int). This means, that there will be an integer overflow in time_t on Tue, 19. Jan 2038 03:14:07 UTC, where the signature bit will flip and the timestamp jump to Fri, 13. Dez 1901 20:45:52 UTC (year 2038 problem).
     *
     * On 64bit machines, this problem does not exist because the Linux memory model defines long int as 64 bit wide there. The minimum value of a 64 bit Unix timestamp is aeons before the bing bang as beginning of space and time and the maximum value is in an (really) extreme distant future.
     *
     * On modern compilers, this static const variable uses the C++-correct initialization by using std::numeric_limits. On older compilers that cannot deal with constexpr functions, a fallback to the LONG_MIN preprocessor macro is used.
     */
    static const time_t noAlarmActive = std::numeric_limits<long int>::min(); // Proper definition
#else
    static const time_t noAlarmActive = LONG_MIN; // Fallback to preprocessor macro for old compilers
#endif
    /**
     * @brief Desktop version flag
     *
     * Flag to indicate if this instance is running as desktop version. In this case, laboratory and e-mail notifications are disabled.
     */
    const bool _desktopVersion;
    /**
     * @brief Beedo flag
     *
     * Flag to indicate if the Beedo engine should be used. If it's enabled, an opto-acoustic notification will be used in addition to the usual desktop notification.
     */
    const bool _activateBeedo;
    /**
     * @brief ActiveMQ client instance
     *
     * The instance of CMSClient, the interface to the Apache ActiveMQ message broker and the CSS alarm server.
     */
    CMSClient _cmsclient;
    /**
     * @brief Map of active alarms
     *
     * All active alarms are listed in this map, where a string containing the PV name acts as key to the content encapsulated in the AlarmStatusEntry class.
     * 
     * As this class uses multithreading, access to this map (both read and write) must ALWAYS be protected by a lock on _statusmapmutex.
     */
    std::map<std::string, AlarmStatusEntry> _statusmap;
    /**
     * @brief Mutex to protect the _statusmap
     *
     * Concurrent insert and erase operations on a std::map are not supported and may result in undefined behaviour or segfaults. Therefore, this mutex is always locked when _statusmap is accessed.
     */
    boost::mutex _statusmapmutex;
    /**
     * @brief Mutex to protect the flashlight accessed
     *
     * The hardware relais cannot be controlled concurrently be several threads, so this mutex is used to make sure that the hardware access is serialized.
     */
    boost::mutex _flashlightmutex;
    /**
     * @brief Watcher thread abortion flag
     *
     * The destructor will set this flag to false, so the watcher and flashlight threads will exit their loops.
     */
    bool _runwatcher;
    /**
     * @brief Flashlight status flag
     *
     * This flag indicates whether the flashlight is currently flashing or not.
     */
    bool _flashlighton;
    /**
     * @brief Notification thread
     *
     * This thread object will run the startWatcher() method that checks the _statusmap for alarm being active for longer than the timeout and initiate the corresponding notifications.
     */
    boost::thread _watcher;
    /**
     * @brief Flashlight operation thread
     *
     * This thread object will run the operateFlashLight() method that monitors the _statusmap and switch the flashlight on or off accordingly.
     */
    boost::thread _flashlightthread;
    /**
     * @brief Timestamp of oldest alarm in _statusmap
     *
     * The timestamp of the longest-active alarm is kept here so checkStatusMap() can calculate whether a notification should be fired. If no alarm is active at all, it is set to noAlarmActive.
     */
    time_t _oldestAlarm;

    /**
     * @brief Check severity string in CSS Alarm Server message
     *
     * Analyse the string and return whether it announces an alarm or the removal/acknowledgement of an alarm.
     * @param severity The severity parameter as recevied from the CSS alarm server.
     * @return True if alarm is active, false if it is cleared
     */
    bool checkSeverityString ( const std::string& severity );
    /**
     * @brief Start the watcher thread
     *
     * Invokes checkStatusMap every second as long as _runwatcher is true.
     * @return Nothing
     */
    void startWatcher();
    /**
     * @brief Check the _statusmap for pending notifications
     *
     * Checks if there is any alarm over the timeout and initiates the appropriate notifications if necessary. On desktop versions also controls the Beedo engine.
     * @return Nothing
     */
    void checkStatusMap();
    /**
     * @brief Operate the red flashlight in the laboratory
     *
     * Calls switchFlashLightOn() and switchFlashLightOff() to switch the flashlight on or off, according to the current alarm status. Loops until _runwatcher is set to false.
     * @return Nothing
     */
    void operateFlashLight();
    /**
     * @brief Switch laboratory flashlight on
     *
     * Tell the hardware interface to enable the flashlight.
     * @return Nothing
     */
    void switchFlashLightOn();
    /**
     * @brief Switch laboratory flashlight off
     *
     * Tell the hardware interface to disable the flashlight.
     * @return Nothing
     */
    void switchFlashLightOff();
    /**
     * @brief Select alarms to be included in a desktop notification
     *
     * Iterates over all entries in _statusmap and selects alarms to be included in a desktop notification. The alarm used have the corresponding flag in AlarmStatusEntry set.
     *
     * As this method operates under a lock on _statusmapmutex held by checkStatusMap(), it has to be very quick. It therefore does only the selection work. The alarm entries to be used are collected in a vector that is passed to sendDesktopNotification() which is spawned as a separate thread.
     * @return Nothing
     */
    void prepareDesktopNotification();
    /**
     * @brief Fire desktop notification
     *
     * This class recevies a list of alarm from prepareDesktopNotification() and puts them into a desktop notification. On systems with a libnotify version of at least 0.7, this API is used directly. On older versions, the library method notify_notification_new() requires a "GtkWidget* attach" pointer which is known to cause problems (this is why the parameter was removed from the API). On systems with the old version, a system() call is used to invoke the binary "notify-send" which is part of the libnotify package.
     * @param alarm Alarms to be included in the notification.
     * @return Nothing
     */
    void sendDesktopNotification ( const std::vector<AlarmStatusEntry > alarm );
    /**
     * @brief Select alarms to be included in an e-mail notification
     *
     * Iterates over all entries in _statusmap and selects alarms to be included in an e-mail notification. The alarm used have the corresponding flag in AlarmStatusEntry set.
     *
     * As this method operates under a lock on _statusmapmutex held by checkStatusMap(), it has to be very quick. It therefore does only the selection work. The alarm entries to be used are collected in a vector that is passed to sendEMailNotification() which is spawned as a separate thread.
     * @return Nothing
     */
    void prepareEMailNotification();
    /**
     * @brief Fire desktop notification
     *
     * This class recevies a list of alarm from prepareEMailNotification() and puts them into an e-mail notification. This is done by invoking EMailSender::sendAlarmNotification()
     * @param alarm Alarms to be included in the notification.
     * @return Nothing
     */
    void sendEMailNotification ( const std::vector< AlarmStatusEntry > alarm );
public:
    /**
     * @brief Constructor
     *
     * Intializes the CMSClient and the libnotify framework on systems with libnotify version >= 0.7. It spawns two additional threads that run startWatcher() and operateFlashLight() respectively.
     * @param desktopVersion Flag to indicate whether this instance should run as desktop version (true) or server version (false).
     * @param activateBeedo Flag to indicate whether the Beedo engine should be used. Only possible on a desktop version.
     * @exception std::logic_error activateBeedo is true but desktopVersion is false. The Beedo engine can only be used with the desktop version.
     */
    AlarmServerConnector ( const bool desktopVersion = false, const bool activateBeedo = false );
    /**
     * @brief Destructor
     * 
     * Sets the _runwatcher flag to false and waits for the two threads (_watcher and _flashlightthread) to finish their loops.
     */
    ~AlarmServerConnector();
    /**
     * @brief Copy constructor (deleted)
     * 
     * This class cannot be copied.
     * @param other Another instance of AlarmServerConnector
     */
    AlarmServerConnector ( const AlarmServerConnector& other ) = delete;
    /**
     * @brief Move constructor (C++11, deleted)
     * 
     * This class cannot be moved.
     * @param other Another instance of AlarmServerConnector
     */
    AlarmServerConnector ( AlarmServerConnector&& other ) = delete;
    /**
     * @brief Copy assignment (deleted)
     * 
     * This class cannot be copied.
     * @param other Another instance of AlarmServerConnector
     * @return Nothing (deleted)
     */
    AlarmServerConnector& operator= ( const AlarmServerConnector& other ) = delete;
    /**
     * @brief Move assignment (C++11, deleted)
     * 
     * This class cannot be moved.
     * @param other Another instance of AlarmServerConnector
     * @return Nothing (deleted)
     */
    AlarmServerConnector& operator= ( AlarmServerConnector&& other ) = delete;
    /**
     * @brief Notify AlarmServerConnector about alarm status change
     * 
     * This method is invoked by CMSClient to notify this instance about a message received from the CSS Alarm Server.
     * @param status Relevant content of the message put into an AlarmStatusEntry
     * @return Nothing
     */
    void notifyStatusChange ( const AlarmStatusEntry status );
    /**
     * @brief Query number of active alarms
     * 
     * Number of alarm entries in the _statusmap.
     * @return Number of active alarms.
     */
    size_t getNumberOfAlarms() const noexcept;
};

}

#endif // ALARMSERVERCONNECTOR_H
