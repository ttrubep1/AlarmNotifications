/**
 * @file desktopalarmwidgetkde4.h
 *
 * @author Tobias Triffterer
 *
 * @brief Implementation if DesktopAlarmWidget for KStatusNotifierItem (KDE4 Plasma Workspaces)
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

#ifndef DESKTOPALARMWIDGETKDE4_H
#define DESKTOPALARMWIDGETKDE4_H

#include "oldgcccompat.h" // Compatibilty macros for GCC < 4.7

#include <QAction>
#include <KDE/KMenu>
#include <KDE/KStatusNotifierItem>

#include "desktopalarmwidget.h"

namespace AlarmNotifications
{

/**
 * @brief Desktop widget featuring an item in the KDE4 notification area
 *
 * This version of the desktop widget provides a KDE4 status notifier item which has extended capabilities in comparision to the pure QSystemTrayIcon provided by the class DesktopAlarmWidgetQt.
 *
 * The KDE project has defined a new protocol for using the notification area: Application cannot draw their icon themselves anymore but they have to use inter-process communication via D-Bus to ask the desktop environment to do so. The advantage is that the applications cannot only draw an icon, but also provide a status. According to the status and the user's configuration the desktop enviroment can automatically show and hide icons to prevent overpopulation of the notification are.
 *
 * This class uses this status system as follows: If everything is fine, the status is set to Passive what usually hides the icon. If the widget is disabled the status is set to Active an in case of an alarm the status is set to NeedsAttention what makes the icon not only visible but in addition blink.
 */
class DesktopAlarmWidgetKde4 :  public DesktopAlarmWidget
{
    Q_OBJECT
private:
    /**
     * @brief Tray icon object
     *
     * Creates an icon in the notification area by using the new DBus-based protocotol defined by KDE.
     */
    KStatusNotifierItem _trayicon;
    /**
     * @brief Tray icon context menu
     *
     * The menu opened by a right click on the tray icon.
     */
    KMenu* _contextmenu;
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
     * @brief Adjust tray icon and tooltip message
     *
     * This method is called internally when this derived class is notified by the base class of a status change. It will load and display the proper icon for the new status and also adjust the tooltip message to tell the user about the alarm status.
     * @param status New status of the alarm widget
     * @return Nothing
     */
    void setStatusIconAndTooltip ( DesktopAlarmWidgetStatus status );
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
     * @brief React on widget enable/disable
     *
     * The constructor of this abstract base class connects this slot to the notificationSwitchChanged() signal. This method reacts on such a change by switching the icon.
     * @param enabled New status of the desktop alarm widget
     * @return Nothing
     */
    void notificationSwitchChange ( bool enabled );
    /**
     * @brief Change the tray icon due to status change
     *
     * This slot is connected to the alarmStatusChanged() signal in the constructor of this abstract base class. This method reacts on a status change by exchanging the alarm icon.
     * @return Nothing
     */
    void changeTrayIcon();
    /**
     * @brief React on click on tray icon
     *
     * This method is connected to the activateRequest signal of the KStatusNotifierItem and reacts on a click on the icon by invoking showStatusMessage() to display a message to the user.
     * @param active Activiation flag of the status item
     * @param pos Position of the mouse pointer at the time of the click
     * @return Nothing
     */
    void activated ( bool active, const QPoint& pos );
public:
    /**
     * @brief Constructor
     *
     * Queries the Beedo activation, initializes the base class and sets up the KStatusNotifierItem.
     *
     */
    DesktopAlarmWidgetKde4();
    /**
     * @brief Destructor
     *
     * Has nothing to do here, all work in done in the base class.
     */
    ~DesktopAlarmWidgetKde4();
    /**
     * @brief Copy constructor (deleted)
     *
     * This class cannot be copied.
     * @param other Another instance of DesktopAlarmWidgetKde4
     */
    DesktopAlarmWidgetKde4 ( const DesktopAlarmWidgetKde4& other ) = delete;
    /**
     * @brief Move constructor (C++11, deleted)
     *
     * This class cannot be moved.
     * @param other Another instance of DesktopAlarmWidgetKde4
     */
    DesktopAlarmWidgetKde4 ( DesktopAlarmWidgetKde4&& other ) = delete;
    /**
     * @brief Copy assignment (deleted)
     *
     * This class cannot be copied.
     * @param other Another instance of DesktopAlarmWidgetKde4
     * @return Nothing (deleted)
     */
    DesktopAlarmWidgetKde4& operator= ( const DesktopAlarmWidgetKde4& other ) = delete;
    /**
     * @brief Move assignment (deleted)
     *
     * This class cannot be moved.
     * @param other Another instance of DesktopAlarmWidgetKde4
     * @return Nothing (deleted)
     */
    DesktopAlarmWidgetKde4& operator= ( DesktopAlarmWidgetKde4&& other ) = delete;
};
}

#endif // DESKTOPALARMWIDGETKDE4_H
