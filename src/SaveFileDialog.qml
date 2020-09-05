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
import QtQuick 2.9
import QtQuick.Dialogs 1.3

FileDialog {
    id: saveFile

    property string name: ""
    property int mode: 0     // 0: Simple save, 1: Clear list after save, 2: Open load dialog after save

    folder: "."
    selectExisting: false
    selectMultiple: false
    nameFilters: ["All files (*)", "Timekeeper data file (*.tkp)"]

    title: "Please select file to save to"
    onAccepted: {
        var s
        var path = saveFile.fileUrl.toString()
        if (path.startsWith("file:///")) {
            var k = path.charAt(9) === ':' ? 8 : 7
            s = path.substring(k)
        } else {
            s = path
        }
        saveFile.name = s;

        var ret
        ret = listView.model.save_data(s);

        if (ret !== 0) {
            // Save again with an empty filename to reset filename in model:
            ret = listView.model.save_data("");
            // Notify the user:
            notificationPopup.type = 3;
            notificationPopup.open();
        }
        else {
            if (mode===1) {
                listView.model.removeAll();
                // Properly reset main window:
                rowActive    = -1;
                activeTaskID = -1;
                counting     = 0;
                TrayManager.updateIcon(1);
            }
            if (mode===2) {
                loadFile.open();
            }
            if (appWindow.aboutToClose) {
                appWindow.readyToClose = 1;
                appWindow.close();
            }
        }

    }

    onRejected: {
        saveFile.name = "";

        if (mode===1) {
            listView.model.removeAll();
            // Properly reset main window:
            rowActive    = -1;
            activeTaskID = -1;
            counting     = 0;
            TrayManager.updateIcon(1);
        }
        if (mode===2) {
            loadFile.open();
        }
        if (appWindow.aboutToClose) {
            appWindow.readyToClose = 1;
            appWindow.close();
        }
    }
}
