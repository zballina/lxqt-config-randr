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
#include <QtCore/QSettings>
#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <getopt.h>
#include <stdlib.h>

#include "razorrandrconfiguration.h"
#include "loaderconfiglogin.h"

#define out

const char* const short_options = "vhs";

const struct option long_options[] = {
    {"version", 0, NULL, 'v'},
    {"help",    0, NULL, 'h'},
    {"startup", 0, NULL, 's'},
    {NULL,      0, NULL,  0}
};

void print_usage_and_exit(int code)
{
    printf("LXQt Randr Configuration %s\n", STR_VERSION);
    puts("Usage: lxqt-config-randr [OPTION]...\n");
    puts("  -s,  --startup            Apply configuration from the saved settings");
    puts("  -h,  --help               Print this help");
    puts("  -v,  --version            Prints application version and exits");
    puts("\nHomepage: <https://github.com/zballina/lxqt-config-randr>");
    puts("Report bugs to <https://github.com/zballina/lxqt-config-randr>");
    exit(code);
}

void print_version_and_exit(int code=0)
{
    printf("%s\n", STR_VERSION);
    exit(code);
}

void parse_args(int argc, char* argv[], out bool& startup)
{
    int next_option;
    startup = false;
    do{
        next_option = getopt_long(argc, argv, short_options, long_options, NULL);
        switch(next_option)
        {
            case 'h':
                print_usage_and_exit(0);
            case 's':
                startup = true;
                break;
            case '?':
                print_usage_and_exit(1);
            case 'v':
                print_version_and_exit();
        }
    }
    while(next_option != -1);
}

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(lxqtconfigrandr);

    QApplication::setApplicationName("lxqt-config-randr");
#ifdef STR_VERSION
    QApplication::setApplicationVersion(QString("%1").arg(STR_VERSION));
#endif
    QApplication::setOrganizationDomain("lxqt");
    QSettings::setDefaultFormat(QSettings::NativeFormat);

    QApplication a(argc, argv);

    bool startup;
    parse_args(argc, argv, startup);

    if(startup)
    {
        QSettings config;
        QFile fileconfig(config.fileName());
        if(fileconfig.exists())
        {
            LoaderConfigLogin loader;
            loader.execute();
        }
        else
        {
            qDebug() << "File config not exist: " << config.fileName();
            qDebug() << "Not load config. Exit without change";
        }

        exit(0);
    }
    else
    {
        LXQtRandrConfig *w = new LXQtRandrConfig;
        w->show();
    }
    return a.exec();
}
