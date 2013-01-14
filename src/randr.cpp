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

#include <QtGui/QIcon>
#include "qtimerconfirmdialog.h"
#include "randr.h"

bool RandR::has_1_2 = true;
bool RandR::has_1_3 = true;
Time RandR::timestamp = 0;

QString RandR::rotationName(int rotation, bool pastTense, bool capitalised)
{
    if (!pastTense)
        switch (rotation) {
            case RR_Rotate_0:
                return QObject::tr("No Rotation");
            case RR_Rotate_90:
                return QObject::tr("Left (90 degrees)");
            case RR_Rotate_180:
                return QObject::tr("Upside-Down (180 degrees)");
            case RR_Rotate_270:
                return QObject::tr("Right (270 degrees)");
            case RR_Reflect_X:
                return QObject::tr("Mirror Horizontally");
            case RR_Reflect_Y:
                return QObject::tr("Mirror Vertically");
            default:
                return QObject::tr("Unknown Orientation");
        }

    switch (rotation) {
        case RR_Rotate_0:
            return QObject::tr("Not Rotated");
        case RR_Rotate_90:
            return QObject::tr("Rotated 90 Degrees Counterclockwise");
        case RR_Rotate_180:
            return QObject::tr("Rotated 180 Degrees Counterclockwise");
        case RR_Rotate_270:
            return QObject::tr("Rotated 270 Degrees Counterclockwise");
        default:
            if (rotation & RR_Reflect_X)
                if (rotation & RR_Reflect_Y)
                    if (capitalised)
                        return QObject::tr("Mirrored Horizontally And Vertically");
                    else
                        return QObject::tr("mirrored horizontally and vertically");
                else
                    if (capitalised)
                        return QObject::tr("Mirrored Horizontally");
                    else
                        return QObject::tr("mirrored horizontally");
            else if (rotation & RR_Reflect_Y)
                if (capitalised)
                    return QObject::tr("Mirrored Vertically");
                else
                    return QObject::tr("mirrored vertically");
            else
                if (capitalised)
                    return QObject::tr("Unknown Orientation");
                else
                    return QObject::tr("unknown orientation");
    }
}

QPixmap RandR::rotationIcon(int rotation, int currentRotation)
{
    // Adjust icons for current screen orientation
    if (!(currentRotation & RR_Rotate_0) && rotation & (RR_Rotate_0 | RR_Rotate_90 | RR_Rotate_180 | RR_Rotate_270)) {
        int currentAngle = currentRotation & (RR_Rotate_90 | RR_Rotate_180 | RR_Rotate_270);
        switch (currentAngle) {
            case RR_Rotate_90:
                rotation <<= 3;
                break;
            case RR_Rotate_180:
                rotation <<= 2;
                break;
            case RR_Rotate_270:
                rotation <<= 1;
                break;
        }

        // Fix overflow
        if (rotation > RR_Rotate_270) {
            rotation >>= 4;
        }
    }

    switch (rotation) {
        case RR_Rotate_0:
        return QPixmap(":/images/go-up.png");
        case RR_Rotate_90:
            return QPixmap(":/images/go-previous.png");
        case RR_Rotate_180:
            return QPixmap(":/images/go-down.png");
        case RR_Rotate_270:
            return QPixmap(":/images/go-next.png");
        case RR_Reflect_X:
            return QPixmap(":/images/object-flip-horizontal.png");
        case RR_Reflect_Y:
            return QPixmap(":/images/object-flip-vertical.png");
        default:
            return QPixmap(":/images/process-stop.png");
    }
}

bool RandR::confirm(const QRect &rect)
{
    Q_UNUSED(rect);

    qDebug() << "Confirm the changes";
    QTimerConfirmDialog acceptDialog(15000, QObject::tr("Your screen configuration has been "
                                                        "changed to the requested settings.\n"
                                                        "Please indicate whether you wish to keep "
                                                        "this configuration.\nIn 15 seconds the "
                                                        "display will revert to your previous "
                                                        "settings."),
                                     QObject::tr("Confirm Display Setting Change"),
                                     true, QTimerConfirmDialog::CountDown, "mainKTimerDialog");

    return acceptDialog.exec();
}

SizeList RandR::sortSizes(const SizeList &sizes)
{
    int *sizeSort = new int[sizes.count()];
    int numSizes = sizes.count();
    SizeList sorted;

    int i = 0;
    foreach(const QSize &size, sizes)
        sizeSort[i++] = size.width() * size.height();

    for (int j = 0; j < numSizes; j++)
    {
        int highest = -1, highestIndex = -1;

        for (int i = 0; i < numSizes; i++)
        {
            if (sizeSort[i] && sizeSort[i] > highest)
            {
                highest = sizeSort[i];
                highestIndex = i;
            }
        }
        sizeSort[highestIndex] = -1;
        Q_ASSERT(highestIndex != -1);

        sorted.append(sizes[highestIndex]);
    }
    delete [] sizeSort;
    sizeSort = 0L;

    return sorted;
}
