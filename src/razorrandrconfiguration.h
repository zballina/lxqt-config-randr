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

#ifndef RAZORRANDRCONFIGURATION_H
#define RAZORRANDRCONFIGURATION_H

#include <QtGui/QDialog>
#include "randrconfig.h"
#include "randrdisplay.h"

namespace Ui {
class RazorRandrConfiguration;
}

class LXQtRandrConfig : public QDialog
{
    Q_OBJECT

public:
    explicit LXQtRandrConfig(QWidget *parent = 0);
    ~LXQtRandrConfig();
    void about();

private slots:
    void on_buttonBox_clicked(QAbstractButton *button);
    void updateButtons(bool status);

private:

    Ui::RazorRandrConfiguration *mUi;
    RandRConfig *mRandrConfig;
    RandRDisplay *mRandrDisplay;
};

#endif // RAZORRANDRCONFIGURATION_H
