/**
 * @file beedo.h
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

#include "oldgcccompat.h" // Compatibilty macros for GCC < 4.7

#ifndef BEEDO_H
#define BEEDO_H

#include <phononnamespace.h>
#include <Path>
// Forward declaration of Phonon classes
namespace Phonon
{
class AudioOutput;
class MediaObject;
class VideoWidget;
}

#include <QObject>
#include <QWidget>
#ifdef BEEDOOLDQTFALLBACK
#include <QBuffer>
#endif

namespace AlarmNotifications
{

/**
 * @brief Opto-acoustic alarm engine
 *
 * In case of an alarm, this class will play a video (with sound) in the top left corner of the screen. The video window is displayed borderless and without the control buttons of the window manager, so it cannot be "clicked away". The video is compiled into the application executable via the Qt Resource beedo.qrc, which refers to the video file beedo.ogv. The video should use the Ogg Theora video and Ogg Vorbis audio codec to ensure that it is playable on as many systems as possible. To ensure that the video resource is not optimized out by the compiler, the main method must call "Q_INIT_RESOURCE(beedo);".
 *
 * As there is no need to play several video overlays, this class is laid out as a singleton. To start and stop the video/audio playback, other code just needs to invoke the static methods start() and stop() respectively. In addition, the GUI elements and the signal/slot mechanism must be initialized from the main event loop, otherwise a Qt error will occur. To do this, the constructor of DesktopAlarmWidget calls Beedo::instance(). When the applications exits, the Beedo class will be destructed last because it is a global object. This is a problem because all the other Qt objects will be gone then and so the destructor of QObject will cause a segmentation fault. To avoid this, the destructor of DesktopAlarmWidget invokes the destroy() method. Trying to use this object after destroy() has been called will result in undefined behaviour or even a crash.
 *
 * If the video playback is active, the file will be looped until stop() is called. Although this class is built for a smooth playback, a small interruption is unavoidable.
 *
 * The Qt libraries on Scientific Linux 6 are somehow buggy: The first play-through of the file works fine, but the second time the video hangs and the sound is very scratchy. As a workaround, the preprocessor definition BEEDOOLDQTFALLBACK is set when cmake detects an older version of Qt. In this case, all objects related to the media playback are destroyed and recreated upon every new start() and new loop. Unfortunately, this increases the gap between two loops significantly. The old Phonon libraries cannot read a media file directly from a Qt resource, so it is read from the executable and stored in a QBuffer that a Phonon::MediaSource can use.
 *
 * The actual video and audio handling is done by the Phonon library which was developed by KDE and is a part of Qt. The sound output of this class is grouped into the notification category of Phonon and PulseAudio.
 */
class Beedo : public QObject
{
    Q_OBJECT
private:
    /**
     * @brief Video/audio play flag
     *
     * Set to true by start_internal() when a video is started and set to false by stop_internal() when the video is stopped.
     */
    bool _go;
    /**
     * @brief Video window
     *
     * Borderless window where the Phonon::VideoWidget is placed on.
     */
    QWidget* _display;
    /**
     * @brief Phonon audio output encapsulation
     *
     * Phonon object that connects to the audio hardware.
     */
    Phonon::AudioOutput* _audio;
    /**
     * @brief Phonon media file encapsulation
     *
     * Does all the work regarding finding the codecs, reading the media file/stream etc.
     */
    Phonon::MediaObject* _media;
    /**
     * @brief Phonon video output encapsulation
     *
     * Phonon object that connects to the graphics server and video hardware.
     */
    Phonon::VideoWidget* _video;
    /**
     * @brief Connection between _media and _audio
     *
     * Passes the audio data from the media object as source to the audio object as sink.
     */
    Phonon::Path _mediaToAudio;
    /**
     * @brief Connection between _media and _video
     *
     * Passes the audio data from the media object as source to the video object as sink.
     */
    Phonon::Path _mediaToVideo;
#ifdef BEEDOOLDQTFALLBACK
    QBuffer _videobuffer;
#endif

