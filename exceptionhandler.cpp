/**
 * @file exceptionhandler.cpp
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

#include "exceptionhandler.h"

#include <cxxabi.h>
#include <iostream>

using namespace AlarmNotifications;

void AlarmNotifications::ExceptionHandler ( std::exception& e, std::string location, const bool quit ) noexcept
{
    int status = 1; // 1 doesn't exist as return value of __cxa_demangle(), so if status is not 0 afterwards something went wrong
    char*const extype = abi::__cxa_demangle ( typeid ( e ).name(), nullptr, nullptr, &status );
    if ( status == 0 )
    {
        std::cerr << "An exception of type " << extype << " occured while " << location << std::endl;
    }
    else
    {
        std::cerr << "An exception of (mangled) type " << typeid ( e ).name() << " occured while instanciating EpicsAccessManager." << std::endl;
    }
    std::cerr << "Error Message: " << e.what() << std::endl;
    free ( extype );
    if ( quit )
    {
        std::cerr << "Exiting gracefully..." << std::endl;
        exit ( 1 );
    }
    else
    {
        std::cerr << "Dazzled and confused, but trying to continue..." << std::endl;
    }
}

void AlarmNotifications::ExceptionHandler ( std::string location, const bool quit ) noexcept
{
    std::cerr << "An unknown exception occured while " << location << std::endl;
    if ( quit )
    {
        std::cerr << "Exiting gracefully..." << std::endl;
        exit ( 1 );
    }
    else
    {
        std::cerr << "Dazzled and confused, but trying to continue..." << std::endl;
    }
}