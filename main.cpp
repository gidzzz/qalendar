/**************************************************************************
    Qalendar
    Copyright (C) 2013 Grzegorz Gidel

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
**************************************************************************/

#include <QApplication>
#include <QDBusConnection>
#include <QDBusConnectionInterface>
#include <QTranslator>
#include <QDebug>

#include <CMulticalendar.h>

#include "MainWindow.h"

int main(int argc, char  *argv[])
{
    QApplication::setOrganizationName("qalendar");
    QApplication::setApplicationName("qalendar");

    // This name is used when emitting dbChange on D-Bus
    CMulticalendar::MCInstance()->setApplicationName("QALENDAR");

    QApplication app(argc, argv);

    const bool runInBackground = argc > 1 && QString(argv[1]) == "--background";

    // Try to find an existing instance of the application: start by checking
    // if PID of the appropriate D-Bus service can be obtained. The simplified
    // override ckeck can be performed because this option should be mutually
    // exclusive with --background and currently there are no other switches.
    if (!(argc > 1 && QString(argv[1]) == "--allow-multiple-instances"))
    if (uint pid = QDBusConnection::sessionBus().interface()->servicePid(DBUS_SERVICE)) {
        // This file contains the command used to start the service
        QFile cmdline("/proc/" + QString::number(pid) + "/cmdline");

        // The command is considered a match if it contains the name of this application
        if (cmdline.open(QIODevice::ReadOnly)
        &&  cmdline.readLine().contains("qalendar"))
        {
            if (runInBackground) {
                qDebug() << "Already running as a service";
            } else {
                qDebug() << "Activating an earlier instance";

                QDBusConnection::sessionBus().send(QDBusMessage::createMethodCall(DBUS_SERVICE,
                                                                                  DBUS_PATH,
                                                                                  DBUS_INTERFACE,
                                                                                  "top_application"));
            }
            return 0;
        }
    }

    QString tsPath = PKGDATADIR "/translations/qalendar_";
    QString lang = qgetenv("LANG");
    qDebug() << "Detected language:" << lang;

    QTranslator translator;
    if (!translator.load(tsPath + lang))
        translator.load(tsPath + "en");
    app.installTranslator(&translator);

    QLocale::setDefault(lang);

    MainWindow window(runInBackground);

    return app.exec();
}
