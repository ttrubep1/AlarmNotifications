/**
 * @file desktopalarmwidgetqt.h
 *
 * @author Tobias Triffterer
 *
 * @brief Implementation if DesktopAlarmWidget for QSystemTrayIcon
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

#ifndef DESKTOPALARMWIDGETQT_H
#define DESKTOPALARMWIDGETQT_H

#include "oldgcccompat.h" // Compatibilty macros for GCC < 4.7

#include <QSystemTrayIcon>

#include "desktopalarmwidget.h"

namespace AlarmNotifications
{

/**
 * @brief Desktop widget featuring a Qt system tray icon
 *
 * This class implements the DesktopAlarmWidget interface by using a QSystemTrayIcon. This implementation should work on almost any desktop operating system supported by Qt. The main work is done in the base class, this class does only the interaction with the GUI.
 *
 * Users of a recent version of KDE4 may prefer the status notifier item provided by DesktopAlarmWidgetKde4.
 */
class DesktopAlarmWidgetQt : public DesktopAlarmWidget
{
    Q_OBJECT
private:
    /**
     * @brief Tray icon object
     *
     * Creates an icon in the sytem tray or notification area.
     */
    QSystemTrayIcon _trayicon;
    /**
     * @brief Tray icon context menu
     *
     * The menu opened by a right click on the tray icon.
     */
    QMenu* _contextmenu;
    /**
     * @brief Context menu entry to toggle the desktop widget
     *
     * Using this action in the context menu, the user can switch notifications on and off.
     */
    QAction* _toggleAction;
    /**
     * @brief Context menu entry to configure notification timeout
     *
     * Using this action in the context menu, the user can configure the timespan between the occurence of an alarm and the display of a notification.
     */
    QAction* _configureAction;
    /**
     * @brief Context menu entry to close the application.
     *
     * Using this action in the context menu, the user can close the AlarmNotifications desktop widget.
     */
    QAction* _exitAction;

    /**
     * @brief Create context menu objects
     *
     * This method is called by the constructor and creates the _contextmenu and its entries.
     * @return Nothing
     */
    virtual void createContextMenu();
    /**
     * @brief Adjust tray icon
     *
     * This method is called internally when this derived class is notified by the base class of a status change. It will load and display the proper icon for the new status.
     * @param status New status of the alarm widget
     * @return Nothing
     */
    void setStatusIcon ( DesktopAlarmWidgetStatus status );
    /**
     * @brief Return compile-time Beedo activation flag
     *
     * The desktop widgets can be compiled in two flavours: The standard one just using desktop notifications and the "beamtime" flavour that uses the Beedo engine to provide an opto-acoustic notification, e.g. in a control room.
     *
     * This static function makes the compile-time switch "-DBEEDO" accessible.
     * @return True if Beedo engine shall be activated.
     */
    static bool getBeedoActivated() noexcept;
private slots:
    /**
     * @brief React on click on tray icon
     *
     * This method is connected to the activated signal of the QSystemTrayIcon and reacts on a click on the icon by invoking showStatusMessage() to display a message to the user.
     * @param reason Type of interaction as defined within Qt
     * @return Nothing
     */
    void activated ( const QSystemTrayIcon::ActivationReason reason );
    /**
     * @brief React on widget enable/disable
     *
     * The constructor of this abstract base class connects this slot to the notificationSwitchChanged() signal. This method reacts on such a change by switching the icon.
     * @param enabled New status of the desktop alarm widget
     * @return Nothing
     */
    virtual void notificationSwitchChange ( bool enabled );
    /**
     * @brief Change the tray icon due to status change
     *
     * This slot is connected to the alarmStatusChanged() signal in the constructor of this abstract base class. This method reacts on a status change by exchanging the alarm icon.
     * @return Nothing
     */
    virtual void changeTrayIcon();
public:
    /**
     * @brief Constructor
     *
     * Queries the Beedo activation, initializes the base class and sets up the QSystemTrayIcon.
     *
     */
    DesktopAlarmWidgetQt();
    /**
     * @brief Destructor
     *
     * Has nothing to do here, all work in done in the base class.
     */
    ~DesktopAlarmWidgetQt();
    /**
     * @brief Copy constructor (deleted)
     *
     * This class cannot be copied.
     * @param other Another instance of DesktopAlarmWidgetQt
     */
    DesktopAlarmWidgetQt ( const DesktopAlarmWidgetQt& other ) = delete;
    /**
     * @brief Move constructor (C++11, deleted)
     *
     * This class cannot be moved.
     * @param other Another instance of DesktopAlarmWidgetQt
     */
    DesktopAlarmWidgetQt ( DesktopAlarmWidgetQt&& other ) = delete;
    /**
     * @brief Copy assignment (deleted)
     *
     * This class cannot be copied.
     * @param other Another instance of DesktopAlarmWidgetQt
     * @return Nothing (deleted)
     */
    DesktopAlarmWidgetQt& operator= ( const DesktopAlarmWidgetQt& other ) = delete;
    /**
     * @brief Move assignment (deleted)
     *
     * This class cannot be moved.
     * @param other Another instance of DesktopAlarmWidgetQt
     * @return Nothing (deleted)
     */
    DesktopAlarmWidgetQt& operator= ( DesktopAlarmWidgetQt&& other ) = delete;
};

}

#endif // DESKTOPALARMWIDGETQT_H
