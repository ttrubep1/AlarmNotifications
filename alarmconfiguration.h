/**
 * @file alarmconfiguration.h
 *
 * @author Tobias Triffterer
 *
 * @brief Singleton to read and change the configuration of this application
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

#ifndef ALARMCONFIGURATION_H
#define ALARMCONFIGURATION_H

#include "oldgcccompat.h" // Compatibilty macros for GCC < 4.7

#include <deque>
#include <vector>
#include <QtCore/QString>
#include <KDE/KSharedConfig>
#include <KDE/KConfigGroup>
#include <KDE/KConfigSkeleton>

namespace AlarmNotifications
{

/**
 * @class AlarmConfiguration
 * @brief Configuration of the AlarmNotifications application
 *
 * This singleton class uses the KConfig framework to read and write the configuration of the AlarmNotifications. Every configuration item has a read and a write method whose names begin with "get" and "set" respectively. To write a changed configuration to disk, the WriteConfiguration() method has to be called. To update the configuration in the memory after it was changed on disk by another instance, the ReReadConfiguration() method must be used.
 *
 * The KConfig framework also provides the KConfigDialog class, which is used to create the ConfigScreen. It uses the methods internal() and internal_skel(), which are not part of the public API.
 * According to the principles of object-oriented programming, the KConfig framework can be exchanged without affecting the public API of the AlarmConfiguration, i.e. only the ConfigScreen would be affected of such a change.
 **/
