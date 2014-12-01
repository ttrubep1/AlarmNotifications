# @file CMakeLists.txt
#
# @author Tobias Triffterer
#
# @brief Build system configuration file
#
# @version 1.0.0
#
#
# AlarmNotifications - Laboratory and desktop notification framework to
# be used with EPICS and Control System Studio
#
# Copyright © 2014 by Tobias Triffterer <tobias@ep1.ruhr-uni-bochum.de>
# for Institut für Experimentalphysik I der Ruhr-Universität Bochum
# (http://ep1.ruhr-uni-bochum.de)
#
# The latest source code is here: https://github.com/ttrubep1/AlarmNotifications
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

cmake_minimum_required(VERSION 2.6)

project(AlarmNotifications)

set(_kde4_uninstall_rule_created TRUE)

# Search for all the dependencies required for AlarmNotifications
find_package(Qt4 REQUIRED)
find_package(KDE4 REQUIRED)
find_package(Boost REQUIRED COMPONENTS thread system)
include(${QT_USE_FILE})

find_package(PkgConfig REQUIRED)
pkg_check_modules(GLIB_PKG REQUIRED glib-2.0 gdk-pixbuf-2.0)
pkg_check_modules(LIBNOTIFY REQUIRED libnotify)

# Determine include directory for ActiveMQ C++ client library
if(NOT EXISTS /usr/bin/activemqcpp-config)
  message(ERROR "Cannot find configuration executable of activemq-cpp library!")
endif(NOT EXISTS /usr/bin/activemqcpp-config)
exec_program(/usr/bin/activemqcpp-config ARGS --includes OUTPUT_VARIABLE ACTIVEMQ_INCLUDE_FLAGS)


# Set GCC-C++ flags used for all source files in the project
set(COMPILE_FLAGS "-Wall -W -Wconversion -Wshadow -Wwrite-strings -pthread -rdynamic -std=gnu++0x -fexceptions -O2 -fPIC ${ACTIVEMQ_INCLUDE_FLAGS}")
set(CMAKE_CXX_FLAGS ${COMPILE_FLAGS})

# Check the version of some libraries and activate compatibility code for older ones
if (LIBNOTIFY_VERSION LESS 0.7)
  add_definitions("-DNOTUSELIBNOTIFY")
  message("\n\n   ATTENTION! Old version of libnotify detected, activating fail-safe mode in AlarmServerConnector!\n\n")
  # Old libnotify implementations (as on SL6) require a strange parameter in notify_notification_new() that is known to cause problems and was therefore removed from the API. On these old systems, AlarmServerConnector will fall back to call the system binary notify-send instead of using libnotify.
endif (LIBNOTIFY_VERSION LESS 0.7)

if ( ( ${QT_VERSION_MAJOR} EQUAL 4) AND ( ${QT_VERSION_MINOR} LESS 8 ) )
  add_definitions("-DBEEDOOLDQTFALLBACK")
  # For some reason, the outdated libraries on SL6 require destroying and recreating the media objects, else playback will fail when playing for the second time...
endif ( ( ${QT_VERSION_MAJOR} EQUAL 4) AND ( ${QT_VERSION_MINOR} LESS 8 ) )


# Additional includes for Scientific Linux 6
if ("$ENV{RHELCOMPAT}" EQUAL "1")
  set(RHELINCLUDES /usr/include/phonon /usr/include/phonon/Phonon)
endif ("$ENV{RHELCOMPAT}" EQUAL "1")

# Define include directories
include_directories(${QT_INCLUDES} ${KDE4_INCLUDES} ${CMAKE_CURRENT_BINARY_DIR}
${GLIB_PKG_INCLUDE_DIRS} ${LIBNOTIFY_INCLUDE_DIRS} ${ZLIB_INCLUDE_DIRS} ${POSTGRES_INCLUDE_DIRS}
${X11_X11_INCLUDE_PATH} ${RHELINCLUDES} "QtSmtpClient/src")

# Some parts of AlarmNotifications that are used in several flavours are grouped into static libraries
set(AlarmNotificationsErrorSRC exceptionhandler.cpp)
set(AlarmNotificationsConfigFileSRC alarmconfiguration.cpp)
set(AlarmNotificationsActiveMQSRC alarmstatusentry.cpp cmsclient.cpp alarmserverconnector.cpp beedo.cpp flashlight.cpp)
set(DesktopWidgetAbstractSRC desktopalarmwidget.cpp emailsender_dummy.cpp x11compat.cpp)

