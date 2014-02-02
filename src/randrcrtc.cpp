/*
 * Copyright (c) 2012 Francisco Salvador Ballina Sánchez <zballinita@gmail.com>
 * Copyright (c) 2007      Gustavo Pichorim Boiko <gustavo.boiko@kdemail.net>
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

#include "randrcrtc.h"
#include "randrscreen.h"
#include "randroutput.h"
#include "randrmode.h"
#include "randrgammainfo.h"

RandRCrtc::RandRCrtc(RandRScreen *parent, RRCrtc id)
    : QObject(parent),
      m_currentRect(0, 0, 0, 0),
      m_originalRect(m_currentRect),
      m_proposedRect(m_originalRect),
      m_proposedBrightness(1.0),
      m_currentVirtualRect(0, 0, 0, 0),
      m_originalVirtualRect(m_currentVirtualRect),
      m_proposedVirtualRect(m_currentVirtualRect)
{
    m_screen = parent;
    Q_ASSERT(m_screen);

    m_currentRotation = m_originalRotation = m_proposedRotation = RandR::Rotate0;
    m_currentRate = m_originalRate = m_proposedRate = 0;
    m_currentMode = 0;
    m_originalBrightness = 1.0;
    m_rotations = RandR::Rotate0;
    m_currentTracking = m_originalTracking = m_proposedTracking = true;
    m_currentVirtualModeEnabled = m_originalVirtualModeEnabled = m_proposedVirtualModeEnabled = false;
    memset (&m_transform, '\0', sizeof (m_transform));
    m_filter = new char[9];
    sprintf(m_filter, "bilinear");

    m_id = id;
}

RandRCrtc::~RandRCrtc()
{
    delete m_filter;
}

RRCrtc RandRCrtc::id() const
{
    return m_id;
}

int RandRCrtc::rotations() const
{
    return m_rotations;
}

int RandRCrtc::rotation() const
{
    return m_currentRotation;
}

float RandRCrtc::brightness() const
{
    return m_currentBrightness;
}

QRect RandRCrtc::virtualRect() const
{
    return m_currentVirtualRect;
}

bool RandRCrtc::tracking() const
{
    return m_currentTracking;
}

bool RandRCrtc::virtualModeEnabled() const
{
    return m_currentVirtualModeEnabled;
}

bool RandRCrtc::isValid(void) const
{
    return m_id != None;
}

void RandRCrtc::loadSettings(bool notify)
{
    if(m_id == None)
        return;

    qDebug() << "Querying information about CRTC" << m_id;

    int changes = 0;
    XRRCrtcInfo *info = XRRGetCrtcInfo(QX11Info::display(), m_screen->resources(), m_id);
    Q_ASSERT(info);

    if (RandR::timestamp != info->timestamp)
        RandR::timestamp = info->timestamp;

    QRect rect = QRect(info->x, info->y, info->width, info->height);
    if (rect != m_currentRect)
    {
        m_currentRect = rect;
        changes |= RandR::ChangeRect;
    }
    
    // Get panning
    XRRPanning  *panning_info = XRRGetPanning(QX11Info::display(), m_screen->resources(), m_id);
    rect = QRect(panning_info->left, panning_info->top, panning_info->width, panning_info->height);
    if(rect != m_currentVirtualRect)
    {
        m_currentVirtualRect = rect;
        changes |= RandR::ChangeVirtualRect;
    }
    if( rect.width() != info->width || rect.height() != info->height )
    {
        m_currentTracking = true;
        changes |= RandR::ChangeVirtualRect;
    }
    else
       m_currentTracking = false;
    XRRFreePanning(panning_info);
    
    // Get red, blue, green and brightness
    float _brightness;
    get_gamma_info(QX11Info::display(), m_screen->resources(), m_id, &_brightness, &red, &blue, &green);
    
    if(_brightness != m_currentBrightness)
    {
        m_currentBrightness = _brightness;
        changes |= RandR::ChangeBrightness;
    }

    // get all connected outputs
    // and create a list of modes that are available in all connected outputs
    OutputList outputs;

    for (int i = 0; i < info->noutput; ++i) {
        outputs.append(info->outputs[i]);
    }

    // check if the list changed from the original one
    if (outputs != m_connectedOutputs)
    {
        changes |= RandR::ChangeOutputs;
        m_connectedOutputs = outputs;
    }

    // get all outputs this crtc can be connected to
    outputs.clear();
    for (int i = 0; i < info->npossible; ++i)
        outputs.append(info->possible[i]);

    if (outputs != m_possibleOutputs)
    {
        changes |= RandR::ChangeOutputs;
        m_possibleOutputs = outputs;
    }

    // get all rotations
    m_rotations = info->rotations;
    if (m_currentRotation != info->rotation)
    {
        m_currentRotation = info->rotation;
        changes |= RandR::ChangeRotation;
    }

    // check if the current mode has changed
    if (m_currentMode != info->mode)
    {
        m_currentMode = info->mode;
        changes |= RandR::ChangeMode;
    }

    RandRMode m = m_screen->mode(m_currentMode);
    if (m_currentRate != m.refreshRate())
    {
        m_currentRate = m.refreshRate();
        changes |= RandR::ChangeRate;
    }
    /*
    if (m_currentBrightness != brightness())
    {
        m_currentBrightness = brightness();
        changes |= RandR::ChangeBrightness;
    }
    */
    if (m_currentRed != red)
    {
        m_currentRed = red;
        changes |= RandR::ChangeBrightness;
    }
    
    if (m_currentGreen != green)
    {
        m_currentGreen = green;
        changes |= RandR::ChangeBrightness;
    }
    
    if (m_currentBlue != blue)
    {
        m_currentBlue = blue;
        changes |= RandR::ChangeBrightness;
    }
    // just to make sure it gets initialized
    m_proposedRect = m_currentRect;
    m_proposedRotation = m_currentRotation;
    m_proposedRate = m_currentRate;
    m_proposedBrightness = m_currentBrightness;
    m_proposedRed = m_currentRed;
    m_proposedGreen = m_currentGreen;
    m_proposedBlue = m_currentBlue;
    m_proposedVirtualRect = m_currentVirtualRect;
    m_proposedTracking = m_currentTracking;
    m_proposedVirtualModeEnabled = m_currentVirtualModeEnabled;

    // free the info
    XRRFreeCrtcInfo(info);

    if (changes && notify)
        emit crtcChanged(m_id, changes);
}

