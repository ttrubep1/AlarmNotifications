/**
 * @file main_beedo.cpp
 *
 * @author Tobias Triffterer
 *
 * @brief Entrance point for the Beedo engine test application
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

#include <QApplication>
#include <QIcon>

#include "beedo.h"
#include "x11compat.h"

using namespace AlarmNotifications;

int main ( int argc, char** argv )
{
    Q_INIT_RESOURCE(beedo); // Force loading the video resource from static library alarmwatcherbeedoresource
    InitializeXThreads();
    QApplication app ( argc, argv );
    app.setApplicationName ( QString::fromUtf8 ( "Beedo engine test application" ) );
    app.setApplicationVersion ( QString::fromUtf8 ( "1.0.0" ) );
    app.setQuitOnLastWindowClosed ( false );
    app.setWindowIcon ( QIcon::fromTheme ( QString::fromUtf8 ( "dialog-warning" ) ) );
    Beedo::instance().start();
    app.exec();
    Beedo::instance().destroy();
    return 0;
}