# Now create the source variables for the main executables
set(ANDaemonSRC emailsender.cpp daemon.cpp main_daemon.cpp)
set(ANDesktopSRC desktopalarmwidgetqt.cpp main_desktopwidget.cpp)
if ( NOT ( ${KDE_VERSION_MINOR} LESS 4 ) ) # KStatusNotifierItem is not available in KDE versions before 4.4.
  set(ANDesktopKde4SRC desktopalarmwidgetkde4.cpp main_desktopwidget-kde4.cpp)
endif ( NOT ( ${KDE_VERSION_MINOR} LESS 4 ) )
# The Beedo engine is activated automatically if its video file is present
if (EXISTS ${CMAKE_SOURCE_DIR}/beedo.ogv)
  set (ANBeedoSRC main_beedo.cpp)
endif (EXISTS ${CMAKE_SOURCE_DIR}/beedo.ogv)
set(ANConfigSRC configscreen.cpp main_config.cpp)

# Include the source code of the QtSmtpClient
set(QtSmtpClientSRC QtSmtpClient/src/emailaddress.cpp QtSmtpClient/src/mimefile.cpp QtSmtpClient/src/mimemessage.cpp QtSmtpClient/src/mimetext.cpp QtSmtpClient/src/mimeattachment.cpp QtSmtpClient/src/mimehtml.cpp QtSmtpClient/src/mimemultipart.cpp QtSmtpClient/src/quotedprintable.cpp QtSmtpClient/src/mimecontentformatter.cpp QtSmtpClient/src/mimeinlinefile.cpp  QtSmtpClient/src/mimepart.cpp QtSmtpClient/src/smtpclient.cpp)
set(QtSmtpClientH QtSmtpClient/src/emailaddress.h QtSmtpClient/src/mimefile.h QtSmtpClient/src/mimeattachment.h QtSmtpClient/src/mimehtml.h QtSmtpClient/src/mimemultipart.h QtSmtpClient/src/quotedprintable.h QtSmtpClient/src/mimecontentformatter.h QtSmtpClient/src/mimepart.h QtSmtpClient/src/smtpclient.h)
qt4_wrap_cpp(QtSmtpClient_MOC ${QtSmtpClientH})

# Run the Qt meta object compiler (moc)
qt4_automoc(${ANDaemonSRC})
qt4_automoc(${AlarmNotificationsActiveMQSRC})
qt4_automoc(${DesktopWidgetAbstractSRC})
qt4_automoc(${ANDesktopSRC})
if ( NOT ( ${KDE_VERSION_MINOR} LESS 4 ) ) # KStatusNotifierItem is not available in KDE versions before 4.4.
  qt4_automoc(${ANDesktopKde4SRC})
endif ( NOT ( ${KDE_VERSION_MINOR} LESS 4 ) )
qt4_add_resources(StatusIconsRES statusicons.qrc)
if (EXISTS ${CMAKE_SOURCE_DIR}/beedo.ogv) # The Beedo engine is activated automatically if its video file is present
  qt4_automoc(${ANBeedoSRC})
  qt4_add_resources(BeedoRES beedo.qrc)
endif (EXISTS ${CMAKE_SOURCE_DIR}/beedo.ogv)
qt4_automoc(${ANConfigSRC})

# Compile the static libraries for the common used parts
add_library(alarmwatchererror STATIC ${AlarmNotificationsErrorSRC})
add_library(alarmwatcherconfigfile STATIC ${AlarmNotificationsConfigFileSRC})
add_library(alarmwatcheractivemq STATIC ${AlarmNotificationsActiveMQSRC})
add_library(desktopwidgetabstract STATIC ${DesktopWidgetAbstractSRC})
add_library(qtsmtpclient STATIC  ${QtSmtpClientSRC} ${QtSmtpClient_MOC})
if (EXISTS ${CMAKE_SOURCE_DIR}/beedo.ogv) # The Beedo engine is activated automatically if its video file is present
  add_library(alarmwatcherbeedoresource ${BeedoRES})
endif (EXISTS ${CMAKE_SOURCE_DIR}/beedo.ogv)

