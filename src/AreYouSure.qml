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
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.0

// This item provides several confirmation dialogs. The type of dialog can be selected via the property "type":
// 1: Delete task
// 2: Reset total time of task
// 3: Reset today's time of task
// 4: Reset current day's time of task (from Report window)
// 5: Reset all times
// 6: Close file & reset tasks
// 7: Remove encryption
// 8: Quit without saving

Dialog {
    id: areyousure

    property int type: 1    // 1: Delete task, 2: Reset total time of task, 3: Reset today's time of task, 4: Reset current day's time of task (from Report), 5: Reset all times, 6: Close file & reset tasks, 7: Remove encryption, 8: Quit without saving. 9: Load without saving
    property int day:  0    // 0=today

    width: 320
    height: 150
    x: parent.width / 2 - width / 2
    y: parent.height / 2 - height / 2

    bottomPadding: -1
    topPadding: -1
    leftPadding: 0
    rightPadding: 0

    focus: true
    modal: true

    onOpened: {
        buttonNo.focus = false;
        buttonYes.focus = false;
        //console.log("type = ",type);
    }

    header: Rectangle {
        height: 30
        width: parent.width
        color: "orange"
        border.color: fg

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            font.bold: true
            text: "Are you sure?"
        }
    }

    contentItem: Rectangle {
        color: bg
        border.color: fg
        radius: 0

        Text {
            width: parent.width
            height: parent.height
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            leftPadding: 5
            rightPadding: 5
            wrapMode: Text.WordWrap

            color: fg
            font.bold: true
            text: {
                switch(type) {
                case 1:
                    "Completely delete task \"" + listView.model.get(currentEntry).title + "\"?"
                    break;
                case 2:
                    "Reset total time of task \"" + listView.model.get(currentEntry).title + "\"?"
                    break;
                case 3:
                    "Reset today's time of task \"" + listView.model.get(currentEntry).title + "\"?"
                    break;
                case 4:
                    "Reset this day's time of task \"" + listView.model.get(currentEntry).title + "\"?"
                    break;
                case 5:
                    "Reset all time entries of all tasks?\n\nThis will not remove tasks from the list."
                    break;
                case 6:
                    "Save current file before closing?"
                    break;
                case 7:
                    "Remove the password and save the current data file in unencrypted format?"
                    break;
                case 8:
                    "Save data before quitting?"
                    break;
                case 9:
                    "Save current file before loading?"
                    break;
                }
            }

        }
    }


    footer: Rectangle {
        height: 40
        width: parent.width
        color: bg
        border.color: fg

        Button {
            id: buttonCancel
            height: 20
            width: 60
            x: 50
            anchors.verticalCenter: parent.verticalCenter
            background: Rectangle {
                height: 30
                width: 60
                anchors.verticalCenter: parent.verticalCenter
                radius: 15
                border.color: parent.down ? blue5 : (parent.focus ? "orange" : blue1)
                border.width: 2
                color: parent.down ? blue1 : blue5
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                color: parent.down ? blue5 : "black"
                font.bold: true
                text: "Cancel"
            }

             onClicked: {
                 switch(type) {
                 case 1:
                     break;
                 case 2:
                     break;
                 case 3:
                     break;
                 case 4:
                     break;
                 case 5:
                     break;
                 case 6:
                     break;
                 case 7:
                     break;
                 case 8:
                     break;
                 case 9:
                     break;
                 }
                 areyousure.close()
             }

        }

        Button {
            id: buttonNo
            height: 20
            width: 60
            x: 130
            anchors.verticalCenter: parent.verticalCenter
            background: Rectangle {
                height: 30
                width: 60
                anchors.verticalCenter: parent.verticalCenter
                radius: 15
                border.color: parent.down ? blue5 : (parent.focus ? "orange" : blue1)
                border.width: 2
                color: parent.down ? blue1 : blue5
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                color: parent.down ? blue5 : "black"
                font.bold: true
                text: "No"
            }

             onClicked: {
                 switch(type) {
                 case 1:
                     break;
                 case 2:
                     break;
                 case 3:
                     break;
                 case 4:
                     break;
                 case 5:
                     break;
                 case 6:
                     listView.model.removeAll();
                     // Properly reset main window:
                     rowActive    = -1;
                     activeTaskID = -1;
                     counting     = 0;
                     TrayManager.updateIcon(1);
                     break;
                 case 7:
                     break;
                 case 8:
                     appWindow.readyToClose = 1;
                     appWindow.close();
                     break;
                 case 9:
//                     listView.model.removeAll();   // Matter of preference: Do we want to clear the current file already after clicking "No", or leave it in case the user cancels the load dialog?
                     loadFile.open();
                     break;
                 }
                 areyousure.close()
             }

        }

        Button {
            id: buttonYes
            height: 20
            width: 60
            x: 210
            anchors.verticalCenter: parent.verticalCenter
            background: Rectangle {
                height: 30
                width: 60
                anchors.verticalCenter: parent.verticalCenter
                radius: 15
                border.color: parent.down ? blue5 : (parent.focus ? "orange" : blue1)
                border.width: 2
                color: parent.down ? blue1 : blue5
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                color: parent.down ? blue5 : "black"
                font.bold: true
                text: "Yes"
            }

            onClicked: {
                switch(type) {
                case 1:
                    if (currentEntry === rowActive) {
                        // If we try to remove the running task, properly stop it first:
                        TrayManager.updateIcon(1);   // Set alert icon
                        listView.model.stopTimer(currentEntry);
                        rowActive  = -1;
                        counting   = 0;
                    }
                    listView.model.remove(currentEntry);
                    if (counting) {
                        // If there was a task running while we were deleting another one, update the active row:
                        rowActive = listView.model.row(activeTaskID);
                        //console.log("rowActive = ",rowActive);
                    }
                    break;
                case 2:
                    listView.model.resetTotal(currentEntry);
                    break;
                case 3:
                    listView.model.resetToday(currentEntry);
                    break;
                case 4:
                    listView.model.add_time(currentEntry, day, -24, 0, 0); // Using add_time with max. possible amount to reset daily time
                    break;
                case 5:
                    listView.model.resetAll();
                    break;
                case 6:
                    if ((listView.model.SaveFileFull.length===0)) {
                        // If there is no save file defined yet, we ask the user to specify one, then clear the list:
                        saveFile.mode = 1;
                        saveFile.open();
                    }
                    else {
                        // If there is a save file defined, save it and then clear list:
                        listView.model.save_data(listView.model.SaveFileFull);
                        listView.model.removeAll();
                        // Properly reset main window:
                        rowActive    = -1;
                        activeTaskID = -1;
                        counting     = 0;
                        TrayManager.updateIcon(1);
                    }
                    break;
                case 7:
                    listView.model.removeEncryption();
                    break;
                case 8:
                    if ((listView.model.SaveFileFull.length===0)) {
                        // If there is no save file defined yet, we ask the user to specify one, then quit:
                        saveFile.mode = 0;
                        saveFile.open();
                    }
                    else {
                        // If there is a save file defined, save it and quit:
                        listView.model.save_data(listView.model.SaveFileFull);
                        appWindow.readyToClose = 1;
                        appWindow.close();
                    }
                    break;
                case 9:
                    if ((listView.model.SaveFileFull.length===0)) {
                        // If there is no save file defined yet, we ask the user to specify one, then open the load dialog:
                        saveFile.mode = 2;
                        saveFile.open();
                    }
                    else {
                        // If there is a save file defined, save it and then open the load dialog:
                        listView.model.save_data(listView.model.SaveFileFull);
                        loadFile.open();
                    }
                    break;
                }
                areyousure.close();
            }

        }

    }


}
