/**
 * @file desktopalarmwidgetkde4.cpp
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

#include "desktopalarmwidgetkde4.h"

using namespace AlarmNotifications;

DesktopAlarmWidgetKde4::DesktopAlarmWidgetKde4()
    : DesktopAlarmWidget ( getBeedoActivated() ),
      _trayicon ( QString::fromUtf8 ( "AlarmNotificationsStatus" ), this ),
      _contextmenu ( nullptr )
{
    _trayicon.setStandardActionsEnabled ( false );
    _trayicon.setAssociatedWidget ( nullptr );
    _trayicon.setCategory ( KStatusNotifierItem::SystemServices );
    _trayicon.setTitle ( QString::fromUtf8 ( "AlarmNotifications Desktop Widget" ) );
    setStatusIconAndTooltip ( ActiveOK );
    createContextMenu();

    connect ( &_trayicon, SIGNAL ( activateRequested ( bool,QPoint ) ), this, SLOT ( activated ( bool,QPoint ) ) );
}

DesktopAlarmWidgetKde4::~DesktopAlarmWidgetKde4()
{

}

void DesktopAlarmWidgetKde4::createContextMenu()
{
    _contextmenu = new KMenu ( QString::fromUtf8 ( "Desktop Alarm Notifications" ), nullptr );
    _toggleAction = _contextmenu->addAction (
                        QIcon::fromTheme ( QString::fromUtf8 ( "dialog-cancel" ) ),
                        QString::fromUtf8 ( "Disable &notifications" ),
                        this,
                        SLOT ( toggleNotifications() )
                    );
    _configureAction = _contextmenu->addAction (
                           QIcon::fromTheme ( QString::fromUtf8 ( "configure" ) ),
                           QString::fromUtf8 ( "&Configure notification timeout" ),
                           this,
                           SLOT ( configureNotificationTimeout() )
                       );
    _exitAction = _contextmenu->addAction (
                      QIcon::fromTheme ( QString::fromUtf8 ( "application-exit" ) ),
                      QString::fromUtf8 ( "&Exit desktop alarm widget" ),
                      this,
                      SLOT ( exitApplication() )
                  );
    _trayicon.setContextMenu ( _contextmenu );
}

void DesktopAlarmWidgetKde4::setStatusIconAndTooltip ( DesktopAlarmWidget::DesktopAlarmWidgetStatus status )
{
    switch ( status )
    {
    case ActiveOK:
        _trayicon.setIconByName ( QString::fromUtf8 ( "help-feedback" ) );
        _trayicon.setStatus ( KStatusNotifierItem::Passive );
        _trayicon.setToolTip ( QString::fromUtf8 ( "help-feedback" ), QString::fromUtf8 ( "AlarmNotifications Desktop Widget" ), QString::fromUtf8 ( "No active alarm in the Detector Control System." ) );
        break;
    case ActiveAlarm:
        _trayicon.setIconByName ( QString::fromUtf8 ( "dialog-warning" ) );
        _trayicon.setStatus ( KStatusNotifierItem::NeedsAttention );
        _trayicon.setToolTip ( QString::fromUtf8 ( "dialog-warning" ), QString::fromUtf8 ( "AlarmNotifications Desktop Widget" ), QString::fromUtf8 ( "ATTENTION! There are active alarms in the Detector Control System!" ) );
        break;
    case Disabled:
    default:
        _trayicon.setIconByName ( QString::fromUtf8 ( "face-plain" ) );
        _trayicon.setStatus ( KStatusNotifierItem::Active );
        _trayicon.setToolTip ( QString::fromUtf8 ( "face-plain" ), QString::fromUtf8 ( "AlarmNotifications Desktop Widget" ), QString::fromUtf8 ( "Alarm notifications disabled!" ) );
        break;
    }
}

void DesktopAlarmWidgetKde4::notificationSwitchChange ( bool enabled )
{
    if ( enabled )
    {
        _toggleAction->setIcon ( QIcon::fromTheme ( QString::fromUtf8 ( "dialog-cancel" ) ) );
        _toggleAction->setText ( QString::fromUtf8 ( "Disable &notifications" ) );
        setStatusIconAndTooltip ( ActiveOK ); // The newly generated AlarmServerConnector will have zero alarms...
    }
    else
    {
        _toggleAction->setIcon ( QIcon::fromTheme ( QString::fromUtf8 ( "dialog-ok-apply" ) ) );
        _toggleAction->setText ( QString::fromUtf8 ( "Enable &notifications" ) );
        setStatusIconAndTooltip ( Disabled );
    }
}

void DesktopAlarmWidgetKde4::changeTrayIcon()
{
    if ( getAlarmActive() )
        setStatusIconAndTooltip ( ActiveAlarm );
    else
        setStatusIconAndTooltip ( ActiveOK );
}

void DesktopAlarmWidgetKde4::activated ( bool active, const QPoint& posparameter )
{
    Q_UNUSED ( active )
    Q_UNUSED ( posparameter )
    showStatusMessage();
}

bool DesktopAlarmWidgetKde4::getBeedoActivated() noexcept
{
#ifndef BEEDO
    return false;
#else
    return true;
#endif
}

#include "desktopalarmwidgetkde4.moc"
