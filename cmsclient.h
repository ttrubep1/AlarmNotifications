/**
 * @file cmsclient.h
 *
 * @author Tobias Triffterer
 *
 * @brief Interface to the CMS (C++ Messaging Service) library of Apache ActiveMQ
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

#ifndef CMSCLIENT_H
#define CMSCLIENT_H

#include "oldgcccompat.h" // Compatibilty macros for GCC < 4.7

#include <cms/CMSException.h>
#include <cms/ExceptionListener.h>
#include <cms/MessageListener.h>

/**
 * 
 * @brief C++ Messaging Service
 * 
 * This namespace is owned by the Apache ActiveMQ C++ library, it is included here just to do some forward declarations.
 */
namespace cms
{
// Forward declarations
class Connection;
class Destination;
class MessageConsumer;
class Session;
}

namespace AlarmNotifications
{

class AlarmServerConnector; // Forward declaration

/**
 * @brief C++ Messaging Service client
 *
 * This class encapsulates the C++ API of the Apache ActiveMQ library. Apache ActiceMQ is an implementation of the Java Messaging Service standard used by the CSS (Control System Studio) Alarm Server. The activemq-cpp library's API is therefore called C++ Messaging Service (CMS).
 *
 * This class connects to the Apache ActiveMQ message broker, subscribes to the topic of the CSS alarm server and receives all the messages there. The messages are then parsed, filtered and the relevant ones forwarded to AlarmServerConnector.
 */
class CMSClient : public cms::MessageListener, public cms::ExceptionListener
{
private:
    /**
     * @brief AlarmServerConnector instance
     *
     * Set by the constructor.
     */
    AlarmServerConnector& _asc;
    /**
     * @brief CMS connection
     *
     * Connection to the C++ Messaging Service
     */
    cms::Connection* _connection;
    /**
     * @brief CMS session
     *
     * Session on the C++ Messaging Service
     */
    cms::Session* _session;
    /**
     * @brief CMS topic
     *
     * Topic on the message broker, i.e. "name" of the "chatroom" where the CSS Alarm Server publishes its messages.
     */
    cms::Destination* _topicServer;
    /**
     * @brief CMS receiver
     *
     * Class to receive and parse the messages received from the ActiveMQ message broker
     */
    cms::MessageConsumer* _consumerServer;

    /**
     * @brief Message listener
     *
     * This method will be called by the ActiveMQ library if a message is received from the message broker.
     * @param message CMS message object
     * @return Nothing
     */
    virtual void onMessage ( const cms::Message* message ) noexcept;
    /**
     * @brief Exception listener
     *
     * This method will be called by the ActiveMQ library if an exception occurs in the messaging system
     * @param ex The CMS exception
     * @return Nothing
     */
    virtual void onException ( const cms::CMSException& ex ) noexcept;
public:
    /**
     * @brief Constructor
     *
     * Creates the necessary objects and connects to the Apache ActiveMQ message broker.
     * @param asc Reference to the AlarmServerConnector instance that should be notified when a relevant message arrives.
     * @exception cms::CMSException Something went wrong within Apache ActiveMQ
     * @exception std::runtime_error Initialization error
     */
    CMSClient ( AlarmServerConnector& asc );
    /**
     * @brief Destructor
     * 
     * Closes the connection to the message broker and cleans everything up.
     */
    ~CMSClient();
    /**
     * @brief Copy constructor (deleted)
     * 
     * This class cannot be copied.
     * @param other Another instance of CMSClient
     */
    CMSClient ( const CMSClient& other ) = delete;
    /**
     * @brief Move constructor (deleted)
     * 
     * This class cannot be moved.
     * @param other Another instance of CMSClient
     */
    CMSClient ( CMSClient&& other ) = delete;
    /**
     * @brief Copy assignment (deleted)
     * 
     * This class cannot be copied.
     * @param other Another instance of CMSClient
     * @return Nothing (deleted)
     */
    CMSClient& operator= ( const CMSClient& other ) = delete;
    /**
     * @brief Move assignment (deleted)
     * 
     * This class cannot be moved.
     * @param other Another instance of CMSClient
     * @return Nothing (deleted)
     */
    CMSClient& operator= ( CMSClient&& other ) = delete;
};

}

#endif // CMSCLIENT_H
