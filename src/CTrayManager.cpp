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
#include <QtWidgets/QApplication>
#include "CTrayManager.h"

CTrayManager::CTrayManager(QObject *parent) : QObject(parent)
{

//  Set up system tray icon:
    // QSystemTrayIcon may only be used AFTER QGuiApplication!

//    if (QSystemTrayIcon::isSystemTrayAvailable()) {
//        qInfo("System Tray available!");
//    }

    icon.addFile(":/icons/Timekeeper_SysTray.png");
    if (icon.isNull()) {
        qInfo("icon is Null!");
    }
    iconAlert.addFile(":/icons/Timekeeper_SysTray_Alert.png");
    if (iconAlert.isNull()) {
        qInfo("iconAlert is Null!");
    }


    trayIconMenu  = new QMenu();
    quitAction    = new QAction("&Quit", trayIconMenu);
    quitAction->setStatusTip("Quit Timekeeper");
    hideRestoreAction = new QAction("&Hide/Restore", trayIconMenu);
    hideRestoreAction->setStatusTip("Hide/Restore Timekeeper window");
    trayIconMenu->addAction(hideRestoreAction);
    trayIconMenu->addSeparator();
    trayIconMenu->addAction(quitAction);

//    hideAction    = new QAction("&Hide", trayIconMenu);
//    hideAction->setStatusTip("Hide Timekeeper window");
//    restoreAction = new QAction("&Show", trayIconMenu);
//    restoreAction->setStatusTip("Show Timekeeper window");
//    trayIconMenu->addAction(hideAction);
//    trayIconMenu->addAction(restoreAction);


    trayIcon = new QSystemTrayIcon();
    trayIcon->setContextMenu(trayIconMenu);
    trayIcon->setIcon(icon);
    trayIcon->setToolTip("Timekeeper Time Tracking");
    trayIcon->show();

//    QObject::connect(quitAction, &QAction::triggered, qApp, &QCoreApplication::quit);   // Works, but only calls ~CTaskModel directly - no clean-up


    appWindow = NULL;

    hidden = false;
}


CTrayManager::~CTrayManager()
{
    // We do the actual clean-up in CTrayManager::cleanup(), triggered by signal QApplication::aboutToQuit
}


void CTrayManager::setup(QApplication *app)
{
// Initialize window list after QML has been loaded, make connections
    int i;

    //qInfo("m_settingMinimizeToTray = %i",m_settingMinimizeToTray);

    wndList = app->topLevelWindows();
    //qInfo("No. of top-level windows: %i",wndList.length() );

    for (i=0; i<wndList.length(); i++) {
        //qInfo("Name of top-level window %i: %s",i,wndList.at(i)->title().toUtf8().data() );
        if (wndList.at(i)->title()=="Timekeeper") {
            appWindow = wndList.at(i);
        }
        // Build the list of windows:
        windowVisibility.append(wndList.at(i)->isVisible());
        //qInfo("Window %i visibility: %i",i,wndList.at(i)->isVisible());
    }

    // Hide / restore windows with dedicated functions, depending on their status at time of hiding:
    //QObject::connect(hideAction, &QAction::triggered, this, &CTrayManager::onHide);    // For separate buttons
    //QObject::connect(restoreAction, &QAction::triggered, this, &CTrayManager::onRestore);  // For separate buttons

    // Hide / restore windows with a single function:
    QObject::connect(hideRestoreAction, &QAction::triggered, this, &CTrayManager::onHideRestore);
    // When quitting, restore all previous windows and send close event to main window:
    QObject::connect(quitAction, &QAction::triggered, this, &CTrayManager::onQuit);
    // Minimize to tray:
    if (m_settingMinimizeToTray > 0) {
        QObject::connect(appWindow, &QWindow::windowStateChanged, this, &CTrayManager::onMinimize);
    }

    // Connect quit signal to dedicated function deleting the icon (if not properly deleted, it will keep hanging around in the system tray on Windows)
    QObject::connect(app, &QApplication::aboutToQuit, this, &CTrayManager::cleanup);

}


