/**
 * @file desktopalarmwidget.h
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

#include "oldgcccompat.h" // Compatibilty macros for GCC < 4.7

#include <boost/thread.hpp>
#include <QObject>
#include <QFuture>
#include <QSystemTrayIcon>
#include <QWidget>

#ifndef DESKTOPALARMWIDGET_H
#define DESKTOPALARMWIDGET_H

class QMenu; // Forward declarations
class QAction;

namespace AlarmNotifications
{

class AlarmServerConnector; // Forward declaration


/**
 * @brief Abstract base class for desktop alarm widgets
 *
 * This class provides most of the functionality for an application showing desktop notifications to the user when an alarm occurcs. The derived classes only have to provide the interface to the desktop environment to display a tray icon, status notifier item etc.
 */
class DesktopAlarmWidget : public QWidget
{
    Q_OBJECT
public:
    /**
     * @brief Status of the desktop widget
     *
     * The current status can be queried from the derived class by calling getStatus(). This status is used e.g. to display the correct icon in the notification area.
     */
    enum DesktopAlarmWidgetStatus
    {
        /**
         * @brief Widget active, status OK
         *
         * The desktop widget is active and no alarm is present
         */
        ActiveOK = 0,
        /**
         * @brief Widget active, status alarm
         *
         * The desktop widget is active and alarms are currently reported.
         */
        ActiveAlarm = 1,
        /**
         * @brief Widget inactive
         *
         * The desktop widet is inactive, the connection to the alarm server has been closed.
         */
        Disabled = 2
    };
private:
    /**
     * @brief Beedo activation flag
     *
     * Indicates whether to Beedo engine shall be used to provide opto-acoustic alarms. Forwarded to the constructor of AlarmServerConnector.
     */
    const bool _activateBeedo;
    /**
     * @brief Pointer to the AlarmServerConnector instance
     *
     * The actual connection to the CSS Alarm Server and the displaying of desktop notifications is done in the AlarmServerConnector class. The initial instance is created in the constructor. When the desktop widget is disabled, the AlarmServerConnector instance is destroyed and the pointer set to nullptr. This is to close the connection to message broker and save resources when the messages from the alarm server are not needed anyway. When the desktop widget is re-enabled, a new instance is created.
     *
     * As the pointer will be set to nullptr when the widget is disabled and this class uses multithreading, dereferencing and writing to this pointer must ALWAYS be protected by a lock on _ascmutex.
     */
    AlarmServerConnector* _asc;

    /**
     * @brief Thread run flag
     *
     * Set to true in the constructor before spawning _iconThread. When it is set to false, _iconThread will leave its loop and exit.
     */
    bool _run;
    /**
     * @brief Alarm flag
     *
     * Indicates whether there is a currently active alarm.
     */
    bool _alarmActive;
    /**
     * @brief Alarm status observer thread
     *
     * This thread runs the observeAlarmStatus() method that will periodically check the alarm status and signal the derived class on a status change.
     */
    QFuture<void> _iconThread;
    /**
     * @brief Protect access to the _asc pointer
     *
     * Because the _asc pointer will be changed and set to nullptr in this multi-threaded class, a mutex must be used to protect access to the _asc pointer.
     */
    boost::mutex _ascmutex;

