/**
 * @file oldgcccompat.h
 *
 * @author Tobias Triffterer
 *
 * @brief Compatibility macros for older versions of the GCC
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

#ifndef OLDGCCCOMPAT_H
#define OLDGCCCOMPAT_H

#ifndef DOXYGEN // Small hack to make compatibility macros visible to doxygen although doxygen simulates a GCC 4.8 compiler
#define DOXYGEN 0
#endif

// Minimum requirement for noexcept and nullptr keyword is GCC 4.6
#if ( __GNUC__ < 4 || ( __GNUC__ == 4 && __GNUC_MINOR__ < 6 ) || DOXYGEN == 1 ) // Make macro visible to documentation
/**
 * @brief Enable compatibility code for GCC < 4.6
 * 
 * This macro will be set to 1 if the compatibility mode is activated (i.e. the gcc version used is lower than 4.6). If the compatibility mode is not needed, the macro will be set to 0.
 **/
#define OLDGCC46COMPAT 1
/**
 * @brief Allow using the noexcept keyword with GCC < 4.6
 **/
#define noexcept throw()
/**
 * @brief Allow using the nullptr keyword with GCC < 4.6
 **/
#define nullptr NULL
#else
#define OLDGCC46COMPAT 0 // Disable compatibility mode
#endif

// Minimum requirement for final classes and methods is GCC 4.7
#if ( __GNUC__ < 4 || ( __GNUC__ == 4 && __GNUC_MINOR__ < 7 ) || DOXYGEN == 1 ) // Make macro visible to documentation
/**
 * @brief Enable compatibility code for GCC < 4.7
 * 
 * This macro will be set to 1 if the compatibility mode is activated (i.e. the gcc version used is lower than 4.7). If the compatibility mode is not needed, the macro will be set to 0.
 **/
#define OLDGCC47COMPAT 1
/**
 * @brief Allow using the final keyword with GCC < 4.7
 **/
#define final
#else
#define OLDGCC47COMPAT 0 // Disable compatibility mode
#endif


#endif // OLDGCCCOMPAT_H