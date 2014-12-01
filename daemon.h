/**
 * @file daemon.h
 *
 * @author Tobias Triffterer
 *
 * @brief AlarmNotifications daemon implementation
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

#ifndef DAEMON_H
#define DAEMON_H

#include <signal.h>

#include "alarmserverconnector.h"

namespace AlarmNotifications
{

/**
 * @brief Alarm Notification daemon
 *
 * This class implements a daemon that can be run in the background. It instanciates the AlarmServerConnector class that will send laboratory, desktop and e-mail Notifications according to the AlarmConfiguration.
 *
 * The daemon is implemented as a singleton. It will take control of several POSIX signals such as SIGINT that is caused by pressing Ctrl+C. This way, the program will not be aborted, but the daemon's run() method will stop which will cause the entry point method (int main) to reach its end. The C++ runtime will then destroy all the global objects including this singleton, so the connection to the Java Message Service established by the CMSClient will be closed properly.
 */
class Daemon
{
private:
    /**
     * @brief Timeout for daemon status message
     *
     * This value, given in seconds, decides how often the daemon will print a status message to stdout. During this wakeup, it will also check if _run has been set to false by the signalReceiver start the exit sequence.
     */
    static const unsigned short int DaemonSleepTimeout = 3; // seconds
    /**
     * @brief Global daemon run flag
     *
     * It is set to true in the constructor and set to false when signalReceiver gets a POSIX signal. Setting this variable to false will cause the daemon process to terminate gracefully.
     */
    bool _run;
    /**
     * @brief Original SIGINT handler
     *
     * The original handler function for the POSIX signal SIGINT will be stored here and reconstructed from here in the signalReceiver.
     */
    __sighandler_t hsigint;
    /**
     * @brief Original SIGHUP handler
     *
     * The original handler function for the POSIX signal SIGHUP will be stored here and reconstructed from here in the signalReceiver.
     */
    __sighandler_t hsighup;
    /**
     * @brief Original SIGQUIT handler
     *
     * The original handler function for the POSIX signal SIGQUIT will be stored here and reconstructed from here in the signalReceiver.
     */
    __sighandler_t hsigquit;
    /**
     * @brief Original SIGUSR1 handler
     *
     * The original handler function for the POSIX signal SIGUSR1 will be stored here and reconstructed from here in the signalReceiver.
     */
    __sighandler_t hsigusr1;
    /**
     * @brief Original SIGUSR2 handler
     *
     * The original handler function for the POSIX signal SIGUSR2 will be stored here and reconstructed from here in the signalReceiver.
     */
    __sighandler_t hsigusr2;
    /**
     * @brief Original SIGTERM handler
     *
     * The original handler function for the POSIX signal SIGTERM will be stored here and reconstructed from here in the signalReceiver.
     */
    __sighandler_t hsigterm;
    /**
     * @brief Connection to the CSS Alarm Server
     *
     * This is the central instance of the AlarmServerConnector that administrates the connection to BEAST and sends out all the Notifications as configured.
     */
    AlarmServerConnector _asc;

    /**
     * @brief POSIX signal handler
     *
     * Method to be registered with the operating system to be invoked whenever a POSIX signal such as SIGINT, SIGHUP, SIGQUIT, SIGUSR1, SIGUSR2 or SIGTERM is received.
     *
     * Upon reception of such a signal, it will set the _run flag to tell the daemon process to stop and then restore the original handlers for the aforementioned signals. This way, the application can be killed by another Ctrl+C if for some reason the C++ runtime fails to exit properly.
     * @param signum Signal number as defined in the POSIX standard
     * @return Nothing
     */
    static void signalReceiver ( int signum );
    /**
     * @brief Constructor
     * 
     * Sets the _run flag to true and installs signalReceiver as the new POSIX signal handler for several signals while backing up the original signal handlers.
     */
    Daemon();
public:
    /**
     * @brief Get singleton instance
     *
     * This static method returns a reference (not a pointer!) to the singleton instance of the daemon class. On the first call to instance(), this singleton instance will be created and the constructor will be called. If an exception occurs in the constructor, instance() will call ExceptionHandler() to print an error message and then terminate the application.
     * @return A reference to the singleton instance
     **/
    static Daemon& instance();
    /**
     * @brief Destructor
     *
     * Writes a shutdown notice with date and time.
     */
    ~Daemon();
    /**
     * @brief Copy constructor (deleted)
     *
     * This class cannot be copied.
     * @param other Another instance of Daemon
     */
    Daemon ( const Daemon& other ) = delete;
    /**
     * @brief Move constructor (C++11, deleted)
     *
     * This class cannot be moved.
     * @param other Another instance of Daemon
     */
    Daemon ( Daemon&& other ) = delete;
    /**
     * @brief Copy assignment (deleted)
     *
     * This class cannot be copied.
     * @param other Another instance of Daemon
     * @return Nothing (deleted)
     */
    Daemon& operator= ( const Daemon& other ) = delete;
    /**
     * @brief Move assignment (C+11, deleted)
     *
     * This class cannot be moved.
     * @param other Another instance of Daemon
     * @return Nothing (deleted)
     */
    Daemon& operator= ( Daemon&& other ) = delete;
    /**
     * @brief Main daemon loop
     *
     * This method will run a loop that prints a status message every DaemonSleepTimeout seconds. When _run is set to false, the loop will be terminated and the daemon process will exit.
     * @return Nothing
     */
    void run();
};

}

#endif // DAEMON_H