class AlarmConfiguration
{
private:
    /**
     * @brief Path to the configuration file
     *
     * Established upon construction by CreateConfigFileLocation().
     **/
    const std::string _configfilelocation;
    /**
     * @brief KConfig backend
     *
     * Shared pointer to the KConfig backend used by this class to manage all the configuration options.
     **/
    KSharedConfigPtr _backend;
    /**
     * @brief KConfig skeleton
     *
     * The skeleton class keeps the information about all established configuration options and their memory locations.
     **/
    KConfigSkeleton _skeleton;
    /**
     * @brief ActiveMQ URI
     *
     * A URI containing protocol, hostname/address and port to connect to the Apache ActiveMQ message broker.
     *
     * The current setting will be saved here by the KConfig framework.
     */
    QString _activemquri;
    /**
     * @brief ActiveMQ username
     *
     * Used by CMSClient to connect to the Apache ActiveMQ message broker.
     *
     * The current setting will be saved here by the KConfig framework.
     */
    QString _activemqusername;
    /**
     * @brief ActiveMQ password
     *
     * Used by CMSClient to connect to the Apache ActiveMQ message broker.
     *
     * The current setting will be saved here by the KConfig framework.
     */
    QString _activemqpassword;
    /**
     * @brief JMS topic name used by CSS Alarm Server
     *
     * The name of the topic on the Apache ActiveMQ message broker that is used by the CSS Alarm Server to announce changes in the alarm status of PVs.
     */
    QString _activemqtopicname;
    /**
     * @brief Timeout for starting alarm notification in the laboratory
     *
     * If an alarm lasts for longer than this value in seconds, a notification in the laboratory itself will be activated.
     */
    unsigned int _laboratorynotificationtimeout;
    /**
     * @brief Timeout for sending a desktop notification
     *
     * If an alarm lasts for longer than this value in seconds, a dekstop notification will send to alert the user at his office PC.
     */
    unsigned int _desktopnotificationtimeout;
    /**
     * @brief Timeout for sending an e-mail notification
     *
     * If an alarm lasts for longer than this value in seconds, an e-mail will be send to a specific mailing list.
     */
    unsigned int _emailnotificationtimeout;
    /**
     * @brief Sender address for alarm e-mail notifications
     *
     * This e-mail address will be used as the sender's address when an alarm e-mail notification is sent.
     */
    QString _emailnotificationfrom;
    /**
     * @brief Recipient address for alarm e-mail notifications
     *
     * This e-mail address will be used as the recipient's address when an alarm e-mail notification is sent.
     */
    QString _emailnotificationto;
    /**
     * @brief Name of the SMTP server
     *
     * DNS name or IP address of the SMTP server used to send the e-mail notifications.
     */
    QString _emailnotificationservername;
    /**
     * @brief Port of SMTP server
     *
     * Numeric port number of the SMTP daemon on the server used to send the e-mail notifications.
     */
    unsigned int _emailnotificationserverport;
    /**
     * @brief Device node of the USB relais for the alarm flash light
     *
     * The relais controlled via this device node is used in the class FlashLight to operate a red flashing light that notifies staff in the lab about an alarm.
     */
    QString _flashlightrelaisdevicenode;
    /**
     * @brief KConfig item for _activemquri setting
     *
     * KConfig subclass to represent one setting in the configuration file. It reads the configuration from the file, stores it in the aforementioned variable and is also used to correctly change the setting within the KConfig framework.
     */
    KConfigSkeleton::ItemString* _activemquriitem;
    /**
     * @brief KConfig item for _activemqusername setting
     *
     * KConfig subclass to represent one setting in the configuration file. It reads the configuration from the file, stores it in the aforementioned variable and is also used to correctly change the setting within the KConfig framework.
     */
    KConfigSkeleton::ItemString* _activemqusernameitem;
    /**
     * @brief KConfig item for _activemqpassword setting
     *
     * KConfig subclass to represent one setting in the configuration file. It reads the configuration from the file, stores it in the aforementioned variable and is also used to correctly change the setting within the KConfig framework.
     */
    KConfigSkeleton::ItemString* _activemqpassworditem;
    /**
     * @brief KConfig item for _activemqtopicname setting
     *
     * KConfig subclass to represent one setting in the configuration file. It reads the configuration from the file, stores it in the aforementioned variable and is also used to correctly change the setting within the KConfig framework.
     */
    KConfigSkeleton::ItemString* _activemqtopicnameitem;
    /**
     * @brief KConfig item for _laboratorynotificationtimeout setting
     *
     * KConfig subclass to represent one setting in the configuration file. It reads the configuration from the file, stores it in the aforementioned variable and is also used to correctly change the setting within the KConfig framework.
     */
    KConfigSkeleton::ItemUInt* _laboratorynotificationtimeoutitem;
    /**
     * @brief KConfig item for _desktopnotificationtimeout setting
     *
     * KConfig subclass to represent one setting in the configuration file. It reads the configuration from the file, stores it in the aforementioned variable and is also used to correctly change the setting within the KConfig framework.
     */
    KConfigSkeleton::ItemUInt* _desktopnotificationtimeoutitem;
    /**
     * @brief KConfig item for _emailnotificationtimeout setting
     *
     * KConfig subclass to represent one setting in the configuration file. It reads the configuration from the file, stores it in the aforementioned variable and is also used to correctly change the setting within the KConfig framework.
     */
    KConfigSkeleton::ItemUInt* _emailnotificationtimeoutitem;
    /**
     * @brief KConfig item for _emailnotificationfrom setting
     *
     * KConfig subclass to represent one setting in the configuration file. It reads the configuration from the file, stores it in the aforementioned variable and is also used to correctly change the setting within the KConfig framework.
     */
    KConfigSkeleton::ItemString* _emailnotificationfromitem;
    /**
     * @brief KConfig item for _emailnotificationto setting
     *
     * KConfig subclass to represent one setting in the configuration file. It reads the configuration from the file, stores it in the aforementioned variable and is also used to correctly change the setting within the KConfig framework.
     */
    KConfigSkeleton::ItemString* _emailnotificationtoitem;
    /**
     * @brief KConfig item for _emailnotificationservername setting
     *
     * KConfig subclass to represent one setting in the configuration file. It reads the configuration from the file, stores it in the aforementioned variable and is also used to correctly change the setting within the KConfig framework.
     */
    KConfigSkeleton::ItemString* _emailnotificationservernameitem;
    /**
     * @brief KConfig item for _emailnotificationserverport setting
     *
     * KConfig subclass to represent one setting in the configuration file. It reads the configuration from the file, stores it in the aforementioned variable and is also used to correctly change the setting within the KConfig framework.
     */
    KConfigSkeleton::ItemUInt* _emailnotificationserverportitem;
    /**
     * @brief KConfig item for _flashlightrelaisdevicenode setting
     *
     * KConfig subclass to represent one setting in the configuration file. It reads the configuration from the file, stores it in the aforementioned variable and is also used to correctly change the setting within the KConfig framework.
     */
    KConfigSkeleton::ItemString* _flashlightrelaisdevicenodeitem;
    /**
     * @brief Establish location of the configuration file
     *
     * A default location for the configuration file can be given at compile time via the CONFIGFILELOCATION macro. This can be overriden at runtime by setting the environment variable ALARMNOTIFICATIONSCONFIG to the full path of the file.
     * 
     * If the path within CONFIGFILELOCATION contains the tilde ("~") which represents the home directory under Linux and the environment variable HOME is set, the tilde will be replaced with the path in HOME.
     *
     * @return Standard string containing the configuration file path
     **/
    static std::string CreateConfigFileLocation();
    /**
     * @brief Constructor
     *
     * Creates all required KConfig items, reads and parses the configuration file to make the data available for use by other parts of the application.
     *
     **/
    AlarmConfiguration();
    /**
     * @brief Create ActiveMQ connectivity settings
     *
     * Creates the KConfig item for the setting regarding the connection to the Apache ActiveMQ message broker used by the CSS Alarm Server.
     * @return Nothing
     */
    void CreateActiveMQConnectivitySettings();
public:
    /**
     * @brief Get singleton instance
     *
     * This static method returns a reference (not a pointer!) to the singleton instance of AlarmConfiguration. On the first call to instance(), this singleton instance will be created and the constructor will be called. If an exception occurs in the constructor, instance() will call ExceptionHandler() to print an error message and then terminate the application.
     * @return A reference to the singleton instance
     **/
    static AlarmConfiguration& instance() noexcept;
    /**
     * @brief Copy constructor (deleted)
     *
     * This class cannot be copied.
     * @param other Another instance of AlarmConfiguration
     **/
    AlarmConfiguration ( const AlarmConfiguration& other ) = delete;
    /**
     * @brief Move constructor (C++11, deleted)
     *
     * This class cannot be moved.
     * @param other Another instance of AlarmConfiguration
     **/
    AlarmConfiguration ( AlarmConfiguration&& other ) = delete;
    /**
     * @brief Destructor
     *
     * Flushed all remaining configuration changes to disk and frees the resources.
     **/
    ~AlarmConfiguration();
    /**
     * @brief Copy assignment (deleted)
     *
     * This class cannot be copied.
     * @param other Another instance of AlarmConfiguration
     * @return Nothing
     **/
    AlarmConfiguration& operator= ( const AlarmConfiguration& other ) = delete;
    /**
     * @brief Move assignment (C++11, deleted)
     *
     * This class cannot be moved.
     * @param other Another instance of AlarmConfiguration
     * @return Nothing
     **/
    AlarmConfiguration& operator= ( AlarmConfiguration&& other ) = delete;
    /**
     * @brief Query config file location
     *
     * Get the location of the configuration file used by this instance of AlarmConfiguration. The location cannot be changed during use of this class but it is established only once at instanciation.
     * @return Standard string containing the full path to the configuration file.
     **/
    const std::string& getConfigFileLocation() const noexcept;
    /**
     * @brief Read configuration again from disk
     *
     * Reads and parses the configuration file again and updates the variables inside AlarmConfiguration accordingly. Changes applied to the AlarmConfiguration that have not been written to disk are discarded,
     * @return Nothhing
     **/
    void ReReadConfiguration();
    /**
     * @brief Write configuration to disk
     *
     * Changes in the settings of this AlarmConfiguration instance are written to the configuration file on disk. If another user also changed the file since it was last loaded into this instance, but the local and remote changes concern different settings, the other user's changes are not overwritten. If both changes concern the same setting, the last saving process wins.
     * @return Nothing
     **/
    void WriteConfiguration();
    /**
     * @brief ActiveMQ connection URI
     *
     * Used by CMSClient to connect to the Apache ActiveMQ message broker.
     *
     * This method cannot throw exceptions.
     * @return The requested setting
     */
    std::string getActiveMQURI() const noexcept;
    /**
     * @brief Change ActiveMQ connection URI
     *
     * Used by CMSClient to connect to the Apache ActiveMQ message broker.
     * @param newSetting New configuration value
     * @return Nothing
     */
    void setActiveMQURI ( const std::string& newSetting );
    /**
     * @brief ActiveMQ connection username
     *
     * Used by CMSClient to connect to the Apache ActiveMQ message broker.
     *
     * This method cannot throw exceptions.
     * @return The requested setting
     */
    std::string getActiveMQUsername() const noexcept;
    /**
     * @brief Change ActiveMQ connection username
     *
     * Used by CMSClient to connect to the Apache ActiveMQ message broker.
     * @param newSetting New configuration value
     * @return Nothing
     */
    void setActiveMQUsername ( const std::string& newSetting );
    /**
     * @brief ActiveMQ connection password
     *
     * Used by CMSClient to connect to the Apache ActiveMQ message broker.
     *
     * This method cannot throw exceptions.
     * @return The requested setting
     */
    std::string getActiveMQPassword() const noexcept;
    /**
     * @brief Change ActiveMQ connection password
     *
     * Used by CMSClient to connect to the Apache ActiveMQ message broker.
     * @param newSetting New configuration value
     * @return Nothing
     */
    void setActiveMQPassword ( const std::string& newSetting );
    /**
     * @brief JMS topic used by the CSS Alarm Server
     *
     * Used by CMSClient to receive messages sent by the CSS Alarm Server
     *
     * This method cannot throw exceptions.
     * @return The requested setting
     */
    std::string getActiveMQTopicName() const noexcept;
    /**
     * @brief Change JMS topic used by the CSS Alarm Server
     *
     * Used by CMSClient to receive messages sent by the CSS Alarm Server
     * @param newSetting New configuration value
     * @return Nothing
     */
    void setActiveMQTopicName ( const std::string& newSetting );
    /**
     * @brief Timeout for starting alarm notification in the laboratory
     *
     * If an alarm lasts for longer than this value in seconds, a notification in the laboratory itself will be activated. A value of 0 disables this type of notification.
     *
     * This method cannot throw exceptions.
     * @return The requested setting
     */
    unsigned int getLaboratoryNotificationTimeout() const noexcept;
    /**
     * @brief Change the timeout for starting alarm notification in the laboratory
     *
     * If an alarm lasts for longer than this value in seconds, a notification in the laboratory itself will be activated. A value of 0 disables this type of notification.
     * @param newSetting New configuration value
     * @return Nothing
     */
    void setLaboratoryNotificationTimeout ( const unsigned int newSetting );
    /**
     * @brief Timeout for sending a desktop notification
     *
     * If an alarm lasts for longer than this value in seconds, a dekstop notification will send to alert the user at his office PC. A value of 0 disables this type of notification.
     *
     * This method cannot throw exceptions.
     * @return The requested setting
     */
    unsigned int getDesktopNotificationTimeout() const noexcept;
    /**
     * @brief Change the timeout for sending a desktop notification
     *
     * If an alarm lasts for longer than this value in seconds, a dekstop notification will send to alert the user at his office PC. A value of 0 disables this type of notification.
     * @param newSetting New configuration value
     * @return Nothing
     */
    void setDesktopNotificationTimeout ( const unsigned int newSetting );
    /**
     * @brief Timeout for sending an e-mail notification
     *
     * If an alarm lasts for longer than this value in seconds, an e-mail will be send to a specific mailing list. A value of 0 disables this type of notification.
     *
     * This method cannot throw exceptions.
     * @return The requested setting
     */
    unsigned int getEMailNotificationTimeout() const noexcept;
    /**
     * @brief Change the timeout for sending an e-mail notification
     *
     * If an alarm lasts for longer than this value in seconds, an e-mail will be send to a specific mailing list. A value of 0 disables this type of notification.
     * @param newSetting New configuration value
     * @return Nothing
     */
    void setEMailNotificationTimeout ( const unsigned int newSetting );
    /**
     * @brief Sender address for alarm e-mail notifications
     *
     * This e-mail address will be used as the sender's address when an alarm e-mail notification is sent.
     *
     * This method cannot throw exceptions.
     * @return The requested setting
     */
    std::string getEMailNotificationFrom() const noexcept;
    /**
     * @brief Change the Sender address for alarm e-mail notifications
     *
     * This e-mail address will be used as the sender's address when an alarm e-mail notification is sent.
     * @param newSetting New configuration value
     * @return Nothing
     */
    void setEMailNotificationFrom ( const std::string& newSetting );
    /**
     * @brief Recipient address for alarm e-mail notifications
     *
     * This e-mail address will be used as the recipient's address when an alarm e-mail notification is sent.
     *
     * This method cannot throw exceptions.
     * @return The requested setting
     */
    std::string getEMailNotificationTo() const noexcept;
    /**
     * @brief Change the recipient address for alarm e-mail notifications
     *
     * This e-mail address will be used as the recipient's address when an alarm e-mail notification is sent.
     * @param newSetting New configuration value
     * @return Nothing
     */
    void setEMailNotificationTo ( const std::string& newSetting );
    /**
     * @brief Name of the SMTP server
     *
     * The DNS name or IP address of the SMTP server used to send the e-mail notifications,
     *
     * This method cannot throw exceptions.
     * @return The requested setting
     */
    std::string getEMailNotificationServerName() const noexcept;
    /**
     * @brief Change the name of the STMP server
     *
     * The DNS name or IP address of the SMTP server used to send the e-mail notifications,
     * @param newSetting New configuration value
     * @return Nothing
     */
    void setEMailNotificationServerName ( const std::string& newSetting );
    /**
     * @brief Port of SMTP server
     *
     * Numeric port number of the SMTP daemon on the server used to send the e-mail notifications.
     *
     * This method cannot throw exceptions.
     * @return The requested setting
     */
    unsigned int getEMailNotificationServerPort() const noexcept;
    /**
     * @brief Change the port of SMTP server
     *
     * Numeric port number of the SMTP daemon on the server used to send the e-mail notifications.
     * @param newSetting New configuration value
     * @return Nothing
     */
    void setEMailNotificationServerPort ( const unsigned int newSetting );
    /**
     * @brief Device node of the USB relais for the alarm flash light
     *
     * The relais controlled via this device node is used in the class FlashLight to operate a red flashing light that notifies staff in the lab about an alarm.
     *
     * This method cannot throw exceptions.
     * @return The requested setting
     */
    std::string getFlashLightRelaisDeviceNode() const noexcept;
    /**
     * @brief Change the device node of the USB relais for the alarm flash light
     *
     * The relais controlled via this device node is used in the class FlashLight to operate a red flashing light that notifies staff in the lab about an alarm.
     * @param newSetting New configuration value
     * @return Nothing
     */
    void setFlashLightRelaisDevideNode ( const std::string& newSetting );
    /**
     * @brief INTERNAL METHOD: Shared pointer to KConfig instance
     *
     * THIS METHOD IS NOT PART OF THE PUBLIC API!!
     *
     * Provides shared access to the KConfig instance managing this configuration. Used to connect the KConfigDialog to this configuration.
     * @return KSharedConfigPtr object
     **/
    KSharedConfigPtr internal();
    /**
     * @brief INTERNAL METHOD: Skeleton of the configuration
     *
     * THIS METHOD IS NOT PART OF THE PUBLIC API!!
     *
     * Provides access to the skeleton object that outlines all possible configuration parameters and their default values.
     * @return Pointer to KConfigSkeleton
     **/
    KConfigSkeleton* internal_skel();
};

}

#endif // ALARMCONFIGURATION_H
