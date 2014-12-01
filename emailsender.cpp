/**
 * @file emailsender.cpp
 *
 * @author Tobias Triffterer
 *
 * @brief Send alarm notifications via e-mail
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

#include "emailsender.h"

#include <iostream>
#include <stdexcept>
#include <string>

#include "alarmconfiguration.h"
#include "mimemessage.h"
#include "mimetext.h"
#include "smtpclient.h"

using namespace AlarmNotifications;

EMailSender& EMailSender::instance() noexcept
{
    static EMailSender global_instance;
    return global_instance;
}

void EMailSender::sendAlarmNotification ( const std::vector< AlarmStatusEntry > alarms ) noexcept
{
    try {
        instance().sendAlarmNotification_internal ( std::move ( alarms ) );
    }
    catch ( std::exception& e )
    {
        std::cerr << "Exception in e-mail sending procedure: " << e.what() << std::endl;
    }
    catch ( ... )
    {
        std::cerr << "Unknown error in e-mail sending procedure: " << std::endl;
    }
}

EMailSender::EMailSender()
{

}

EMailSender::~EMailSender()
{

}

void EMailSender::sendAlarmNotification_internal ( const std::vector< AlarmStatusEntry > alarms )
{
    SmtpClient smtp (
        QString::fromUtf8 ( AlarmConfiguration::instance().getEMailNotificationServerName().c_str() ),
        AlarmConfiguration::instance().getEMailNotificationServerPort(),
        SmtpClient::TcpConnection
    );
    EmailAddress sender (
        QString::fromUtf8 ( AlarmConfiguration::instance().getEMailNotificationFrom().c_str() ),
        QString::fromUtf8 ( "Alarm Notification Daemon" )
    );
    EmailAddress recipient (
        QString::fromUtf8 ( AlarmConfiguration::instance().getEMailNotificationTo().c_str() ),
        QString::fromUtf8 ( "Alarm Notification Mailing List" )
    );
    MimeMessage email;
    email.setSender ( &sender );
    email.addTo ( &recipient );
    email.setSubject ( "Detector Control System Alarm" );
    MimeText text;
    text.setEncoding(MimePart::QuotedPrintable);
    text.setCharset(QString::fromUtf8("utf8"));
    text.setText ( composeMessageText ( alarms ) );
    email.addPart ( &text );

    std::cout << "Sending alarm notification by e-mail!" << std::endl;
    const bool connection = smtp.connectToHost();
    if ( !connection )
    {
        QString errormessage = QString::fromUtf8 ( "An error occured while connecting to the SMTP server.\n" );
        errormessage += "Server response: " + smtp.getResponseCode() + QString::fromUtf8 ( " " ) + smtp.getResponseText();
        std::string stderrormessage ( errormessage.toUtf8().data() );
        throw std::runtime_error ( stderrormessage );
    }
    const bool sending = smtp.sendMail(email);
    if ( !sending )
    {
        QString errormessage = QString::fromUtf8 ( "An error occured while sending the e-mail.\n" );
        errormessage += "Server response: " + smtp.getResponseCode() + QString::fromUtf8 ( " " ) + smtp.getResponseText();
        std::string stderrormessage ( errormessage.toUtf8().data() );
        throw std::runtime_error ( stderrormessage );
    }
    smtp.quit();
}

QString EMailSender::composeMessageText ( const std::vector< AlarmStatusEntry >& alarms )
{
    QString text;
    text += QString::fromUtf8 ( "Hello,\n\nthe following PV(s) triggered an alarm:\n\n" );
    for ( auto i = alarms.begin(); i != alarms.end(); i++ )
    {
        text+= QString::fromUtf8 ( ( *i ).getPVName().c_str() ) + QString::fromUtf8 ( "\n" );
    }
    text += QString::fromUtf8 ( "\nPlease remember to acknowledge the alarms if you go solving the problem.\n\n\nYour Alarm Notification Service\n" );
    return text;
}