# Compile the executables for the various flavours
add_executable(an-daemon ${ANDaemonSRC})
add_executable(an-desktop ${ANDesktopSRC} ${StatusIconsRES})
if ( NOT ( ${KDE_VERSION_MINOR} LESS 4 ) ) # KStatusNotifierItem is not available in KDE versions before 4.4.
  add_executable(an-desktop-kde4 ${ANDesktopKde4SRC} ${StatusIconsRES})
  if (EXISTS ${CMAKE_SOURCE_DIR}/beedo.ogv) # The Beedo engine is activated automatically if its video file is present
    add_executable(an-desktop-kde4-beamtime ${ANDesktopKde4SRC} ${StatusIconsRES})
  endif (EXISTS ${CMAKE_SOURCE_DIR}/beedo.ogv)
endif ( NOT ( ${KDE_VERSION_MINOR} LESS 4 ) )
if (EXISTS ${CMAKE_SOURCE_DIR}/beedo.ogv) # The Beedo engine is activated automatically if its video file is present
  add_executable(an-beedo ${ANBeedoSRC})
  add_executable(an-desktop-beamtime ${ANDesktopSRC} ${StatusIconsRES})
endif (EXISTS ${CMAKE_SOURCE_DIR}/beedo.ogv)
add_executable(an-config ${ANConfigSRC})

# Declare some variables to keep the list of required libraries clean
set(LibsCore ${QT_QTCORE_LIBRARY} ${KDE4_KDECORE_LIBS} ${KDE4_KDEUI_LIBS} ${Boost_LIBRARIES})
set(LibsGui ${LibsCore} ${QT_QTGUI_LIBRARY} ${QT_PHONON_LIBRARY} ${X11_X11_LIB})
set(LibsNetwork ${QT_QTNETWORK_LIBRARY})
set(LibsAlarm activemq-cpp ${GLIB_PKG_LIBRARIES} ${LIBNOTIFY_LIBRARIES})
set(LibsAll ${LibsGui} ${LibsNetwork} ${LibsAlarm})

# Define which executable needs which libraries, internal and external ones
target_link_libraries(an-daemon alarmwatcherconfigfile alarmwatcheractivemq alarmwatchererror qtsmtpclient ${LibsAll})
target_link_libraries(an-desktop desktopwidgetabstract alarmwatcheractivemq desktopwidgetabstract alarmwatcherconfigfile alarmwatchererror ${LibsGui} ${LibsAlarm})
if ( NOT ( ${KDE_VERSION_MINOR} LESS 4 ) ) # KStatusNotifierItem is not available in KDE versions before 4.4.
  target_link_libraries(an-desktop-kde4 desktopwidgetabstract alarmwatcheractivemq desktopwidgetabstract alarmwatcherconfigfile alarmwatchererror ${LibsGui} ${LibsAlarm})
  if (EXISTS ${CMAKE_SOURCE_DIR}/beedo.ogv) # The Beedo engine is activated automatically if its video file is present
    target_link_libraries(an-desktop-kde4-beamtime desktopwidgetabstract alarmwatcheractivemq desktopwidgetabstract alarmwatcherconfigfile alarmwatchererror alarmwatcherbeedoresource ${LibsGui} ${LibsAlarm})
    set_target_properties(an-desktop-kde4-beamtime PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS} -DBEEDO")
  endif (EXISTS ${CMAKE_SOURCE_DIR}/beedo.ogv)
endif ( NOT ( ${KDE_VERSION_MINOR} LESS 4 ) )
if (EXISTS ${CMAKE_SOURCE_DIR}/beedo.ogv) # The Beedo engine is activated automatically if its video file is present
  target_link_libraries(an-beedo desktopwidgetabstract alarmwatcheractivemq desktopwidgetabstract alarmwatcherconfigfile alarmwatchererror alarmwatcherbeedoresource ${LibsGui} ${LibsAlarm})
  target_link_libraries(an-desktop-beamtime desktopwidgetabstract alarmwatcheractivemq desktopwidgetabstract alarmwatcherconfigfile alarmwatchererror alarmwatcherbeedoresource ${LibsGui} ${LibsAlarm})
  set_target_properties(an-desktop-beamtime PROPERTIES COMPILE_FLAGS "${COMPILE_FLAGS} -DBEEDO")
endif (EXISTS ${CMAKE_SOURCE_DIR}/beedo.ogv)
target_link_libraries(an-config alarmwatcherconfigfile alarmwatchererror ${LibsCore} ${LibsGui})