void RandRCrtc::handleEvent(XRRCrtcChangeNotifyEvent *event)
{
    qDebug() << "[CRTC] Event...";
    int changed = 0;

    if (event->mode != m_currentMode)
    {
        qDebug() << "   Changed mode";
        changed |= RandR::ChangeMode;
        m_currentMode = event->mode;
    }

    if (event->rotation != m_currentRotation)
    {
        qDebug() << "   Changed rotation: " << event->rotation;
        changed |= RandR::ChangeRotation;
        m_currentRotation = event->rotation;
    }
    if (event->x != m_currentRect.x() || event->y != m_currentRect.y())
    {
        qDebug() << "   Changed position: " << event->x << "," << event->y;
        changed |= RandR::ChangeRect;
        m_currentRect.moveTopLeft(QPoint(event->x, event->y));
    }

    RandRMode mode = m_screen->mode(m_currentMode);
    if (mode.size() != m_currentRect.size())
    {
        qDebug() << "   Changed size: " << mode.size();
        changed |= RandR::ChangeRect;
        m_currentRect.setSize(mode.size());
        //Do NOT use event->width and event->height here, as it is being returned wrongly
    }

    if (changed)
        emit crtcChanged(m_id, changed);
}

RandRMode RandRCrtc::mode() const
{
    return m_screen->mode(m_currentMode);
}

QRect RandRCrtc::rect() const
{
    return m_currentRect;
}

float RandRCrtc::refreshRate() const
{
    return m_currentRate;
}