    /**
     * @brief Constructor
     *
     * On new Qt libraries, the constructor initializes all the hardware connections by invoking createMediaObjects(). If BEEDOOLDQTFALLBACK is set, this is done every time a file is played by playAlarmVideo(). On these old libraries, the constructor puts the video file into a QBuffer as the old libs cannot read from a Qt resource.
     */
    Beedo();
    /**
     * @brief Internal routine to start video/audio playback
     *
     * Called by the static helper method start(). Sets the _go flag and emits signalPlay() to invoke playAlarmVideo() in the GUI thread. If _go is already set this method does nothing.
     * @return Nothing
     */
    void start_internal() noexcept;
    /**
     * @brief Internal routine to stop video/audio playback
     *
     * Called by the static helper method stop(). Clears the _go flag and emits signalStop() to invoke stopAlarmVideo() in the GUI thread. If _go is already cleared this method does nothing.
     *
     * This method cannot throw exceptions.
     * @return Nothing
     */
    void stop_internal() noexcept;
    /**
     * @brief Create the Qt objects to play a media file
     *
     * Encapsulates all the initialization for Phonon. On recent Qt versions this method is invoked once by the constructor. On old versions (BEEDOOLDQTFALLBACK is set), the objects have to be recreated every time by playAlarmVideo().
     *
     * This method cannot throw exceptions.
     * @return Nothing
     */
    void createMediaObjects();
    /**
     * @brief Delete the Qt and Phonon objects
     *
     * This method deleted the objects for audio and video playback and clears all corresponding resources. On old Qt versions, playAlarmVideo() does this before recreating the objects. On all versions, this method is called by destroy().
     * @return Nothing
     */
    void destroyMediaObjects();
private slots:
    /**
     * @brief Instruct phonon to start playing
     *
     * This slot method receives the signalPlay() emitted by start_internal() and instructs Phonon to start playing. On old versions of Qt (BEEDOOLDQTFALLBACK is set), this method recreates the needed widgets first to avoid problems.
     * @return Nothing
     */
    void playAlarmVideo();
    /**
     * @brief Instruct Phonon to stop playing
     *
     * This slot method receives the signalStop() emitted by stop_internal() and instructs Phonon to stop playing. The video window is not closed by this method as this may result in a video driver crash. This close is done in phononStateChange() after the status has changed to stopped.
     * @return Nothing
     */
    void stopAlarmVideo();
    /**
     * @brief React on a Phonon state change
     *
     * This slot is connected to the stateChanged signal on _media. It will close the video window after playback has been stopped and restart the video file after it ran out to procude a loop until stop() is called.
     * @param newState Current state of _media
     * @param oldState Old state of _media before the change
     * @return Nothing
     */
    void phononStateChange ( const Phonon::State newState, const Phonon::State oldState );
public:
    /**
     * @brief Get singleton instance
     *
     * This returns a reference (not a pointer) to the singleton instance. On the first invocation, the singleton instance is created. This should only be done from within the GUI thread (as in the constructor of DesktopAlarmWidget) - otherwise a Qt error will occur.
     * @return Reference to singleton instance
     */
    static Beedo& instance();
    /**
     * @brief Start video playback
     *
     * This will activate the Beedo engine and play the compiled-in video/audio in an endless loop until stop() is called. If the playback is already running, this method does nothing.
     *
     * This method cannot throw exceptions.
     * @return Nothing
     */
    inline static void start() noexcept
    {
        Beedo::instance().start_internal();
    }
    /**
     * @brief Stop video playback
     *
     * This will disable the Beedo engine, stop the video/audio playback and close the borderless window. It is already stopped, this method does nothing.
     *
     * This method cannot throw exceptions.
     * @return Nothing
     */
    inline static void stop() noexcept
    {
        Beedo::instance().stop_internal();
    }
    /**
     * @brief Destructor
     *
     * Calls destroy() just to be safe, but destroy() should have been called before by the user. This destructor will be one of the last destructors called because this singleton instance is a global object. As the Qt infrastructure will be gone by then, it will cause a segmentation fault.
     *
     * To avoid this, the user should call destroy() at an appropriate point.
     */
    virtual ~Beedo();
    /**
     * @brief Copy constructor (deleted)
     *
     * This class cannot be copied.
     * @param other Another instance of Beedo
     */
    Beedo ( const Beedo& other ) = delete;
    /**
     * @brief Move constructor (C++11, deleted)
     *
     * This class cannot be moved.
     * @param other Another instance of Beedo
     */
    Beedo ( Beedo&& other ) = delete;
    /**
     * @brief Copy assignment (deleted)
     *
     * This class cannot be copied.
     * @param other Another instance of Beedo
     * @return Nothing (deleted)
     */
    Beedo& operator= ( const Beedo& other ) = delete;
    /**
     * @brief Move assignment (C++11, deleted)
     *
     * This class cannot be moved.
     * @param other Another instance of Beedo
     * @return Nothing (deleted)
     */
    Beedo& operator= ( Beedo&& other ) = delete;
    /**
     * @brief Destroy all media objects
     *
     * Stops the video/audio playback and deleted all related objects. The user has to cal this method when the application exists (as e.g. in the destructor of DesktopAlarmWidget), otherwise a segmentation fault is likely to occur when the destructor tries to remove the objects but the rest of the Qt infrastructure is already gone.
     * @return Nothing
     */
    void destroy();
signals:
    /**
     * @brief Signal GUI to start playing
     *
     * Emitted by start_internal() to call playAlarmVideo() in the GUI thread.
     * @return Nothing
     */
    void signalPlay();
    /**
     * @brief Signal GUI to stop playing
     *
     * Emitted by stop_internal() to call stopAlarmVideo() in the GUI thread.
     * @return Nothing
     */
    void signalStop();
};

}

#endif // BEEDO_H
