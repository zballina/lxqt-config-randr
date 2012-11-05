/*
 * Copyright (c) 2012 Francisco Salvador Ballina Sánchez <zballinita@gmail.com>
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

#include "razorrandrdialog.h"
#include "ui_razorrandrdialog.h"

RazorRandrDialog::RazorRandrDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RazorRandrDialog), mDisplay(dpy)
{
    ui->setupUi(this);
    mDisplay = new RandRDisplay();
}

RazorRandrDialog::~RazorRandrDialog()
{
    delete ui;
}