bool RandRCrtc::applyProposed()
{
    qDebug() << "Applying proposed changes for CRTC" << m_id << "...";
    qDebug() << "       Current Screen rect:" << m_screen->rect();
    qDebug() << "       Current CRTC rect:" << m_currentRect;
    qDebug() << "       Current rotation:" << m_currentRotation;
    qDebug() << "       Proposed CRTC rect:" << m_proposedRect;
    qDebug() << "       Proposed rotation:" << m_proposedRotation;
    qDebug() << "       Proposed refresh rate:" << m_proposedRate;
    qDebug() << "       Proposed brightness:" << m_proposedBrightness;
    qDebug() << "       Enabled outputs:";
    if (m_connectedOutputs.isEmpty())
        qDebug() << "          - none";
    for (int i = 0; i < m_connectedOutputs.count(); ++i)
        qDebug() << "          -" << m_screen->output(m_connectedOutputs.at(i))->name();

    RandRMode mode;
    if (m_proposedRect.size() == m_currentRect.size() && m_proposedRate == m_currentRate)
    {
        mode = m_screen->mode(m_currentMode);
    }
    else
    {
        // find a mode that has the desired size and is supported
        // by all connected outputs
        ModeList modeList = modes();
        ModeList matchModes;

        foreach(RRMode m, modeList)
        {
            RandRMode mode = m_screen->mode(m);
            if (mode.size() == m_proposedRect.size())
                matchModes.append(m);
        }

        // if no matching modes were found, disable output
        // else set the mode to the first mode in the list. If no refresh rate was given
        // or no mode was found matching the given refresh rate, the first mode of the
        // list will be used
        if (!matchModes.count())
            mode = RandRMode();
        else
            mode = m_screen->mode(matchModes.first());

        foreach(RRMode m, matchModes)
        {
            RandRMode testMode = m_screen->mode(m);
            if (testMode.refreshRate() == m_proposedRate)
            {
                mode = testMode;
                break;
            }
        }
    }

    // if no output was connected, set the mode to None
    if (!m_connectedOutputs.count())
        mode = RandRMode();
    else if (!mode.isValid())
        return false;

    if (mode.isValid())
    {
        if (m_currentRotation == m_proposedRotation ||
            (m_currentRotation == RandR::Rotate0 && m_proposedRotation == RandR::Rotate180) ||
            (m_currentRotation == RandR::Rotate180 && m_proposedRotation == RandR::Rotate0) ||
            (m_currentRotation == RandR::Rotate90 && m_proposedRotation == RandR::Rotate270) ||
            (m_currentRotation == RandR::Rotate270 && m_proposedRotation == RandR::Rotate90))
        {
            QRect r = QRect(0,0,0,0).united(m_proposedRect);
            if (r.width() > m_screen->maxSize().width() || r.height() > m_screen->maxSize().height())
                return false;

            // if the desired mode is bigger than the current screen size, first change the
            // screen size, and then the crtc size
            if (!m_screen->rect().contains(r))
            {
                // try to adjust the screen size
                if (!m_screen->adjustSize(r))
                    return false;
            }

        }
        else
        {

            QRect r(m_proposedRect.topLeft(), QSize(m_proposedRect.height(), m_proposedRect.width()));
            if (!m_screen->rect().contains(r))
            {
                // check if the rotated rect is smaller than the max screen size
                r = m_screen->rect().united(r);
                if (r.width() > m_screen->maxSize().width() || r.height() > m_screen->maxSize().height())
                    return false;

                // adjust the screen size
                r = r.united(m_currentRect);
                if (!m_screen->adjustSize(r))
                    return false;
            }
        }
    }



    if(m_proposedVirtualModeEnabled)
    {
        /////////////////////////////////////
        // Changing Size "--fb"
        m_screen->setSize( m_proposedVirtualRect.size() );
        /////////////////////////////////////

        if(m_proposedTracking)
        {/*
            s = XRRSetCrtcConfig(QX11Info::display(), m_screen->resources(), m_id,
                        RandR::timestamp, m_proposedVirtualRect.width(), m_proposedVirtualRect.height(),  mode.id(),
                        m_proposedRotation, outputs, m_connectedOutputs.count());*/
        }
    }

    { // Set scale
        int major, minor; // Chack XRandr version. Xrandr 1.3 needed.
        XRRQueryVersion (QX11Info::display(), &major, &minor);
        if (major > 1 || (major == 1 && minor >= 3))
        {
            int nparams = 0;
            XFixed *params = NULL;
            memset (&m_transform, '\0', sizeof (m_transform));
            float width;
            float height;
            if(m_proposedTracking || !m_proposedVirtualModeEnabled)
                width = height = 1.0;
            else
            {
                width = (float)m_proposedVirtualRect.size().width() / (float)m_proposedRect.size().width();
                height = (float)m_proposedVirtualRect.size().height() / (float)m_proposedRect.size().height();
            }
            m_transform.matrix[0][0] = XDoubleToFixed (width);
            m_transform.matrix[1][1] = XDoubleToFixed (height);
            m_transform.matrix[2][2] = XDoubleToFixed (1.0);
            XRRSetCrtcTransform (QX11Info::display(), m_id, &m_transform, m_filter, params, nparams);
            qDebug() << "[RandRCrtc::applyProposed] scale width" << width << "height=" << height;
        }
    }


    RROutput *outputs = new RROutput[m_connectedOutputs.count()];
    for (int i = 0; i < m_connectedOutputs.count(); ++i)
        outputs[i] = m_connectedOutputs.at(i);

    Status s;
    s = XRRSetCrtcConfig(QX11Info::display(), m_screen->resources(), m_id,
                RandR::timestamp, m_proposedRect.x(), m_proposedRect.y(), mode.id(),
                m_proposedRotation, outputs, m_connectedOutputs.count());

    delete[] outputs;

    // Set panning
    if(m_proposedVirtualModeEnabled)
    {
        /////////////////////////////////////
        XRRPanning *panning = XRRGetPanning  (QX11Info::display(),m_screen->resources(), m_id);
        panning->left = panning->top = 0;
        panning->width = m_proposedVirtualRect.width();
        panning->height = m_proposedVirtualRect.height();
        panning->track_width = 0;
        panning->track_height = 0;
        panning->track_left = panning->track_top = 0;
        panning->timestamp = RandR::timestamp;
        {
        	Status s = XRRSetPanning (QX11Info::display(), m_screen->resources(), m_id, panning);
        	if (s == RRSetConfigSuccess)
        		qDebug() << "[RandRCrtc::applyProposed] Panning changed";
        	else
        		qDebug() << "[RandRCrtc::applyProposed] Panning doesn't changed";
        }
        XRRFreePanning(panning);
        /////////////////////////////////////
    }
    
    // Set gamma
    //Gamma is applied twice
    qDebug() << "[RandRCrtc::applyProposed] m_proposedBrightness" << m_proposedBrightness;
    // Wait for set gamma. Wait for Xrandr setting brightness when virtual size is changed
    sleep(3);
    //XFlush(QX11Info::display());
    //XSync(QX11Info::display(), False);
    //Gamma is applied twice
    set_gamma(QX11Info::display(), m_screen->resources(), m_id, m_proposedBrightness, red, blue, green);
    set_gamma(QX11Info::display(), m_screen->resources(), m_id, m_proposedBrightness, red, blue, green);
    m_currentBrightness = m_proposedBrightness;
    
    

    bool ret;
    if (s == RRSetConfigSuccess)
    {
        qDebug() << "Changes for CRTC" << m_id << "successfully applied.";
        m_currentMode = mode.id();
        m_currentRotation = m_proposedRotation;
        m_currentRect = m_proposedRect;
        m_currentRate = mode.refreshRate();
        m_currentVirtualRect = m_proposedVirtualRect;
        m_currentTracking = m_proposedTracking;
        m_currentVirtualModeEnabled = m_proposedVirtualModeEnabled;
        
        emit crtcChanged(m_id, RandR::ChangeMode);
        ret = true;
    }
    else
    {
        qDebug() << "Failed to apply changes for CRTC" << m_id;
        ret = false;
        // Invalidate the XRRScreenResources cache
        if(s == RRSetConfigInvalidConfigTime)
            m_screen->loadSettings(true);
    }

    if(!m_proposedVirtualModeEnabled)
        m_screen->adjustSize();
    return ret;
}

