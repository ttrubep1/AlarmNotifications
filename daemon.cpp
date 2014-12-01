/**
 * @file daemon.cpp
 *
 * @author Tobias Triffterer
 *
 * @brief AlarmNotifications daemon implementation
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

#include "daemon.h"


#include <iostream>
#include <QtCore/QDateTime>

#include "exceptionhandler.h"

using namespace AlarmNotifications;

Daemon& Daemon::instance()
{
    try
    {
        static Daemon global_instance;
        return global_instance;
    }
    catch ( std::exception& e )
    {
        ExceptionHandler ( e, "while starting the AlarmNotifications daemon.", true );
        exit ( 1 ); // ExceptionHandler will also call exit(), but g++ does not know when compiling this
        // so it will complain about a missing return value
    }
    catch ( ... )
    {
        ExceptionHandler ( "while starting the AlarmNotifications daemon.", true );
        exit ( 1 ); // ExceptionHandler will also call exit(), but g++ does not know when compiling this
        // so it will complain about a missing return value
    }
}

Daemon::Daemon()
    : _run ( true )
{
    hsigint = signal ( SIGINT, &signalReceiver );
    hsighup = signal ( SIGHUP, &signalReceiver );
    hsigquit = signal ( SIGQUIT, &signalReceiver );
    hsigusr1 = signal ( SIGUSR1, &signalReceiver );
    hsigusr2 = signal ( SIGUSR2, &signalReceiver );
    hsigterm = signal ( SIGTERM, &signalReceiver );
    std::cout << QDateTime::currentDateTime().toString ( QString::fromUtf8 ( "dd. MMM yyyy hh:mm:ss" ) ).toStdString() << ": Starting AlarmNotifications daemon..." << std::endl;
}

Daemon::~Daemon()
{
    std::cout << QDateTime::currentDateTime().toString ( QString::fromUtf8 ( "dd. MMM yyyy hh:mm:ss" ) ).toStdString() << ": Stopping AlarmNotifications daemon..." << std::endl;
}

void Daemon::run()
{
    try
    {
        while ( _run )
        {
            sleep ( DaemonSleepTimeout );
            std::cout << QDateTime::currentDateTime().toString ( QString::fromUtf8 ( "dd. MMM yyyy hh:mm:ss" ) ).toStdString() << ": ";
            if ( _asc.getNumberOfAlarms() == 0 )
                std::cout << "No alarms active.";
            else
                std::cout << "Number of active alarms: " << _asc.getNumberOfAlarms();
            std::cout << std::endl;
        }
    }
    catch ( std::exception& e )
    {
        ExceptionHandler ( e, "while running the AlarmNotifications daemon.", true );
    }
    catch ( ... )
    {
        ExceptionHandler ( "while running the AlarmNotifications daemon.", true );
    }
}

void Daemon::signalReceiver ( int signum )
{
    if ( signum != SIGINT && signum != SIGHUP && signum != SIGQUIT && signum != SIGUSR1 && signum != SIGUSR2 && signum != SIGTERM )
        return;
    instance()._run = false;
    signal ( SIGINT, instance().hsigint );
    signal ( SIGHUP, instance().hsighup );
    signal ( SIGQUIT, instance().hsigquit );
    signal ( SIGUSR1, instance().hsigusr1 );
    signal ( SIGUSR2, instance().hsigusr2 );
    signal ( SIGTERM, instance().hsigterm );
}
