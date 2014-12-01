/**
 * @file configscreen.h
 *
 * @author Tobias Triffterer
 *
 * @brief GUI dialog to configure this application
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

#ifndef CONFIGSCREEN_H
#define CONFIGSCREEN_H

#include "oldgcccompat.h" // Compatibilty macros for GCC < 4.7

#include <QtCore/QString>
#include <QtGui/QComboBox>
#include <QtGui/QLabel>
#include <QtGui/QLineEdit>
#include <QtGui/QDialogButtonBox>
#include <QtGui/QFormLayout>
#include <KDE/KConfigSkeleton>
#include <KDE/KPageDialog>
#include <KDE/KConfigDialog>
#include <KDE/KConfigDialogManager>

#include "alarmconfiguration.h"

namespace AlarmNotifications {

class ConfigScreen : public KConfigDialog
{
  Q_OBJECT
private:
    KConfigSkeleton*const _conf;
    KConfigDialogManager*const _confman;
    QWidget*const _activemqscreen;
    QFormLayout*const _lactivemqscreen;
    
    void CreateActiveMQConnectivityWidgets();
public:
    ConfigScreen();
    ConfigScreen ( const ConfigScreen& other ) = delete;
    ConfigScreen ( ConfigScreen&& other ) = delete;
    virtual ~ConfigScreen();
    ConfigScreen& operator= ( const ConfigScreen& other ) = delete;
    ConfigScreen& operator= ( ConfigScreen&& other ) = delete;
};

}

#endif // CONFIGSCREEN_H
