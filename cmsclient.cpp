/**
 * @file cmsclient.cpp
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

#include "cmsclient.h"

#include <iostream>
#include <stdexcept>

#include <activemq/library/ActiveMQCPP.h>
#include <cms/Connection.h>
#include <cms/ConnectionFactory.h>
#include <cms/Destination.h>
#include <cms/MapMessage.h>
#include <cms/MessageConsumer.h>
#include <cms/Session.h>

#include "alarmconfiguration.h"
#include "alarmserverconnector.h"

using namespace AlarmNotifications;

CMSClient::CMSClient ( AlarmServerConnector& asc )
    : _asc ( asc ),
      _connection ( nullptr ),
      _session ( nullptr ),
      _topicServer ( nullptr ),
      _consumerServer ( nullptr )
{
    try
    {
        activemq::library::ActiveMQCPP::initializeLibrary(); // If you don't do this, all calls to ActiveMQ will segfault...
    }
    catch ( std::runtime_error& ex )
    {
        std::cerr << "Runtime error while initializing ActiveMQCPP library!" << std::endl;
        std::cerr << ex.what() << std::endl;
        throw;
    }
    try
    {
        cms::ConnectionFactory* factory = cms::ConnectionFactory::createCMSConnectionFactory ( AlarmConfiguration::instance().getActiveMQURI() );
        _connection = factory->createConnection (
                          AlarmConfiguration::instance().getActiveMQUsername(),
                          AlarmConfiguration::instance().getActiveMQPassword()
                      );
    }
    catch ( cms::CMSException& ex )
    {
        std::cerr << "Cannot create CMS connection!" << std::endl;
        std::cerr << ex.getMessage() << std::endl;
        ex.printStackTrace();
        _connection = nullptr;
        throw;
    }
    _connection->start();
    _connection->setExceptionListener ( this );
    try
    {
        //Taken one-to-one from the documentation...
        _session = _connection->createSession ( cms::Session::AUTO_ACKNOWLEDGE );
        _topicServer = _session->createTopic ( AlarmConfiguration::instance().getActiveMQTopicName() );
        _consumerServer = _session->createConsumer ( _topicServer );
        _consumerServer->setMessageListener ( this );
    }
    catch ( cms::CMSException& ex )
    {
        std::cerr << "Cannot create CMS session/topic!" << std::endl;
        std::cerr << ex.getMessage() << std::endl;
        ex.printStackTrace();
        if ( _session != nullptr )
            _session->close();
        _connection->close(); // Clean things up...
        delete _consumerServer;
        delete _topicServer;
        delete _session;
        delete _connection;
        _consumerServer = nullptr;
        _topicServer = nullptr;
        _session = nullptr;
        _connection = nullptr;
        throw; //... and escalate the exception
    }
}

CMSClient::~CMSClient()
{
    // According to the documentation, this calls may result in an excpetion.
    // As destructors are noexcept in C++11 unless explicitly stated otherwise, this is
    // a very bad idea and will instantly crash the program.
    // To avoid this, all the clean-up commands are but in a try-catch block that throws away
    // any exception (theres no handling that could be done)
    // And to stop one error from stopping the rest of the cleanup, each command got its own
    // try-catch block.
    try
    {
        if ( _session != nullptr )
            _session->close();
    }
    catch ( ... ) {}
    try
    {
        if ( _connection != nullptr )
            _connection->close();
    }
    catch ( ... ) {}
    try
    {
        delete _consumerServer;
    }
    catch ( ... ) {}
    try
    {
        delete _topicServer;
    }
    catch ( ... ) {}
    try
    {
        delete _session;
    }
    catch ( ... ) {}
    try
    {
        delete _connection;
    }
    catch ( ... ) {}
    try
    {
        activemq::library::ActiveMQCPP::shutdownLibrary(); // Final close down
    }
    catch ( ... ) {}
}

void CMSClient::onMessage ( const cms::Message* message ) noexcept
{
    const cms::MapMessage*const mapmessage = dynamic_cast<const cms::MapMessage*> ( message );
    // There are four message types in CMS, but the CSS Alarm Server uses MapMessage only
    // If message is not a MapMessage, dynamic_cast will return a nullptr...
    if ( mapmessage == nullptr )
        return; // ... and we throw the message away
    if ( !mapmessage->itemExists ( "TEXT" ) )
        return;
    // The Alarm Server sends frequent "IDLE" messages to show that it's still there...
    if ( mapmessage->getString ( "TEXT" ) != "STATE" )
        return; // ...but we don't have to forward them.
    if ( !mapmessage->itemExists ( "NAME" ) || !mapmessage->itemExists ( "SEVERITY" ) || !mapmessage->itemExists ( "STATUS" ) )
        return; // Make sure all required keys are present
    std::string rawname = mapmessage->getString ( "NAME" ); // The alarm server uses the pseudo-procotol denomination "epics://" in
    const std::string name = rawname.replace ( rawname.find ( "epics://" ), 8, "" ); // front of the PV names, so we strip it
    const AlarmStatusEntry ase (
        name,
        mapmessage->getString ( "SEVERITY" ),
        mapmessage->getString ( "STATUS" )
    );
    _asc.notifyStatusChange ( ase ); // This message passed filtering, so it's relevant and forwarded to the AlarmServerConnector
}

void CMSClient::onException ( const cms::CMSException& ex ) noexcept
{
    ex.printStackTrace();
}
