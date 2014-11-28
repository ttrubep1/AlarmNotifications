/**
 * @file configfilelocation.inc
 *
 * @author Tobias Triffterer
 *
 * @brief Macro for the default path of the configuration file
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

#ifndef CONFIGFILELOCATION_INC
#define CONFIGFILELOCATION_INC

#ifndef CONFIGFILELOCATION // Do not define here if already defined via compiler parameter
/**
 * @def CONFIGFILELOCATION
 * @brief Default path for the AlarmWatcher configuration file
 *
 *  This is the default location of the AlarmWatcher configuration file that will be compiled into the binary.
 *
 * It will be interpreted by the AlarmConfiguration class.
 * It can be overwritten using the ALARMNOTIFICATIONSCONFIG environment variable.
 **/
#define CONFIGFILELOCATION "~/.config/alarmnotifications.ini"
#endif

#endif