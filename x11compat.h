/**
 * @file x11compat.h
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

#ifndef X11COMPAT_H
#define X11COMPAT_H

#include "oldgcccompat.h" // Compatibilty macros for GCC < 4.7

namespace AlarmNotifications
{

/**
 * @brief Call XInitThreads() from Xlib
 *
 * This basically invokes the plain X11 API call XInitThreads() and throws an exception if the initialization failed.
 * @return Nothing
 * @exception std::logic_error The X11 API returned 0.
 */
void InitializeXThreads();

}

#endif // X11COMPAT_H
