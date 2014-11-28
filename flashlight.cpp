/**
 * @file flashlight.cpp
 *
 * @author Tobias Triffterer
 *
 * @brief Control USB relais for red alarm flash light
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

#include "flashlight.h"

#include <fcntl.h>
#include <stdexcept>

#include "alarmconfiguration.h"
#include "exceptionhandler.h"

using namespace AlarmNotifications;

FlashLight& FlashLight::instance() noexcept
{
    static FlashLight global_instance;
    return global_instance;
}


FlashLight::FlashLight()
    : _deviceNode ( AlarmConfiguration::instance().getFlashLightRelaisDeviceNode() ),
      _fd ( -1 ),
      _fdOpen ( false )
{
}

FlashLight::~FlashLight()
{
    if ( _fdOpen )
        closeSerialInterface();
}

void FlashLight::switchOn() noexcept
{
    try{
        FlashLight::instance().switchInternal ( true );
    }
    catch ( std::exception& e )
    {
        ExceptionHandler ( e, "switching on the flash light." );
    }
    catch ( ... )
    {
        ExceptionHandler ( "switching on the flash light." );
    }
}

void FlashLight::switchOff() noexcept
{
    try {
        FlashLight::instance().switchInternal ( false );
    }
    catch ( std::exception& e )
    {
        ExceptionHandler ( e, "switching off the flash light." );
    }
    catch ( ... )
    {
        ExceptionHandler ( "switching off the flash light." );
    }
}

void FlashLight::switchInternal ( const bool lightSwitch )
{
    const deviceCommand command = createCommand ( lightSwitch );
    boost::lock_guard<boost::mutex> concurrencylock ( _serialLineMutex );

    openSerialInterface();
    configureSerialInterface();
    writeSerialInteface ( command );
    closeSerialInterface();
}

FlashLight::deviceCommand FlashLight::createCommand ( const bool lightSwitch )
{
    deviceCommand command;
    // Command: Set relais
    command.push_back ( '\xff' );
    // Selected relais: number one (device has only one)
    command.push_back ( '\x01' );
    // Select action:
    if ( lightSwitch )
        command.push_back ( '\x01' );
    else
        command.push_back ( '\x00' );
    return command;
}

void FlashLight::openSerialInterface()
{
    _fd = open ( _deviceNode.c_str(), O_RDWR | O_NOCTTY | O_NONBLOCK );
    if ( _fd < 0 )
        throw std::runtime_error ( "Cannot open serial interface for flashlight." );
    _fdOpen = true;
}

void FlashLight::configureSerialInterface()
{
    if ( !_fdOpen )
        throw std::logic_error ( "configureSerialInterface() called on closed interface." );
    termios serialConfig;
    const int getattr_result = tcgetattr ( _fd, &serialConfig );
    if ( getattr_result < 0 )
        throw std::runtime_error ( "Error while reading configuration of serial interface." );

    // Return immediately on read if no bytes are available
    serialConfig.c_cc[VMIN] = 0;
    serialConfig.c_cc[VTIME] = 0;

    // Ignore modem control lines
    serialConfig.c_cflag |= CREAD | CLOCAL;

    // Switch off software handshake
    serialConfig.c_iflag &= ~IXON & ~IXOFF;

    // Set 8bit character size
    serialConfig.c_cflag &= ~CSIZE;
    serialConfig.c_cflag |= CS8;

    // Set parity to none
    serialConfig.c_cflag &= ~PARENB & ~PARODD;
    serialConfig.c_iflag |= IGNPAR;

    // Set number of stop bits to 1
    serialConfig.c_cflag &= ~CSTOPB;

    // Disable hardware handshake
    serialConfig.c_cflag &= ~CRTSCTS;

    // Set baud rate
    const int ispeed_result = cfsetispeed ( &serialConfig, deviceBaudRate );
    if ( ispeed_result < 0 )
        throw std::runtime_error ( "Cannot set input baud rate." );
    const int ospeed_result = cfsetospeed ( &serialConfig, deviceBaudRate );
    if ( ospeed_result < 0 )
        throw std::runtime_error ( "Cannot set input baud rate." );

    //Write new configuration to interface
    const int setattr_result = tcsetattr ( _fd, TCSANOW, &serialConfig );
    if ( setattr_result < 0 )
        throw std::runtime_error ( "Error while writing configuration of serial interface." );
}

void FlashLight::writeSerialInteface ( const FlashLight::deviceCommand command )
{
    if ( !_fdOpen )
        throw std::logic_error ( "writeSerialInterface() called on closed interface." );
    const long unsigned int numBytes = command.size();
    uint8_t* buffer = nullptr;
    try
    {
        // Create a local character buffer to be passed to write()
        buffer = new uint8_t[numBytes+1]; // One byte safety, just to be sure
        if ( buffer == nullptr )
            throw std::bad_alloc(); // If new returns a null pointer, there is not enough memory available
        // Set all bytes in the buffer to zero.
        memset ( buffer, 0, numBytes+1 );
        // Copy string content into buffer
        std::copy ( command.begin(), command.end(), buffer );
        // Write buffer to serial interface
        long int bytesWritten = -1;
        do
        {
            bytesWritten = write ( _fd, buffer, numBytes );
        }
        while ( ( bytesWritten < 0 && EAGAIN == errno ) ); // Repeat if error code EAGAIN occurs

        if ( bytesWritten < 0 )
            throw std::runtime_error ( "An error occured while writing to the serial interface" );
        if ( ( long unsigned int ) ( bytesWritten ) < numBytes )
            throw std::runtime_error ( "Could not write all bytes to serial interface" );

        delete[] buffer;
    }
    catch ( ... )
    {
        delete[] buffer;
    }
}

void FlashLight::closeSerialInterface()
{
    _fdOpen = false;
    close ( _fd );
    _fd = -1;
}