bool RandRCrtc::proposeSize(const QSize &s)
{
    m_proposedRect.setSize(s);
    m_proposedRate = 0;
    return true;
}

bool RandRCrtc::proposeVirtualSize(const QSize &s)
{
    m_proposedVirtualRect.setSize(s);
    return true;
}

bool RandRCrtc::proposeTracking(bool tracking)
{
    m_proposedTracking = tracking;
    return true;
}

bool RandRCrtc::proposeVirtualModeEnabled(bool enabled)
{
    m_proposedVirtualModeEnabled = enabled;
    return true;
}

bool RandRCrtc::proposePosition(const QPoint &p)
{
    m_proposedRect.moveTopLeft(p);
    return true;
}

bool RandRCrtc::proposeRotation(int rotation)
{
    // check if this crtc supports the asked rotation
    if (!rotation & m_rotations)
        return false;

    m_proposedRotation = rotation;
    return true;

}

bool RandRCrtc::proposeRefreshRate(float rate)
{
    m_proposedRate = rate;
    return true;
}

bool RandRCrtc::proposeBrightness(float _brightness)
{
    m_proposedBrightness = _brightness;
    return true;
}

void RandRCrtc::proposeOriginal()
{
    m_proposedRotation = m_originalRotation;
    m_proposedRect = m_originalRect;
    m_proposedRate = m_originalRate;
    m_proposedBrightness = m_originalBrightness;
    m_proposedVirtualRect = m_originalVirtualRect;
    m_proposedTracking = m_originalTracking;
    m_proposedVirtualModeEnabled = m_originalVirtualModeEnabled;
}

