/**
 * @file flashlight.h
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

#ifndef FLASHLIGHT_H
#define FLASHLIGHT_H

#include "oldgcccompat.h" // Compatibilty macros for GCC < 4.7

#include <string>
#include <termios.h>
#include <vector>

#include <boost/thread.hpp>

namespace AlarmNotifications
{

/**
 * @brief Controller for USB relais of red alarm flash light
 *
 * This class controls a USB relais of type "KMTronic USB One Relay". For communication with a PC, the USB relais uses the "FT232 USB-Serial (UART) IC". A standard Linux driver exists for this IC and it should appear as "/dev/ttyUSBn" with a non-negative integer n.
 *
 * The communication protocol of the device is implemented in this class, although for now only writing to the bus is supported. As there is only one alarm flash light, this class is implemented as a singleton. The user just has to call the static methods switchOn() and switchOff(), everything else is done internally. The two switch functions will never throw exceptions, any occuring exception will be forwarded to the global ExceptionHandler().
 */
class FlashLight
{
    /**
     * @brief Type for storing commands to the device
     *
     * The relais is controlled by sending a sequence of 8-bit characters over USB. Using a vector, these sequences can be handled quite easily.
     */
    typedef std::vector<uint8_t> deviceCommand ;
private:
    /**
     * @brief Path to the device file
     *
     * Path to the file representing the access to the FT232 UART IC. It should appear under Linux as "/dev/ttyUSBn" with a non-negative integer n.
     */
    const std::string _deviceNode;
    /**
     * @brief Mutex to protect device access
     *
     * The serial interface can only be accessed by one thread at a time, so for safety this mutex is used to exclude concurrent access.
     */
    boost::mutex _serialLineMutex;
    /**
     * @brief File descriptor
     *
     * The file descriptor index as returned by the open() system call.
     */
    int _fd;
    /**
     * @brief File descriptor use flag
     *
     * Set by openSerialInterface() when a handle on the device could be achieved and reset by closeSerialInterface() when the file handle is released.
     */
    bool _fdOpen;
    /**
     * @brief Constant for baud rate
     *
     * The baud rate of the virtual serial interface emulated by USB, to be used in configureSerialInterface().
     */
    static const unsigned int deviceBaudRate = B9600; // Taken from the manual of the USB relais

    /**
     * @brief Constructor
     *
     * Queries the location of the device file from AlarmConfiguration, but does not create a handle. This is only done when needed in openSerialInterface().
     */
    FlashLight();
    /**
     * @brief Internal routine to switch relais on or off
     *
     * This instance method is called by switchOn() and switchOff() to do the actual work.
     * @param lightSwitch True if the relais should be switched on, false if it should be switched off
     * @return Nothing
     */
    void switchInternal ( const bool lightSwitch );
    /**
     * @brief Assemble command sequence
     *
     * This method implements the communication protocol of the USB relais and returns a command sequence according to the requested switch action.
     * @param lightSwitch True if the relais should be switched on, false if it should be switched off
     * @return Command sequence to be forwarded to writeSerialInteface()
     */
    static deviceCommand createCommand ( const bool lightSwitch );
    /**
     * @brief Open serial interface
     *
     * Tries to create a handle for the device file _deviceNode and stores the file descriptor in _fd.
     * @return Nothing
     * @exception std::runtime_error The device node could not be opened.
     */
    void openSerialInterface();
    /**
     * @brief Set options of serial interface for USB relais
     *
     * This method sets the required flags in the termios structure for the serial device and applies these settings.
     * @return Nothing
     * @exception std::logic_error The serial interface has not been opened.
     * @exception std::runtime_error The device settings couldn't be applied.
     */
    void configureSerialInterface();
    /**
     * @brief Write command sequence to the USB relais
     *
     * The command sequence is transfered into a local buffer that can be fed into the write() system call.
     * @param command Command sequence to be written to the bus
     * @return Nothing
     * @exception std::logic_error The serial interface has not been opened.
     * @exception std::runtime_error An error occured while writing to the device.
     * @exception std::bad_alloc There is not enough memory available to transfer the command to a local buffer.
     */
    void writeSerialInteface ( const deviceCommand command );
    /**
     * @brief Close the device handle
     *
     * The device handle is closed and destroyed, _fdOpen is reset to false. Errors occuring during close, e.g. because the USB device has been removed while using it, are silently ignored.
     * @return Nothing
     */
    void closeSerialInterface();
public:
    /**
     * @brief Get singleton instance
     *
     * This returns a reference (not a pointer) to the singleton instance. On the first invocation, the singleton instance is created. Please note that this will not create a handle on the device node - it is only allocated while it is really in use.
     * @return Reference to singleton instance
     */
    static FlashLight& instance() noexcept;
    /**
     * @brief Destructor
     *
     * If the _fdOpen flag is set, closeSerialInterface() is invoked.
     */
    ~FlashLight();
    /**
     * @brief Switch on red alarm flash light
     *
     * This will order the USB relais to switch on the alarm light.
     *
     * This method cannot throw exceptions.
     * @return Nothing
     */
    static void switchOn() noexcept;
    /**
     * @brief Switch off red alarm flash light
     *
     * This will order the USB relais to switch off the alarm light.
     *
     * This method cannot throw exceptions.
     * @return Nothing
     */
    static void switchOff() noexcept;
    /**
     * @brief Copy constructor (deleted)
     *
     * This class cannot be copied.
     * @param other Another instance of FlashLight
     */
    FlashLight ( const FlashLight& other ) = delete;
    /**
     * @brief Move constructor (C++11, deleted)
     *
     * This class cannot be moved.
     * @param other Another instance of FlashLight
     */
    FlashLight ( FlashLight&& other ) = delete;
    /**
     * @brief Copy assignment (deleted)
     *
     * This class cannot be copied.
     * @param other Another instance of FlashLight
     * @return Nothing (deleted)
     */
    FlashLight& operator= ( const FlashLight& other ) = delete;
    /**
     * @brief Move assignment (C++11, deleted)
     *
     * This class cannot be moved.
     * @param other Another instance of FlashLight
     * @return Nothing (deleted)
     */
    FlashLight& operator= ( FlashLight&& other ) = delete;
};

}

#endif // FLASHLIGHT_H
