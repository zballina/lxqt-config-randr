/*
 * Copyright (c) 2012 Francisco Salvador Ballina Sánchez <zballinita@gmail.com>
 * Copyright (c) 2007      Gustavo Pichorim Boiko <gustavo.boiko@kdemail.net>
 * Copyright (c) 2002,2003 Hamish Rodda <rodda@kde.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef RANDRDISPLAY_H
#define RANDRDISPLAY_H

#include <QtGui/QWidget>
#include <QtCore/QSettings>
#include <X11/Xlib.h>

#include "randr.h"

class RandRDisplay
{
public:
    RandRDisplay();
    ~RandRDisplay();

    bool isValid() const;
    const QString& errorCode() const;
    const QString& version() const;

    int eventBase() const;
    int errorBase() const;

    int screenIndexOfWidget(QWidget* widget);

    int numScreens() const;
    LegacyRandRScreen* legacyScreen(int index);
    LegacyRandRScreen* currentLegacyScreen();
//#ifdef HAS_RANDR_1_2
    RandRScreen* screen(int index);
    RandRScreen* currentScreen();
//#endif
    void setCurrentScreen(int index);
    int	currentScreenIndex() const;

    bool needsRefresh() const;
    void refresh();

    /**
     * Loads saved settings.
     *
     * @param config the KConfig object to load from
     * @param loadScreens whether to call LegacyRandRScreen::load() for each screen
     * @retuns true if the settings should be applied on KDE startup.
     */
    bool loadDisplay(QSettings &config, bool loadScreens = true);
    void saveDisplay(QSettings &config, bool syncTrayApp);
    void saveStartup(QSettings &config);
    void disableStartup(QSettings &config);

    static bool applyOnStartup(QSettings &config);
    static bool syncTrayApp(QSettings &config);

    void applyProposed(bool confirm = true);

    bool canHandle(const XEvent *e) const;
    void handleEvent(XEvent *e);

private:
    Display *m_dpy;
    int	m_numScreens;
    int	m_currentScreenIndex;
    LegacyScreenList m_legacyScreens;
//#ifdef HAS_RANDR_1_2
    ScreenList m_screens;
//#endif
    bool m_valid;
    QString	m_errorCode;
    QString	m_version;

    int	m_eventBase;
    int m_errorBase;
};

#endif // RANDRDISPLAY_H