    /**
     * @brief Abstract context menu method
     *
     * Derived classes must implement this method to create the context menu for their system tray icon or status notifier item. The context menu should provide an option to enable or disable the notifications via toggleNotifications(), change the timespan between alarm trigger and notification display via configureNotificationTimeout() and close the widget via exitApplication(). Also, a single click on the widget should call showStatusMessage() to display the current status.
     * @return void
     */
    virtual void createContextMenu() = 0;
    /**
     * @brief Thread to periodically check the status
     *
     * This method runs in a separate thread controlled by _iconThread. It continously checks the current status and emits the signal alarmStatusChanged() if anything happens, so the derived classes can act accordingly.
     * @return void
     */
    void observeAlarmStatus();
signals:
    /**
     * @brief Alarm has been triggered or acknowledged
     *
     * This signal is fired by the observeAlarmStatus() method/thread to notify derived classes about a status change.
     * @return Nothing
     */
    void alarmStatusChanged ( );
    /**
     * @brief Widget has been enabled or disabled
     *
     * This signal is fired by the toggleNotifications() method to inform the base classes about a switch change.
     * @param enabled New status of the desktop widget.
     * @return Nothing
     */
    void notificationSwitchChanged ( bool enabled );
protected slots:
    /**
     * @brief Toggles the "main switch"
     *
     * If the widget is currently active, it will disabled, and vice versa. When it is disabled, the instance of AlarmServerConnector will be destroyed and _asc set to nullptr. When it is enabled, a new instance of AlarmServerConnector will be created. Derived classes should connect this slot to the appropriate item in their context menu.
     * @return Nothing
     */
    void toggleNotifications();
    /**
     * @brief Close the desktop widget
     *
     * This tells QApplication to stop the event loop which will cause QApplication::exec() to return in the main method. It will then exit and cause the destruction of all objects, including the derived desktop widget class. Derived classes should connect this slot to the appropriate item in their context menu.
     * @return Nothing
     */
    void exitApplication();
    /**
     * @brief Change the tray icon due to status change
     *
     * This slot is connected to the alarmStatusChanged() signal in the constructor of this abstract base class. Derived classes must implement their logic to change the tray icon here, so that the icon always represents the current status.
     * @return Nothing
     */
    virtual void changeTrayIcon () = 0;
    /**
     * @brief Change timespan between alarm trigger and notification display
     *
     * Shows a window where the user can adjust the DesktopNotificationTimeout setting (see AlarmConfiguration). Derived classes should connect this slot to the appropriate item in their context menu.
     * @return Nothing
     */
    void configureNotificationTimeout();
    /**
     * @brief React on widget enable/disable
     *
     * The constructor of this abstract base class connects this slot to the notificationSwitchChanged() signal. Derived classes must implement their logic to show the proper icon for a disabled alarm widget.
     * @param enabled New status of the desktop alarm widget
     * @return Nothing
     */
    virtual void notificationSwitchChange ( bool enabled ) = 0;
protected:
    /**
     * @brief Show alarm active flag to derived class
     *
     * Derived classes can access _alarmActive through this method. As this flag should only be changed by observeAlarmStatus(), the derived classes have read-only access.
     * @return bool
     */
    inline bool getAlarmActive() const noexcept
    {
        return _alarmActive;
    }
    /**
     * @brief Query alarm status
     *
     * Returns the current status so derived classes can display the correct icon.
     * @return Item of the status enum
     */
    DesktopAlarmWidgetStatus getStatus() const noexcept;
    /**
     * @brief Show status dialog box
     *
     * Shows a message box explaining the current status of the desktop alarm widget to the user.
     * @return Nothing
     */
    void showStatusMessage ();
public:
    /**
     * @brief Constructor
     *
     * Instanciates an initial instance of AlarmServerConnector and spawns the _iconThread running observeAlarmStatus().
     * @param activateBeedo Flag whether to use the Beedo engine
     */
    DesktopAlarmWidget ( const bool activateBeedo = false );
    /**
     * @brief Destructor
     * 
     * Sets the _run flag to false and waits for _iconThread to finish.
     */
    virtual ~DesktopAlarmWidget();
    /**
     * @brief Copy constructor (deleted)
     * 
     * This class cannot be copied.
     * @param other Another instance of DesktopAlarmWidget
     */
    DesktopAlarmWidget ( const DesktopAlarmWidget& other ) = delete;
    /**
     * @brief Move constructor (C++11, deleted)
     * 
     * This class cannot be moved.
     * @param other Another instance of DesktopAlarmWidget
     */
    DesktopAlarmWidget ( DesktopAlarmWidget&& other ) = delete;
    /**
     * @brief Copy assignment (deleted)
     * 
     * This class cannot be copied.
     * @param other Another instance of DesktopAlarmWidget
     * @return Nothing (deleted)
     */
    DesktopAlarmWidget& operator= ( const DesktopAlarmWidget& other ) = delete;
    /**
     * @brief Move assignment (C++11, deleted)
     * 
     * This class cannot be moved.
     * @param other Another instance of DesktopAlarmWidget
     * @return Nothing (deleted)
     */
    DesktopAlarmWidget& operator= ( DesktopAlarmWidget&& other ) = delete;
};

}
#endif // DESKTOPALARMWIDGET_H
