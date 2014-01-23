/*
 * Copyright (c) 2007, 2008 Harry Bock <hbock@providence.edu>
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

#include <QtGui/QMessageBox>
#include <QtGui/QMenu>

#include "collapsiblewidget.h"
#include "outputconfig.h"
#include "outputgraphicsitem.h"
#include "layoutmanager.h"
#include "randrconfig.h"
#include "randroutput.h"
#include "randrdisplay.h"
#include "randrscreen.h"

RandRConfig::RandRConfig(QWidget *parent, RandRDisplay *display)
    : QWidget(parent), Ui::RandRConfigBase()
{
    m_display = display;
    Q_ASSERT(m_display);

    if (!m_display->isValid()) {
        // FIXME: this needs much better handling of this error...
        return;
    }

    qDebug() << "Display is valid";
    setupUi(this);
    layout()->setMargin(0);

    connect( identifyOutputsButton, SIGNAL(clicked()), SLOT(identifyOutputs()));
    connect( &identifyTimer, SIGNAL(timeout()), SLOT(clearIndicators()));
    connect( &compressUpdateViewTimer, SIGNAL(timeout()), SLOT(slotDelayedUpdateView()));
    connect(unifyOutputs, SIGNAL(toggled(bool)), SLOT(unifiedOutputChanged(bool)));

    identifyTimer.setSingleShot( true );
    compressUpdateViewTimer.setSingleShot( true );

    // create the container for the settings widget
    QHBoxLayout *layout = new QHBoxLayout(outputList);
    layout->setSpacing(0);
    layout->setContentsMargins(0,0,0,0);
    m_container = new SettingsContainer(outputList);
    m_container->setSizePolicy(QSizePolicy::Minimum,
                          QSizePolicy::Minimum);
    layout->addWidget(m_container);

#ifdef HAS_RANDR_1_3
    qDebug() << "HAS_RANDR_1_3";
    if (RandR::has_1_3)
    {
        primaryDisplayBox->setVisible(true);
        label->setVisible(true);
    }
    else
#endif //HAS_RANDR_1_3
    {
        primaryDisplayBox->setVisible(false);
        label->setVisible(false);
    }

    QSettings config;
    config.beginGroup("Screen_0");
    bool outputunified = config.value("OutputsUnified", false).toBool();
    config.endGroup();

    if (outputunified)
    {
        unifyOutputs->setChecked(true);
    }
    // create the scene
    qDebug() << "Before create Scene";
    qDebug() << "Current screen rect " << m_display->currentScreen()->rect();

    m_scene = new QGraphicsScene(m_display->currentScreen()->rect(), screenView);
    screenView->setScene(m_scene);
    screenView->installEventFilter(this);

    m_layoutManager = new LayoutManager(m_display->currentScreen(), m_scene);
    qDebug() << "Terminated constructor Config";

    load();
}

RandRConfig::~RandRConfig()
{
    clearIndicators();
}

void RandRConfig::load(void)
{
    if (!m_display->isValid())
    {
        qDebug() << "Invalid display! Aborting config load.";
        return;
    }

    m_scene->clear();
    qDeleteAll(m_outputList);
    m_outputList.clear();
    m_configs.clear(); // objects deleted above

    OutputMap outputs = m_display->currentScreen()->outputs();
#ifdef HAS_RANDR_1_3
    RandROutput *primary = m_display->currentScreen()->primaryOutput();
    if (RandR::has_1_3)
    {
        // disconnect while we repopulate the combo box
        disconnect(primaryDisplayBox, SIGNAL(currentIndexChanged(int)), this, SIGNAL(changed()));
        disconnect(primaryDisplayBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePrimaryDisplay()));
        primaryDisplayBox->clear();
        primaryDisplayBox->addItem(tr("No display selected"), "None");
    }
#endif //HAS_RANDR_1_3

    // FIXME: adjust it to run on a multi screen system
    CollapsibleWidget *w;
    OutputGraphicsItem *o;
    OutputConfigList preceding;
    foreach(RandROutput *output, outputs)
    {
        OutputConfig *config = new OutputConfig(this, output, preceding, unifyOutputs->isChecked());
        m_configs.append( config );
        preceding.append( config );

        QString description = output->isConnected()
            ? tr("%1 (Connected)").arg(output->name())
            : output->name();
        w = m_container->insertWidget(config, description);
        if(output->isConnected()) {
            w->setExpanded(true);
            qDebug() << "Output rect:" << output->rect();
        }
        connect(config, SIGNAL(connectedChanged(bool)), this, SLOT(outputConnectedChanged(bool)));
        m_outputList.append(w);

        o = new OutputGraphicsItem(config);
        m_scene->addItem(o);

        connect(o,    SIGNAL(itemChanged(OutputGraphicsItem*)),
                this, SLOT(slotAdjustOutput(OutputGraphicsItem*)));

        connect(config, SIGNAL(updateView()), this, SLOT(slotUpdateView()));
        connect(config, SIGNAL(optionChanged()), this, SIGNAL(changed()));

#ifdef HAS_RANDR_1_3
        if (RandR::has_1_3 && output->isConnected())
        {
            primaryDisplayBox->addItem(output->name(), QVariant::fromValue(output->id()));
            if (primary == output)
            {
                primaryDisplayBox->setCurrentIndex(primaryDisplayBox->count()-1);
            }
        }
#endif //HAS_RANDR_1_3
    }
#ifdef HAS_RANDR_1_3
    if (RandR::has_1_3)
    {
        connect(primaryDisplayBox, SIGNAL(currentIndexChanged(int)), this, SIGNAL(changed()));
        connect(primaryDisplayBox, SIGNAL(currentIndexChanged(int)), this, SLOT(updatePrimaryDisplay()));
    }
#endif //HAS_RANDR_1_3
    slotUpdateView();
}

void RandRConfig::outputConnectedChanged(bool connected)
{
    OutputConfig *config = static_cast <OutputConfig *> (sender());
    int index = m_configs.indexOf(config);
    QString description = connected
            ? tr("%1 (Connected)").arg(config->output()->name())
            : config->output()->name();
    m_outputList.at(index)->setCaption(description);
}

void RandRConfig::save()
{
    if (!m_display->isValid())
        return;

    QSettings config;
    config.beginGroup("Screen_0");
    config.setValue("OutputsUnified", unifyOutputs->isChecked());
    config.endGroup();

    apply();
}

void RandRConfig::defaults()
{
    update();
}

void RandRConfig::apply()
{
    qDebug() << "Applying settings...";

    // normalize positions so that the coordinate system starts at (0,0)
    QPoint normalizePos;
    bool first = true;
    foreach(CollapsibleWidget *w, m_outputList)
    {
        OutputConfig *config = static_cast<OutputConfig *>(w->innerWidget());
        if( config->isActive())
        {
            QPoint pos = config->position();
            if( first )
            {
                normalizePos = pos;
                first = false;
            } else
            {
                if( pos.x() < normalizePos.x())
                    normalizePos.setX( pos.x());
                if( pos.y() < normalizePos.y())
                    normalizePos.setY( pos.y());
            }
        }
    }
    normalizePos = -normalizePos;
    qDebug() << "Normalizing positions by" << normalizePos;

    foreach(CollapsibleWidget *w, m_outputList)
    {
        OutputConfig *config = static_cast<OutputConfig *>(w->innerWidget());
        RandROutput *output = config->output();

        if(!output->isConnected())
            continue;

        QSize res = config->resolution();

        if(!res.isNull())
        {
            if(!config->hasPendingChanges( normalizePos ))
            {
                qDebug() << "Ignoring identical config for" << output->name();
                continue;
            }
            QRect configuredRect(config->position(), res);

            qDebug() << "Output config for" << output->name() << ":\n"
                        "  rect =" << configuredRect
                     << ", rot =" << config->rotation()
                     << ", rate =" << config->refreshRate()
                     << ", brightness " << config->brightness();

            // Break the connection with the previous CRTC for changed outputs, since
            // otherwise the code could try to use the same CRTC for two different outputs.
            // This is probably rather hackish and may not always work, but I don't see
            // a better way with this codebase, definitely not with the time I have now.
            output->disconnectFromCrtc();

            output->proposeRect(configuredRect.translated( normalizePos ));
            output->proposeRotation(config->rotation());
            output->proposeRefreshRate(config->refreshRate());
            output->proposeBrightness(config->brightness());
            output->proposeVirtualSize(config->virtualSize());
            output->proposeTracking(config->tracking());
            output->proposeVirtualModeEnabled(config->virtualModeEnabled());
        } else // user wants to disable this output
        {
            qDebug() << "Disabling" << output->name();
            output->slotDisable();
        }
    }
#ifdef HAS_RANDR_1_3
    if (RandR::has_1_3)
    {
        int primaryOutputIndex = primaryDisplayBox->currentIndex();
        RandRScreen *screen = m_display->currentScreen();
        if (primaryOutputIndex > 0)
        {
            QVariant output = primaryDisplayBox->itemData(primaryOutputIndex);
            screen->proposePrimaryOutput(screen->output(output.value<RROutput>()));
        }
        else
        {
            screen->proposePrimaryOutput(0);
        }
    }
#endif //HAS_RANDR_1_3
    m_display->applyProposed();
    update();
}

void RandRConfig::updatePrimaryDisplay()
{
    QString primary=primaryDisplayBox->currentText();
    foreach( QGraphicsItem* item, m_scene->items())
    {
        OutputGraphicsItem* itemo = dynamic_cast< OutputGraphicsItem* >( item );
        if(itemo && (itemo->objectName()==primary)!=itemo->isPrimary())
        {
            itemo->setPrimary(itemo->objectName()==primary);
        }
    }
}

void RandRConfig::update()
{
    // TODO: implement
    emit changed(false);
}

void RandRConfig::unifiedOutputChanged(bool checked)
{
    Q_FOREACH(OutputConfig *config, m_configs) {
        config->setUnifyOutput(checked);
        config->updateSizeList();
    }

    emit changed(true);
}

bool RandRConfig::eventFilter(QObject *obj, QEvent *event)
{
    if ( obj == screenView && event->type() == QEvent::Resize ) {
        slotUpdateView();
        return false;
    } else {
        return QWidget::eventFilter(obj, event);
    }
}

void RandRConfig::slotAdjustOutput(OutputGraphicsItem *o)
{
    Q_UNUSED(o);
    qDebug() << "Output graphics item changed:";

    // TODO: Implement
}

void RandRConfig::slotUpdateView()
{
    compressUpdateViewTimer.start( 0 );
}

void RandRConfig::slotDelayedUpdateView()
{
    QRect r;
    bool first = true;

    // updates the graphics view so that all outputs fit inside of it
    foreach(OutputConfig *config, m_configs)
    {
        if (first)
        {
            first = false;
            r = config->rect();
        }
        else
            r = r.united(config->rect());
    }
    // scale the total bounding rectangle for all outputs to fit
    // 80% of the containing QGraphicsView
    float scaleX = (float)screenView->width() / r.width();
    float scaleY = (float)screenView->height() / r.height();
    float scale = (scaleX < scaleY) ? scaleX : scaleY;
    scale *= 0.80f;

    screenView->resetMatrix();
    screenView->scale(scale,scale);
    screenView->ensureVisible(r);
    screenView->setSceneRect(r);

    foreach( QGraphicsItem* item, m_scene->items()) {
        if( OutputGraphicsItem* itemo = dynamic_cast< OutputGraphicsItem* >( item ))
            itemo->configUpdated();
    }
    updatePrimaryDisplay();
    screenView->update();
}

uint qHash( const QPoint& p )
{
    return p.x() * 10000 + p.y();
}

void RandRConfig::identifyOutputs()
{
    identifyTimer.stop();
    clearIndicators();
    QHash< QPoint, QStringList > ids; // outputs at centers of screens (can be more in case of clone mode)
    OutputMap outputs = m_display->currentScreen()->outputs();
    foreach(RandROutput *output, outputs)
    {
        if( !output->isConnected() || output->rect().isEmpty())
            continue;
        ids[ output->rect().center() ].append( output->name());
    }
    for( QHash< QPoint, QStringList >::ConstIterator it = ids.constBegin();
         it != ids.constEnd();
         ++it )
    {
        QLabel *si = new QLabel(it->join("\n"), NULL, Qt::X11BypassWindowManagerHint);
        QFont fnt = QApplication::font();
        fnt.setPixelSize(100);
        si->setFont(fnt);
        si->setFrameStyle(QFrame::Panel);
        si->setFrameShadow(QFrame::Plain);
        si->setAlignment(Qt::AlignCenter);
        QRect targetGeometry(QPoint(0,0), si->sizeHint());
            targetGeometry.moveCenter(it.key());
        si->setGeometry(targetGeometry);
        si->show();
            m_indicators.append( si );
    }
    identifyTimer.start( 1500 );
}

void RandRConfig::clearIndicators()
{
    qDeleteAll( m_indicators );
    m_indicators.clear();
}

void RandRConfig::insufficientVirtualSize()
{
    QMessageBox message(this);

    message.setText(tr( "Insufficient virtual size for the total screen size.\n"
                               "The configured virtual size of your X server is insufficient for this setup. "
                               "This configuration needs to be adjusted.\n"
                            "Changing configuration failed. Please adjust your xorg.conf manually." ));
    message.show();
}

bool RandRConfig::x11Event(XEvent* e)
{
    return QWidget::x11Event(e);
}
