/**
 * @file configscreen.cpp
 *
 * @author Tobias Triffterer
 *
 * @brief GUI dialog to configure this application
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
#include <QtGui/QDoubleSpinBox>
#include <QtGui/QVBoxLayout>

#include "configscreen.h"

using namespace AlarmNotifications;

ConfigScreen::ConfigScreen ( )
    : KConfigDialog ( nullptr, "AlarmNotifications Configuration", AlarmConfiguration::instance().internal_skel() ),
      _conf ( AlarmConfiguration::instance().internal_skel() ),
      _confman ( new KConfigDialogManager ( this, _conf ) ),
      _activemqscreen ( new QWidget ),
      _lactivemqscreen ( new QFormLayout ( _activemqscreen ) )
{
    CreateActiveMQConnectivityWidgets();

    _activemqscreen->setLayout ( _lactivemqscreen );
    _confman->updateWidgets();
    setButtons ( KDialog::ButtonCode::Ok | KDialog::ButtonCode::Apply | KDialog::ButtonCode::Cancel );
    addPage ( _activemqscreen, _conf, "Alarm notifications", "mail-forwarded", "Alarm notification settings" );
    setWindowTitle ( "AlarmNotifications Configuration" );
    setWindowIcon ( KIcon ( "preferences-system", nullptr ) );
}

ConfigScreen::~ConfigScreen()
{

}

void ConfigScreen::CreateActiveMQConnectivityWidgets()
{
    QLineEdit* activemquri = new QLineEdit ( _activemqscreen );
    activemquri->setObjectName ( "kcfg_ActiveMQURI" );
    _lactivemqscreen->addRow ( "ActiveMQ URI:", activemquri );
    _confman->addWidget ( activemquri );
    QLineEdit* activemqusername = new QLineEdit ( _activemqscreen );
    activemqusername->setObjectName ( "kcfg_ActiveMQUsername" );
    _lactivemqscreen->addRow ( "ActiveMQ username:", activemqusername );
    _confman->addWidget ( activemqusername );
    QLineEdit* activemqpassword = new QLineEdit ( _activemqscreen );
    activemqpassword->setObjectName ( "kcfg_ActiveMQPassword" );
    _lactivemqscreen->addRow ( "ActiveMQ password:", activemqpassword );
    _confman->addWidget ( activemqpassword );
    QLineEdit* activemqtopicname = new QLineEdit ( _activemqscreen );
    activemqtopicname->setObjectName ( "kcfg_ActiveMQTopicName" );
    _lactivemqscreen->addRow ( "CSS Alarm Server topic name:", activemqtopicname );
    _confman->addWidget ( activemqtopicname );
    QSpinBox* laboratorynotificationtimeout = new QSpinBox ( _activemqscreen );
    laboratorynotificationtimeout->setMinimum ( 0 );
    laboratorynotificationtimeout->setMaximum ( 3600 );
    laboratorynotificationtimeout->setSuffix ( QString::fromUtf8 ( " seconds" ) );
    laboratorynotificationtimeout->setSpecialValueText ( QString::fromUtf8 ( "Notification disabled" ) );
    laboratorynotificationtimeout->setObjectName ( "kcfg_LaboratoryNotificationTimeout" );
    _lactivemqscreen->addRow ( "Laboratory notification timeout:", laboratorynotificationtimeout );
    _confman->addWidget ( laboratorynotificationtimeout );
    QSpinBox* desktopnotificationtimeout = new QSpinBox ( _activemqscreen );
    desktopnotificationtimeout->setMinimum ( 0 );
    desktopnotificationtimeout->setMaximum ( 3600 );
    desktopnotificationtimeout->setSuffix ( QString::fromUtf8 ( " seconds" ) );
    desktopnotificationtimeout->setSpecialValueText ( QString::fromUtf8 ( "Notification disabled" ) );
    desktopnotificationtimeout->setObjectName ( "kcfg_DesktopNotificationTimeout" );
    _lactivemqscreen->addRow ( "Desktop notification timeout:", desktopnotificationtimeout );
    _confman->addWidget ( desktopnotificationtimeout );
    QSpinBox* emailnotificationtimeout = new QSpinBox ( _activemqscreen );
    emailnotificationtimeout->setMinimum ( 0 );
    emailnotificationtimeout->setMaximum ( 3600 );
    emailnotificationtimeout->setSuffix ( QString::fromUtf8 ( " seconds" ) );
    emailnotificationtimeout->setSpecialValueText ( QString::fromUtf8 ( "Notification disabled" ) );
    emailnotificationtimeout->setObjectName ( "kcfg_EMailNotificationTimeout" );
    _lactivemqscreen->addRow ( "E-Mail notification timeout:", emailnotificationtimeout );
    _confman->addWidget ( emailnotificationtimeout );
    QLineEdit* emailnotificationfrom = new QLineEdit ( _activemqscreen );
    emailnotificationfrom->setObjectName ( "kcfg_EMailNotificationFrom" );
    _lactivemqscreen->addRow ( "E-Mail notification sender address:", emailnotificationfrom );
    _confman->addWidget ( emailnotificationfrom );
    QLineEdit* emailnotificationto = new QLineEdit ( _activemqscreen );
    emailnotificationto->setObjectName ( "kcfg_EMailNotificationTo" );
    _lactivemqscreen->addRow ( "E-Mail notification recipient address:", emailnotificationto );
    _confman->addWidget ( emailnotificationto );
    QLineEdit* emailnotificationservername = new QLineEdit ( _activemqscreen );
    emailnotificationservername->setObjectName ( "kcfg_EMailNotificationServerName" );
    _lactivemqscreen->addRow ( "SMTP server name:", emailnotificationservername );
    _confman->addWidget ( emailnotificationservername );
    QSpinBox* emailnotificationserverport = new QSpinBox ( _activemqscreen );
    emailnotificationserverport->setMinimum ( 0 ); // Minimum port number in TCP standard
    emailnotificationserverport->setMaximum ( 65535 ); // Maximum port number in TCP standard
    emailnotificationserverport->setObjectName ( "kcfg_EMailNotificationServerPort" );
    _lactivemqscreen->addRow ( "SMTP server port:", emailnotificationserverport );
    _confman->addWidget ( emailnotificationserverport );
    QLineEdit* flashlightrelaisdevicenode = new QLineEdit ( _activemqscreen );
    flashlightrelaisdevicenode->setObjectName ( QString::fromUtf8 ( "kcfg_FlashLightRelaisDeviceNode" ) );
    _lactivemqscreen->addRow ( QString::fromUtf8 ( "Device node of relais for red flash light:" ), flashlightrelaisdevicenode );
    _confman->addWidget ( flashlightrelaisdevicenode );
}

#include "configscreen.moc"
