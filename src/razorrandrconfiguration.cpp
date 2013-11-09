/***************************************************************************
 *   Copyright (C) 2012 by Francisco Ballina Sanchez                       *
 *   zballinita@gmail.com                                                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include <QtGui/QMessageBox>

#include "razorrandrconfiguration.h"
#include "ui_razorrandrconfiguration.h"

LXQtRandrConfig::LXQtRandrConfig(QWidget *parent) :
    QDialog(parent),
    mUi(new Ui::RazorRandrConfiguration)
{
    mUi->setupUi(this);
    setWindowIcon(QIcon(":/icons/preferences-desktop-display.png"));
    updateButtons(false);
    mRandrDisplay = new RandRDisplay();
    mRandrConfig = new RandRConfig(this, mRandrDisplay);

    mUi->verticalLayout->addWidget(mRandrConfig);
    connect(mRandrConfig, SIGNAL(changed(bool)), this, SLOT(updateButtons(bool)));
}

LXQtRandrConfig::~LXQtRandrConfig()
{
    delete mRandrDisplay;
    delete mRandrConfig;
    delete mUi;
}

void LXQtRandrConfig::updateButtons(bool status)
{
    mUi->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(status);
    mUi->buttonBox->button(QDialogButtonBox::Apply)->setEnabled(status);
    mUi->buttonBox->button(QDialogButtonBox::Reset)->setEnabled(status);
}

void LXQtRandrConfig::on_buttonBox_clicked(QAbstractButton *button)
{
    if(mUi->buttonBox->button(QDialogButtonBox::Ok) == button)
    {
        mRandrConfig->apply();
        QApplication::quit();
    }
    else if(mUi->buttonBox->button(QDialogButtonBox::Apply) == button)
    {
        mRandrConfig->apply();
        updateButtons(false);
    }
    else if(mUi->buttonBox->button(QDialogButtonBox::Cancel) == button)
    {
        QApplication::quit();
    }
    else if(mUi->buttonBox->button(QDialogButtonBox::Reset) == button)
    {
        mRandrConfig->load();
        updateButtons(false);
    }
    else if(mUi->buttonBox->button(QDialogButtonBox::Help) == button)
    {
        about();
    }
}

void LXQtRandrConfig::about()
{
    QMessageBox::about(this, QString("LXQt Randr ") + STR_VERSION,
                       tr("<p><b>LXQt Randr Configuration</b></p>"
                           "Qt-based tool to configure the X output using "
                          "the RandR 1.3/1.2 extension, based in KDE parts, "
                          "intended to be a viable option for the LXQt desktop.<br><br>"
                          "The program is provided AS IS with NO WARRANTY OF ANY KIND, "
                          "INCLUDING THE WARRANTY OF DESIGN, MERCHANTABILITY "
                          "AND FITNESS FOR A PARTICULAR PURPOSE.<br><br>"
                          "Francisco Ballina Sanchez<br>email: zballinita@gmail.com"));
}
