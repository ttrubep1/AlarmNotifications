
This software package was created at the [Institut für Experimentalphysik (EP1)] (http://ep1.ruhr-uni-bochum.de) I of [Ruhr-Universität Bochum (RUB)] (https://www.ruhr-uni-bochum.de) located in [Bochum] (https://en.wikipedia.org/wiki/Bochum), [NRW] (https://en.wikipedia.org/wiki/North_Rhine-Westphalia), [Germany] (https://en.wikipedia.org/wiki/Germany) during development for the Detector Control System (DCS) of the [PANDA] (http://www-panda.gsi.de) [Electromagnetic Calorimeter] (http://www-panda.gsi.de/framework/det_iframe.php?section=EMC) (EMC).

It is provided without any warranty on a pure "as-is" basis in the hope that it might be useful for other groups of the PANDA collaboration or other members of the scientific community. This software is licensed under the [GNU General Public License, version 3] (https://www.gnu.org/licenses/gpl-3.0) or later.

# Introduction

This software is written to be used with [EPICS] (http://aps.anl.gov/epics/), the Experimental Physics and Industrial Control System, although it does not directly depend on EPICS libraries like `libca.so`  and `libCom.so`.

To quote its developers,
>  EPICS is a set of software tools and applications which provide a software infrastructure for use in building distributed control systems to operate devices such as Particle Accelerators, Large Experiments and major Telescopes. Such distributed control systems typically comprise tens or even hundreds of computers, networked together to allow communication between them and to provide control and feedback of the various parts of the device from a central control room, or even remotely over the internet.

Every (numeric) process variable (PV) in EPICS can have alarm borders assigned, so that EPICS will change the status of the PV from `OK` to `MINOR ALARM` or `MAJOR ALARM`, depending on the severity of the problem.

EPICS itself provides no further feature to alert staff of the problem: Usually, only the display in the operator interface (OPI) will turn orange or red, but if nobody is watching nobody will notice.

This application provides these notifications in connection with the "​**B**​est **E**​ver **A**​larm **S**​ystem **T**​oolkit" BEAST created as part of the [Control System Studio (CSS)] (https://github.com/ControlSystemStudio/cs-studio). There are three stages of notifications, usually used one after the other:

1. Laboratory notification: Activating a flashlight in the lab
2.  Desktop notification: Showing an overlay notification on the office desktop
3.  E-mail notification: Writing an e-mail to an internal mailing list

# Prerequisites

First of all, you need a working control system based on EPICS, but the creation of this is outside the scope of this document.

Second, you need to setup and configure the CSS BEAST alarm server. Please read its [documentation] (http://cs-studio.sourceforge.net/docbook/) in case you need help. BEAST will use the [Java Message Service] (https://en.wikipedia.org/wiki/Java_Message_Service) protocol to communicate to its clients.

Therefore you will need a JMS Message Broker to handle the message passing. There are various implementations of the JMS protocol, open and closed source ones among them. The implementation used and tested at EP1 is [Apache ActiveMQ] (https://activemq.apache.org/) which is provided for free under the [Apache License 2.0] (https://www.apache.org/licenses/LICENSE-2.0).

To compile the EP1 AlarmNotifications application, you need the [GNU C++ compiler] (https://gcc.gnu.org/), at least at version 4.4.7. Other compilers or older versions of the GCC-C++ may work but are untested. In addition, AlarmNotifications makes use of the [C++11 language standard] (https://en.wikipedia.org/wiki/C%2B%2B11), so you need support for this in your C++ system libraries.

In addition, AlarmNotifications uses the [Qt framework] (https://qt-project.org/), so the Qt4 Core, Gui, Network, and Phonon libraries as well as their corresponding header files are required, at minimum version 4.6.

In the last years, the major Linux desktop environments have agreed on a standard API to display notifications on the desktop. As a client component, [libnotify] (http://www.galago-project.org/docs/api/libnotify/) has been created so that applications can be desktop-agnostic. AlarmNotifications uses it to display its alerts according to the user's taste and settings. For compiling AlarmNotifications, the header files of libnotify are required.

Optionally, the AlarmNotifications desktop widget can also use the new [Status Notifier Item API] (http://www.notmart.org/misc/statusnotifieritem/index.html). To use this, at least Platform version 4.4 of the [KDE Software Compilation] (https://www.kde.org) is required. If the requirement is not fulfilled, the creation of this flavour of AlarmNotifications is skipped automatically. In the meantime, the Status Notifier API has also received support from other desktop environments than KDE Plasma, so running this flavour of AlarmNotifications does not require running a [Plasma Workspace] (https://www.kde.org/workspaces/).

Last but not least, the build process is controlled by [CMake] (http://www.cmake.org/), version 2.6 or higher.

# Compilation

If all the prerequisites listed above are fulfilled, compiling AlarmNotifications should be quite straightforward:

If you haven't done so already, grab the most recent source code from GitHub:
* `git clone --recursive https://github.com/ttrubep1/AlarmNotifications.git`<br />
(The option `--recursive` is necessary to also checkout the submodules used within this project.)

After git has done its job, descend into the newly generated directory:
* `cd AlarmNotifications`

Create a directory for compiling so the built files don't pollute your tree:
* `mkdir build`
* `cd build`

Now get the compiler to work:
* `cmake ..`
* `make`

The last command can be accelerated by adding the parameter `-jN` where `N` represents the number of (virtual) CPUs in your computer because make will activate multi-threaded compilation.

# Flavours

Compiling AlarmNotifications will create several binaries with different purposes and functions:

* `an-daemon`: This is the "daemon" program, that runs on a server and controls the laboratory and e-mail notifications. Nevertheless, there can be any number of daemons on the network including zero - the desktop clients can also run on their own. (**Please note:** This daemon does not require root privileges, so for general security reason you should run it on a normal user account!)
* `an-desktop`: Runs on a user's desktop and displays a notification if an alarm occurs. The delay between alarm trigger and notification display can be configured.
* `an-desktop-beamtime`: Only created if the beedo framework has been activated (see below). Plays a video in an endless loop until the alarm is acknowledged in addition to the notification.
* `an-desktop-kde4`: Sibling of `an-desktop`, uses the Status Notifier Item API instead of the old QSystemTrayIcon.
* `an-desktop-kde4-beamtime`: Only created if the beedo framework has been activated (see below). Sibling of `an-desktop-beamtime`, uses the Status Notifier Item API instead of the old QSystemTrayIcon.

# Opto-acoustic alarms: The "Beedo" engine

This flavour of the AlarmNotifications application has been written with a test beam control room situation in mind, where the attention of the shift crew has to be drawn to a problem immediately. This feature is known as the "beedo engine".

In addition to the usual notification, this program will create a frame-less window (that cannot simply be "clicked away") and play a video (with sound) in an endless loop. As such a video would be site-specific, it is not included in this repository. To enable the beedo engine, just create such a video and place it as `beedo.ogv` (Ogg Theora video codec recommended) in the source code directory.

The video should have a resolution of 256*256 pixels. If your video differs, just change the definition of `const QSize widgetsize` in `beedo.cpp`.

After you placed the video, just re-run CMake and recompile AlarmNotifications. The `-beamtime` flavours should now also be created.
Please note that the video will actually be integrated into the executable, so nobody can exchange the file on night shift... :wink:

# Configuration

The configuration of all the AlarmNotifications application are stored in `~/.config/alarmnotifications.ini` in the user's home directory.

Please note that you must create this configuration file and place the settings for the connection to the ActiveMQ message broker in there before you can start an AlarmNotifications application for the first time, because otherwise it will get stuck on startup.

These are all the configuration options currently supported:

### ActiveMQURI

The URI given to the ActiveMQ client library to tell it where to look for the message broker. Usually it should look like `failover:(tcp://[hostname]:61616)`. The `failover` part tells the client to re-establish the connection if it should be interrupted.

### ActiveMQUsername

The username to connect to the message broker. Please refer to the documentation of your JMS server to find out what you have to put here.

### ActiveMQPasswort

The username to connect to the message broker. Please refer to the documentation of your JMS server to find out what you have to put here.

### ActiveMQTopicName

The JMS topic your CSS BEAST talks to. This can be found in the logfile after the startup of BEAST. Usually, it is the name of your alarm server configuration with `_SERVER` appended.

### LaboratoryNotificationTimeout

The time in seconds that should pass between the trigger of an alarm and the activation of the flashlight. A value of 0 disables laboratory notifications. The desktop flavours will ignore this setting and never try to enable a flashlight.

### DesktopNotificationTimeout

The time in seconds that should pass between the trigger of an alarm and the display of a notification on the desktop. A value of 0 disables desktop notifications on the desktop. Please note that `an-daemon` also can show desktop notifications, because it may be running on a central lab computer.

### EMailNotificationTimeout

The time in seconds that should pass between the trigger of an alarm and the submission of an e-mail. A value of 0 disables e-mail notifications. To reduce e-mail traffic, the e-mail will be created when one alarm is active long enough but include all other alarms active at that moment. The desktop flavours will ignore this setting and never try to send e-mails.

### EMailNotificationServerName

The domain name (or IP address) of the SMTP server used to send e-mails. Please make sure that the server is configured to act as an [open relay] (https://en.wikipedia.org/wiki/Open_mail_relay) for the e-mails sent by `an-daemon` but not for the entire internet as this will turn your server into a cesspool full of spam.

### EMailNotificationServerPort

The TCP port where the SMTP server is listening, usually 25. Please do not use port 587 (submission) because this is reserved for SMTP user accounts requiring authentication, but `an-daemon` needs an open relay.

### EMailNotificationFrom

E-mail address that should be used as the sender of the alarm notification e-mails. Can be a pure e-mail address or name and address in the form `Max Mustermann <mustermann@example.com>`.

### EMailNotificationTo

E-mail address (e.g. address of a mailing list) that receives the alarm notifications. Can be a pure e-mail address or name and address in the form `Max Mustermann <mustermann@example.com>`.

### FlashLightRelaisDeviceNode

The device file where the commands to the relais controlling the flash light should be written to. Usually a path like `/dev/ttyUSBX` with `X` representing a number `0` or greater. Please set the permissions on this device accordingly, so that `an-daemon` can run without root access and still write to the device. To achieve this you may want to [write an udev rule] (http://www.reactivated.net/writing_udev_rules.html) for your relais.

# Flashlight hardware

Here at EP1, the flashlight used for laboratory notifications is operated via an USB-controllable relais that simply switches the 12 V supply voltage on and off.
The protocol implemented in `flashlight.cpp` is very easy, but device specific. The API for the flashlight operation is rather simple: The class `FlashLight` provides only two static methods: `switchOn()` and `switchOff()`, so you can easily replace our implementation with one for your device. If your relais also has a serial interface (e.g. via USB) you may reuse our code and just change `createCommand()` according to your needs.<br />
<br />
<br />
<br />
Bochum, in autumn 2014<br />
Tobias Triffterer<br />
[tobias@ep1.ruhr-uni-bochum.de] (mailto:tobias@ep1.ruhr-uni-bochum.de)
