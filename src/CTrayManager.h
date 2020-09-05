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
#ifndef CTRAYMANAGER_H
#define CTRAYMANAGER_H

#include <QtWidgets/QApplication>
#include <QSystemTrayIcon>
#include <QIcon>
#include <QMenu>
#include <QAction>
#include <QObject>
#include <QList>
#include <QWindow>
#include <QWindowList>

class CTrayManager : public QObject
{
    Q_OBJECT

public:
    CTrayManager(QObject *parent = nullptr);
    ~CTrayManager();

    void setup(QApplication *app = nullptr);

    Q_INVOKABLE void updateSettings();
    Q_INVOKABLE void updateIcon(quint8 type);

    Q_PROPERTY(qint16  settingMinimizeToTray       MEMBER m_settingMinimizeToTray        NOTIFY settingChanged)


signals:
    void settingChanged();
    void iconChanged();


public slots:
    void onHide();
    void onRestore();
    void onHideRestore();
    void onMinimize(Qt::WindowState windowState);
    void onQuit();
    void cleanup();


private:

    bool hidden;                    // Tracks if the application is currently hidden or not
    QList<bool> windowVisibility;   // Contains which windows were visible at time of hiding
    QSystemTrayIcon *trayIcon;
    QIcon icon;                     // Holds the regular SysTray icon
    QIcon iconAlert;                // Holds the SysTray icon in alert state
    QMenu *trayIconMenu;
    QAction *quitAction;
    QAction *hideAction;
    QAction *restoreAction;
    QAction *hideRestoreAction;
    QWindowList wndList;            // List containing all the top-level windows of the application
    QWindow *appWindow;

    qint16 m_settingMinimizeToTray;

};

#endif // CTRAYMANAGER_H
