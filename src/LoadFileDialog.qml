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
    id: loadFile

    property string name: ""

    folder: "."
    selectExisting: true
    selectMultiple: false
    nameFilters: ["All files (*)", "Timekeeper data file (*.tkp)"]

    title: "Please select file to load"
    onAccepted: {
        var s
        var path = loadFile.fileUrl.toString()
        if (path.startsWith("file:///")) {
            var k = path.charAt(9) === ':' ? 8 : 7
            s = path.substring(k)
        } else {
            s = path
        }
        loadFile.name = s;
        listView.model.removeAll();  // Clear old file
        // Properly reset main window:
        rowActive    = -1;
        activeTaskID = -1;
        counting     = 0;
        TrayManager.updateIcon(1);

        listView.model.checkFileType(s);  // Find out if the new file is encrypted or unencrypted

        // Depending on file type, request a password or load directly:
        if (listView.model.PWneeded===1) {
            pwEntry.filename = s;
            pwEntry.mode  = 1;   // Query password & load file
            pwEntry.open();
            // Note: Update all properties that are depending on saved settings in the Password Dialog, not here!
            // This dialog is closed after pwEntry.open(), while pwEntry can stay open for an indefinite amount of time before actually loading the file
        }
        else {
            listView.model.load_data("",loadFile.name);
            timeToDisplay = listView.model.settingStartingView;
        }

    }

    onRejected: {

    }

}
