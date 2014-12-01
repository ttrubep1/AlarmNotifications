/**
 * @file emailsender.h
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

#ifndef EMAILSENDER_H
#define EMAILSENDER_H

#include "oldgcccompat.h" // Compatibilty macros for GCC < 4.7

#include <vector>

#include <QString>

#include "alarmstatusentry.h"

namespace AlarmNotifications
{

/**
 * @brief Send alarm notifications via e-mail
 *
 * This class encapsulates the code that produces an e-mail notification that can be sent to a mailing list to inform the staff about an alarm that occured while nobody was in the laboratory.
 *
 * The actual SMTP client work is done by the classes of the SmtpClient-for-Qt project, The code has been written by "bluetiger9" on GitHub and is available here: https://www.github.com/bluetiger9/SmtpClient-for-Qt. It is licensed unter the GNU Lesser General Public License, version 2.1.
 *
 * The task of this class is to compose the message text and instruct the SmtpClient properly to have the e-mail sent. The parameters for the connection to the SMTP server are read from the AlarmConfiguration class.
 */
class EMailSender
{
private:
    /**
     * @brief Constructor
     *
     * Has nothing to do here, actually...
     */
    EMailSender();
    /**
     * @brief Compose and send the e-mail notification
     *
     * This method is invoked by sendAlarmNotification() and does the actual work. It reads the necessary configuration parameters, creates a connection to the SMTP server, assembles the e-mail message and sends it.
     * @param alarms Alarms to be listed in the e-mail
     * @return Nothing
     */
    void sendAlarmNotification_internal ( const std::vector< AlarmStatusEntry > alarms );
    /**
     * @brief Compose message text
     *
     * This method creates the text to be put into the body of the alarm notification e-mail.
     * @param alarms Alarm to be listed in the e-mail
     * @return Message body text as QString
     */
    QString composeMessageText ( const std::vector< AlarmStatusEntry >& alarms );
public:
    /**
     * @brief Get singleton instance
     *
     * Returns a reference (not a pointer) to the single global instance of this class.
     * @return Reference to singleton instance
     */
    static EMailSender& instance() noexcept;
    /**
     * @brief Destructor
     *
     * Has nothing to do here, actually...
     */
    ~EMailSender();
    /**
     * @brief Copy constructor (deleted)
     *
     * This class cannot be copied.
     * @param other Another instance of EMailSender
     */
    EMailSender ( const EMailSender& other ) = delete;
    /**
     * @brief Move constructor (C++11, deleted)
     *
     * This class cannot be moved.
     * @param other Another instance of EMailSender
     */
    EMailSender ( EMailSender&& other ) = delete;
    /**
     * @brief Copy assignment (deleted)
     *
     * This class cannot be copied.
     * @param other Another instance of EMailSender
     * @return Nothing (deleted)
     */
    EMailSender& operator= ( const EMailSender& other ) = delete;
    /**
     * @brief Move assignment (C++11, deleted)
     *
     * This class cannot be moved.
     * @param other Another instance of EMailSender
     * @return Nothing (deleted)
     */
    EMailSender& operator= ( EMailSender&& other ) = delete;
    /**
     * @brief Send an alarm notification via e-mail
     *
     * This static method gets a reference to the global instance of EMailSender and invokes sendAlarmNotification_internal() to send an e-mail to tell the staff about the alarms.
     * @param alarms Alarms to be listed in the e-mail
     * @return Nothing
     */
    static void sendAlarmNotification ( const std::vector<AlarmStatusEntry> alarms ) noexcept;
};

}
#endif // EMAILSENDER_H
