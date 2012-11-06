/*
 * Copyright (c) 2012 Francisco Salvador Ballina Sánchez <zballinita@gmail.com>
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

#include <QtGui/QApplication>
#include <QtCore/QDebug>

#include "razorrandrconfiguration.h"

int main(int argc, char *argv[])
{
    QApplication::setApplicationName("RazorXRandr");
#ifdef RazorXRandr_VERSION
    QApplication::setApplicationVersion(QString("%1").arg(RazorXRandr_VERSION));
#endif
    QApplication::setOrganizationDomain("www.hostingbroad.com");
    QSettings::setDefaultFormat(QSettings::IniFormat);

    QApplication a(argc, argv);
    RazorRandrConfiguration w;
    w.show();

    return a.exec();
}