void RandRCrtc::setOriginal()
{
    m_originalRotation = m_currentRotation;
    m_originalRect = m_currentRect;
    m_originalRate = m_currentRate;
    m_originalBrightness = m_currentBrightness;
    m_originalVirtualRect = m_currentVirtualRect;
    m_originalTracking = m_currentTracking;
    m_originalVirtualModeEnabled = m_currentVirtualModeEnabled;
}

bool RandRCrtc::proposedChanged()
{
    return (m_proposedRotation != m_currentRotation ||
        m_proposedRect != m_currentRect ||
        m_proposedRate != m_currentRate ||
        m_proposedBrightness != m_currentBrightness ||
        m_proposedVirtualRect != m_currentVirtualRect ||
        m_proposedTracking != m_currentTracking ||
        m_proposedVirtualModeEnabled != m_currentVirtualModeEnabled);
}

bool RandRCrtc::addOutput(RROutput output, const QSize &s)
{
    QSize size = s;
    // if no mode was given, use the current one
    if (!size.isValid())
        size = m_currentRect.size();

    // check if this output is not already on this crtc
    // if not, add it
    if (m_connectedOutputs.indexOf(output) == -1)
    {
        // the given output is not possible
        if (m_possibleOutputs.indexOf(output) == -1)
            return false;

        m_connectedOutputs.append(output);
    }
    m_proposedRect = QRect(m_proposedRect.topLeft(), s);
    return true;
}

bool RandRCrtc::removeOutput(RROutput output)
{
    int index = m_connectedOutputs.indexOf(output);
    if (index == -1)
        return false;

    m_connectedOutputs.removeAt(index);
    return true;
}

OutputList RandRCrtc::connectedOutputs() const
{
    return m_connectedOutputs;
}

ModeList RandRCrtc::modes() const
{
    ModeList modeList;

    bool first = true;

    foreach(RROutput o, m_connectedOutputs)
    {
        RandROutput *output = m_screen->output(o);
        if (first)
        {
            modeList = output->modes();
            first = false;
        }
        else
        {
            foreach(RRMode m, modeList)
            {
                if (output->modes().indexOf(m) == -1)
                    modeList.removeAll(m);
            }
        }
    }

    return modeList;
}
