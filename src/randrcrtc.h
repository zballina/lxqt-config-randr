/*
 * Copyright (c) 2012 Francisco Salvador Ballina Sánchez <zballinita@gmail.com>
 * Copyright (c) 2007 Gustavo Pichorim Boiko <gustavo.boiko@kdemail.net>
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

#ifndef RANDRCRTC_H
#define RANDRCRTC_H

#include <QtGui/QX11Info>
#include <QtCore/QObject>
#include <QtCore/QRect>

#include "randr.h"

/** Class representing a CRT controller. */
class RandRCrtc : public QObject
{
    Q_OBJECT

public:
    RandRCrtc(RandRScreen *parent, RRCrtc id);
    ~RandRCrtc();

    RRCrtc id() const;
    int rotations() const;
    int rotation() const;

    void loadSettings(bool notify = false);
    void handleEvent(XRRCrtcChangeNotifyEvent *event);

    bool isValid(void) const;
    RandRMode mode() const;
    QRect rect() const;
    float refreshRate() const;

    bool proposeSize(const QSize &s);
    bool proposePosition(const QPoint &p);
    bool proposeRotation(int rotation);
    bool proposeRefreshRate(float rate);
    bool proposeBrightness(float brightness);
    bool proposeTracking(bool tracking);
    bool proposeVirtualSize(const QSize &size);
    bool proposeVirtualModeEnabled(bool enable);

    // applying stuff
    bool applyProposed();
    void proposeOriginal();
    void setOriginal();
    bool proposedChanged();

    bool addOutput(RROutput output, const QSize &size = QSize());
    bool removeOutput(RROutput output);
    OutputList connectedOutputs() const;

    ModeList modes() const;
    
    //Gamma vaules
    float red, blue, green;
    float brightness() const;
    
    // Virtual modes
    QRect virtualRect() const;
    bool tracking() const;
    bool virtualModeEnabled() const;

signals:
    void crtcChanged(RRCrtc c, int changes);

private:
    RRCrtc m_id;
    RRMode m_currentMode;

    QRect m_currentRect;
    QRect m_currentVirtualRect;
    float m_currentRate;
    int m_currentRotation;
    float m_currentBrightness;
    float m_currentRed;
    float m_currentBlue;
    float m_currentGreen;
    bool m_currentTracking;
    bool m_currentVirtualModeEnabled;


    QRect m_originalRect;
    QRect m_originalVirtualRect;
    float m_originalRate;
    int m_originalRotation;
    float m_originalBrightness;
    bool m_originalTracking;
    bool m_originalVirtualModeEnabled;

    QRect m_proposedRect;
    QRect m_proposedVirtualRect;
    float m_proposedRate;
    int m_proposedRotation;
    float m_proposedBrightness;
    float m_proposedRed;
    float m_proposedGreen;
    float m_proposedBlue;
    bool m_proposedTracking;
    bool m_proposedVirtualModeEnabled;

    OutputList m_connectedOutputs;
    OutputList m_possibleOutputs;
    int m_rotations;
    
    XTransform m_transform;
    char *m_filter;

    RandRScreen *m_screen;
};

#endif // RANDRCRTC_H
