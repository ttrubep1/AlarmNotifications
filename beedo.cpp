/**
 * @file beedo.cpp
 *
 * @author Tobias Triffterer
 *
 * @brief Provide an opto-acoustic alarm notification on a control room PC
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

#include "beedo.h"

#include <iostream>
#include <stdexcept>
#include <unistd.h>
#include <QFile>
#include <QIcon>
#include <QLayout>
#include <QTimer>
#include <QUrl>
#include <AudioOutput>
#include <MediaObject>
#include <MediaSource>
#include <VideoWidget>

#include "exceptionhandler.h"

using namespace AlarmNotifications;

Beedo& Beedo::instance()
{
    //std::cout << "Beedo instance requested!" << std::endl;
    static Beedo global_instance;
    return global_instance;
}

Beedo::Beedo()
    : QObject ( nullptr ),
      _go ( false ),
      _display ( nullptr ),
      _audio ( nullptr ),
      _media ( nullptr ),
      _video ( nullptr )
{
#ifndef BEEDOOLDQTFALLBACK
    createMediaObjects();
#else
    QFile videoresource ( QString::fromUtf8 ( ":/beedo.ogv" ) );
    videoresource.open ( QIODevice::ReadOnly );
    _videobuffer.open ( QIODevice::ReadWrite );
    _videobuffer.write ( videoresource.readAll() );
    _videobuffer.seek ( 0 );
#endif

    connect ( this, SIGNAL ( signalPlay() ), this, SLOT ( playAlarmVideo() ) );
    connect ( this, SIGNAL ( signalStop() ), this, SLOT ( stopAlarmVideo() ) );
}

Beedo::~Beedo()
{
    destroy();
    // Just to be sure. If destroy() was called before, nothing will happen.
}

void Beedo::createMediaObjects()
{
    _display = new QWidget (
        nullptr,
        Qt::Widget |
        Qt::X11BypassWindowManagerHint |
        Qt::CustomizeWindowHint |
        Qt::WindowStaysOnTopHint
    );
    _media = new Phonon::MediaObject ( nullptr );
    _audio = new Phonon::AudioOutput ( Phonon::NotificationCategory, nullptr );
    _video = new Phonon::VideoWidget ( _display );
    _mediaToAudio = Phonon::createPath ( _media, _audio );
    _mediaToVideo = Phonon::createPath ( _media, _video );
    const QSize widgetsize ( 256, 256 );
    const QRect widgetrect ( QPoint ( 0,0 ),widgetsize );
    _display->setGeometry ( widgetrect );
    _display->setMinimumSize ( widgetsize );
    _display->setMinimumSize ( widgetsize );
    _video->setGeometry ( widgetrect );
    _video->setMinimumSize ( widgetsize );
    _video->setMaximumSize ( widgetsize );
    _display->setWindowIcon ( QIcon::fromTheme ( QString::fromUtf8 ( "dialog-warning" ) ) );
    connect ( _media, SIGNAL ( stateChanged ( Phonon::State,Phonon::State ) ), this, SLOT ( phononStateChange ( Phonon::State,Phonon::State ) ) );
}

void Beedo::destroyMediaObjects()
{
    if ( _media == nullptr || _audio == nullptr || _video == nullptr || _display == nullptr )
        return;
    _media->stop();
    _media->clearQueue();
    _media->clear();
    _mediaToAudio.disconnect();
    _mediaToVideo.disconnect();
    _video->hide();
    delete _display;
    _display = nullptr;
    delete _audio;
    _audio = nullptr;
    delete _media;
    _media = nullptr;
}

void Beedo::destroy()
{
    _go = false;
    destroyMediaObjects();
}

void Beedo::start_internal() noexcept
{
    try
    {
        if ( _go )
            return; // Beedo already running
        _go = true;
        emit signalPlay();
    }
    catch ( std::exception& e )
    {
        ExceptionHandler ( e, "Beedo::start_internal()" );
    }
    catch ( ... )
    {
        ExceptionHandler ( "Beedo::start_internal()" );
    }
}

void Beedo::stop_internal() noexcept
{
    try
    {
        if ( !_go )
            return; // Beedo already stopped
        _go = false;
        emit signalStop();
    }
    catch ( std::exception& e )
    {
        ExceptionHandler ( e, "Beedo::stop_internal()" );
    }
    catch ( ... )
    {
        ExceptionHandler ( "Beedo::stop_internal()" );
    }
}

void Beedo::playAlarmVideo()
{
#ifdef BEEDOOLDQTFALLBACK
    destroyMediaObjects();
    createMediaObjects();
#endif
    //_media->setCurrentSource ( QUrl::fromLocalFile ( QString::fromUtf8 ( "/home/tobias/Dokumente/Studium/EP1/slowcontrol/AlarmWatcher/beedo.ogv" ) ) );
#ifndef BEEDOOLDQTFALLBACK
    _media->setCurrentSource ( QUrl ( QString::fromUtf8 ( "qrc:/beedo.ogv" ) ) );
#else
    _videobuffer.seek ( 0 );
    _media->setCurrentSource ( &_videobuffer );
#endif
#ifndef BEEDOOLDQTFALLBACK
    while ( _media->state() == Phonon::LoadingState )
        usleep ( 10*1000 ); // Sleep for 10 ms
    usleep ( 500*1000 ); // Phonon apparently needs some more time, else the call to play() will be lost
#else
    //Strange bug in old versions of the library - or in the SL6 binaries
    usleep ( 750*1000 );
#endif
    if ( !_media->isValid() )
        std::cout <<  "Cannot play \"Beedo\" video resource!" << std::endl;
    _display->show();
    _media->play();
}

void Beedo::stopAlarmVideo()
{
    _media->stop();
}

void Beedo::phononStateChange ( const Phonon::State newState, const Phonon::State oldState )
{
    if ( newState == Phonon::PlayingState && oldState != Phonon::PlayingState )
        _display->show();
    if ( newState != Phonon::PlayingState && oldState == Phonon::PlayingState && !_go )
        _display->hide();
    if ( newState != Phonon::PlayingState && oldState == Phonon::PlayingState && _go )
        QTimer::singleShot ( 0, this, SLOT ( playAlarmVideo() ) ); // Do an infinite loop until someone invokes stop()
}

#include "beedo.moc"