void CTrayManager::updateSettings()
{
//  Update any settings that were changed in the QML part

    // Minimize to tray:
    if (m_settingMinimizeToTray > 0) {
        QObject::connect(appWindow, &QWindow::windowStateChanged, this, &CTrayManager::onMinimize);
    }
    else {
        QObject::disconnect(appWindow, &QWindow::windowStateChanged, this, &CTrayManager::onMinimize);
    }

}


void CTrayManager::updateIcon(quint8 type)
{
//  Update the icon
//  type:  0=Regular icon, 1=Alert icon

    if (type==1) {
        trayIcon->setIcon(iconAlert);
        trayIcon->setToolTip("Timekeeper - No task active!");
        //trayIcon->showMessage("Timekeeper","No task active!",iconAlert, 5000);
    }
    else {
        trayIcon->setIcon(icon);
        trayIcon->setToolTip("Timekeeper Time Tracking");
    }

    emit iconChanged();

}


void CTrayManager::onHide()
{
//  Hide all visible windows
    int i;

    if (!hidden) {
        // Memorize which windows were visible at time of hiding:
        for (i=0; i<windowVisibility.length(); i++) {
            //qInfo("Window %i visibility: %i",i,wndList.at(i)->isVisible());
            windowVisibility.replace(i, wndList.at(i)->isVisible());
            if (wndList.at(i)->isVisible()) {
                wndList.at(i)->hide();
            }
        }
        hidden = true;
    }

}


void CTrayManager::onRestore()
{
//  Restore all previously visible windows
    int i;

    if (hidden) {
        // Restore only those windows which were visible at time of hiding:
        for (i=0; i<windowVisibility.length(); i++) {
            if (windowVisibility.at(i)==1) {
                wndList.at(i)->showNormal();
            }
        }
        hidden = false;
    }

}


void CTrayManager::onHideRestore()
{
//  Hide all visible windows / Restore all previously visible windows
    int i;

    if (!hidden) {
        // Memorize which windows were visible at time of hiding:
        for (i=0; i<windowVisibility.length(); i++) {
            //qInfo("Window %i visibility: %i",i,wndList.at(i)->isVisible());
            windowVisibility.replace(i, wndList.at(i)->isVisible());
            if (wndList.at(i)->isVisible()) {
                wndList.at(i)->hide();
            }
        }
        hidden = true;
    }
    else {
        // Restore only those windows which were visible at time of hiding:
        for (i=0; i<windowVisibility.length(); i++) {
            if (windowVisibility.at(i)==1) {
                wndList.at(i)->showNormal();
            }
        }
        hidden = false;
    }

}


void CTrayManager::onMinimize(Qt::WindowState windowState)
{
//  Minimize to tray (only connected to signal if appropriate setting is set)
    int i;

    if (windowState==Qt::WindowMinimized) {
        if (!hidden) {
            // Memorize which windows were visible at time of hiding:
            for (i=0; i<windowVisibility.length(); i++) {
                //qInfo("Window %i visibility: %i",i,wndList.at(i)->isVisible());
                windowVisibility.replace(i, wndList.at(i)->isVisible());
                if (wndList.at(i)->isVisible()) {
                    wndList.at(i)->hide();
                }
            }
            hidden = true;
        }
    }

}


void CTrayManager::onQuit()
{
//  Restore the main application windows if it was hidden, then send it a close event
    int i;

    if (hidden) {
        // Restore other windows:
        for (i=0; i<windowVisibility.length(); i++) {
            if (windowVisibility.at(i)==1) {
                // We want to restore the main window last to ensure it gets focus, so we skip it here:
                if (wndList.at(i)->title()!="Timekeeper") {
                    wndList.at(i)->showNormal();
                }
            }
        }
        hidden = false;
    }

    // Finally, restore the main window and send a close event:
    appWindow->showNormal();
    appWindow->raise();    // Raise window to top (showNormal does not do that if the window is already visible)
    appWindow->close();    // This triggers the onClosing handler in main.qml
}


void CTrayManager::cleanup()
{
// Make sure the system tray is removed (mainly required for Windows)

    //qInfo("CTrayManager::cleanup");
    if (trayIcon != nullptr) delete trayIcon;
    if (trayIconMenu != nullptr) delete trayIconMenu;
}

