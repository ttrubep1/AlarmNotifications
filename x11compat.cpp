/**
 * @file x11compat.cpp
 *
 * @author Tobias Triffterer
 *
 * @brief Compability wrapper for the X11 API
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

#include "x11compat.h"

#include <stdexcept>

// This include creates a lot of defines like "None" that will break
// enums in other header files, so this function call has
// been isolated into its own translation unit:
#include "X11/Xlib.h"

using namespace AlarmNotifications;

void AlarmNotifications::InitializeXThreads()
{
    const int result = XInitThreads();
    if ( result == 0 )
        throw std::logic_error ( "Could not initialize X11 threads!" );
}
