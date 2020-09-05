/*
Copyright (C) 2020 by Sebastian Kauertz.

This file is part of Timekeeper, a Qt-based time tracking app.

Timekeeper is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

Timekeeper is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see <https://www.gnu.org/licenses/>.
*/
#include <QGuiApplication>
#include <QtWidgets/QApplication>
#include <QQmlApplicationEngine>
#include <QQmlEngine>
#include <QQmlContext>
#include <QInputDialog>
#include <QSystemTrayIcon>
#include <QIcon>
#include <QMenu>
#include <QObject>
#include <QList>
#include <QWindow>

#include "src/CTaskModel.h"
#include "src/CTrayManager.h"


int main(int argc, char *argv[])
{
    CTrayManager *trayManager;


    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

//    QGuiApplication app(argc, argv);   // If using QtCharts, replace QGuiApplication with QApplication
    // Use QApplication to enable use of QFileDialog instead of DefaultFileDialog.qml on Linux:
    QApplication app(argc, argv);

    QQmlApplicationEngine engine;

    qmlRegisterType<CTaskModel>("TaskModel", 1, 0, "TaskModel");

    //qApp->aboutQt();

    // Instantiate the tray manager:
    trayManager = new CTrayManager();

    // Register the tray manager with the root context to access its properties:
    engine.rootContext()->setContextProperty("TrayManager", trayManager);

    // Load the main window:
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));
    if (engine.rootObjects().isEmpty())
        return -1;

    // After the QML is loaded, finish initialization of tray manager:
    trayManager->setup(&app);

    return app.exec();

}
