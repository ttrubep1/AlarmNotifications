/**
 * @file exceptionhandler.h
 *
 * @author Tobias Triffterer
 *
 * @brief Generic functions for exception handling
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

#ifndef EXCEPTIONHANDLER_H
#define EXCEPTIONHANDLER_H

#include <exception>
#include <string>

#include "oldgcccompat.h" // Compatibilty macros for GCC < 4.7

namespace AlarmNotifications {

/**
 * @brief Generic exception handler for known exceptions
 *
 * This function will display an error message containing the type name and the message of the exception and optionally quit the application gracefully.
 *
 * @param e The standard exception object
 * @param location A string explaining the point where the exception occured
 * @param quit A flag whether to exit the application or not, default is false
 * @return Nothing. If quit is set to true, this function does not return.
 **/
void ExceptionHandler ( std::exception& e, std::string location, const bool quit = false ) noexcept;
/**
 * @brief Generic exception handler for unknown exceptions
 *
 * This function will display an error message explaining that no exception object is available and optionally quit the application gracefully.
 *
 * @param location A string explaining the point where the exception occured
 * @param quit A flag whether to exit the application or not, default is false
 * @return Nothing. If quit is set to true, this function does not return.
 **/
void ExceptionHandler ( std::string location, const bool quit = false ) noexcept;

}
#endif
