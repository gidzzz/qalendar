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
#include <QTranslator>
#include <QDebug>

#include "MainWindow.h"

int main(int argc, char  *argv[])
{
    QApplication::setOrganizationName("qalendar");
    QApplication::setApplicationName("qalendar");

    QApplication app(argc, argv);

    QString tsPath = PKGDATADIR "/translations/qalendar_";
    QString lang = qgetenv("LANG");
    qDebug() << "Detected language:" << lang;

    QTranslator translator;
    if (!translator.load(tsPath + lang))
        translator.load(tsPath + "en");
    app.installTranslator(&translator);

    QLocale::setDefault(lang);

    MainWindow window;

    window.show();

    return app.exec();
}
