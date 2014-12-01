/**
 * @file desktopalarmwidgetqt.cpp
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

#include "desktopalarmwidgetqt.h"

#include <QAction>
#include <QMenu>

using namespace AlarmNotifications;

DesktopAlarmWidgetQt::DesktopAlarmWidgetQt()
    : DesktopAlarmWidget ( getBeedoActivated() ),
      _trayicon ( this ),
      _contextmenu ( nullptr ),
      _toggleAction ( nullptr ),
      _exitAction ( nullptr )
{
    setStatusIcon ( ActiveOK );
    _trayicon.setToolTip ( QString::fromUtf8 ( "AlarmNotifications Desktop Widget" ) );
    createContextMenu();
    connect ( &_trayicon, SIGNAL ( activated ( QSystemTrayIcon::ActivationReason ) ), this, SLOT ( activated ( QSystemTrayIcon::ActivationReason ) ) );
}

DesktopAlarmWidgetQt::~DesktopAlarmWidgetQt()
{

}

void DesktopAlarmWidgetQt::setStatusIcon ( DesktopAlarmWidget::DesktopAlarmWidgetStatus status )
{
    switch ( status )
    {
    case ActiveOK:
        _trayicon.setIcon ( QIcon ( QString::fromUtf8 ( ":/icons/activeok.png" ) ) );
        break;
    case ActiveAlarm:
        _trayicon.setIcon ( QIcon ( QString::fromUtf8 ( ":/icons/activealarm.png" ) ) );
        break;
    case Disabled:
    default:
        _trayicon.setIcon ( QIcon ( QString::fromUtf8 ( ":/icons/disabled.png" ) ) );
        break;
    }
}

void DesktopAlarmWidgetQt::createContextMenu()
{
    _contextmenu = new QMenu ( QString::fromUtf8 ( "Desktop Alarm Notifications" ), nullptr );
    _toggleAction = _contextmenu->addAction (
                        QIcon ( QString::fromUtf8 ( ":/icons/disable.png" ) ),
                        QString::fromUtf8 ( "Disable &notifications" ),
                        this,
                        SLOT ( toggleNotifications() )
                    );
    _configureAction = _contextmenu->addAction (
                           QIcon ( QString::fromUtf8 ( ":/icons/configure.png" ) ),
                           QString::fromUtf8 ( "&Configure notification timeout" ),
                           this,
                           SLOT ( configureNotificationTimeout() )
                       );
    _exitAction = _contextmenu->addAction (
                      QIcon ( QString::fromUtf8 ( ":/icons/exit.png" ) ),
                      QString::fromUtf8 ( "&Exit desktop alarm widget" ),
                      this,
                      SLOT ( exitApplication() )
                  );
    _trayicon.setContextMenu ( _contextmenu );
    _trayicon.show();
}

void DesktopAlarmWidgetQt::activated ( const QSystemTrayIcon::ActivationReason reason )
{
    if ( reason != QSystemTrayIcon::Trigger )
        return; // Only react on single click
    showStatusMessage();
}

void DesktopAlarmWidgetQt::notificationSwitchChange ( bool enabled )
{
    if ( enabled )
    {
        _toggleAction->setIcon ( QIcon ( QString::fromUtf8 ( ":/icons/disable.png" ) ) );
        _toggleAction->setText ( QString::fromUtf8 ( "Disable &notifications" ) );
        setStatusIcon ( ActiveOK ); // The newly generated AlarmServerConnector will have zero alarms...
    }
    else
    {
        _toggleAction->setIcon ( QIcon ( QString::fromUtf8 ( ":/icons/enable.png" ) ) );
        _toggleAction->setText ( QString::fromUtf8 ( "Enable &notifications" ) );
        setStatusIcon ( Disabled );
    }
}

void DesktopAlarmWidgetQt::changeTrayIcon()
{
    if ( getAlarmActive() )
        setStatusIcon ( ActiveAlarm );
    else
        setStatusIcon ( ActiveOK );
}

bool DesktopAlarmWidgetQt::getBeedoActivated() noexcept
{
#ifndef BEEDO
    return false;
#else
    return true;
#endif
}

#include "desktopalarmwidgetqt.moc"
