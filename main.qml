/*
Copyright (C) 2020 by Sebastian Kauertz.

This file is part of Timekeeper, a Qt-based time tracking app.

Timekeeper is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License
as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.

Timekeeper is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty
of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.

You should have received a copy of the GNU General Public License along with this program.
If not, see <https://www.gnu.org/licenses/>.

Icons by Fontawesome Free: https://fontawesome.com/license/free
*/
import QtQuick 2.11
import QtQuick.Controls 2.2
import QtQuick.Layouts 1.0
import TaskModel 1.0
import QtQuick.Window 2.11
import QtQuick.Dialogs 1.3
import "./src"


ApplicationWindow {
    id: appWindow
    visible: true
    width:  listView.model.windowWidth
    height: listView.model.windowHeight
    minimumWidth: 320
    minimumHeight: 300
    x: listView.model.windowPosX
    y: listView.model.windowPosY
    title: "Timekeeper"



    Component.onCompleted: {
        if (listView.model.defaultFile===1) {
            // A default file was found in the ini file - the model knows already if it is encrypted or not
            // Depending on file type, request a password or load directly:
            if (listView.model.PWneeded===1) {
                pwEntry.filename = listView.model.SaveFileFull;
                pwEntry.mode  = 1;   // Query password & load file
                pwEntry.open();
                timeToDisplay = listView.model.settingStartingView;
            }
            else {
                listView.model.load_data("",listView.model.SaveFileFull);
                timeToDisplay = listView.model.settingStartingView;
            }
        }
        else {
            // No default file was found - open file dialog
            //loadFile.open();
        }

        TrayManager.settingMinimizeToTray = listView.model.settingMinimizeToTray;
        TrayManager.updateIcon(1);   // Set alert icon
    }

    onClosing: {
        aboutToClose = 1;
        listView.model.savePosition(appWindow.x, appWindow.y, appWindow.width, appWindow.height);
        //console.log("Closing Application window...");

        if (readyToClose) {
            close.accepted = true;
        }
        else {
            if (listView.model.settingAutosave > 0) {
                // Autosave
                if (listView.model.rowCount() > 0) {
                    if ((listView.model.SaveFileFull.length===0)) {
                        // If there is no save file defined yet, we ask the user to specify one and then save:
                        close.accepted = false;
                        saveFile.mode = 0;
                        saveFile.open();
                    }
                    else {
                        // If there is a save file defined, save it and quit:
                        listView.model.save_data(listView.model.SaveFileFull);
                        readyToClose = 1;
                        close.accepted = true;
                    }
                }
            }
            else {
                // No Autosave - open a popup to ask the user if they want to save or not, but only if there is data to save:
                if (listView.model.rowCount() > 0) {
                    close.accepted = false;
                    areyousurePopup.type = 8;
                    areyousurePopup.open();
                }
                else {
                  close.accepted = true;
                }
            }
        }

    }

    property int currentEntry: -1           // Index containing the current entry in the list
    property int counting: 0                // 1 if any task is currently counting time, 0 otherwise
    property int rowActive: -1              // Index containing the entry that is currently counting time
    property int activeTaskID: -1           // Task ID that is currently counting time
    property int activeTaskSecToday: 0      // Debug
    property int timeToDisplay: 0           // Type of time to display; 0: Today, 1: This month, 2: This year
    property int lightMode: (listView.model.settingColorScheme===1)    // Light (1) or dark (0) color scheme

    property int aboutToClose: 0            // The main window wants to close
    property int readyToClose: 0            // 0 if we still have stuff to do, 1 if the main window may close

    property date   currentDate: new Date()
    property int    monthCount:  currentDate.getMonth() + 1    // Current month (1 = January)
    property int    year:        currentDate.getFullYear();    // Current year

    // Colors
    // Blues
    property string blue1: "#00205C"
    property string blue1mod: "#002C69"
    property string blue2: "#005587"
    property string blue3: "#0084AD"
    property string blue4: "#6099B0"
    property string blue5: "#B8C8D4"
    property string blue5mod: "#B8CADA"
    // Highlight colors
    property string highlightGreen:  "#81BC00"
    property string highlightYellow: "#E1E000"
    property string highlightOrange: "#FF5200"
    property string highlightRed:    "#EA002A"
    property string highlightPink:   "#E01783"
    property string highlightPurple: "#A91C8E"
    property string highlightBlue:   "#00ACC8"
    // Highlight colors, darker shade
    property string highlightGreenDark:  "#3B8000"
    property string highlightYellowDark: "#738006"
    property string highlightOrangeDark: "#802D00"
    property string highlightRedDark:    "#80091B"
    property string highlightPinkDark:   "#7E0A7A"
    property string highlightPurpleDark: "#4A0080"
    property string highlightBlueDark:   "#148069"
    // Highlight colors, lighter shade
    property string highlightOrangeLight: "#FF5900"
    // Greys
    property string grey1:  "#1A1A1A"
    property string grey2:  "#333333"
    property string grey3:  "#4D4D4D"
    property string grey4:  "#666666"
    property string grey5:  "#808080"
    property string grey6:  "#999999"
    property string grey7:  "#B3B3B3"

    property string bg:       lightMode===0 ? blue1 : blue5
    property string fg:       lightMode===0 ? blue5 : blue1
    property string txtColor: lightMode===0 ? blue5 : blue1
    property string button1:  lightMode===0 ? grey3 : grey6
    property string button2:  lightMode===0 ? blue5 : blue1
    property string menuHighlight:   lightMode===0 ? blue5 : blue4
    property string taskHighlight1:  lightMode===0 ? blue2 : blue4
    property string taskHighlight2:  lightMode===0 ? blue1mod : blue5mod
    property string taskActiveColor: lightMode===0 ? blue2 : blue4
    property string borderColor:     lightMode===0 ? blue5 : blue1

    color: bg


    // Instantiate all the secondary QML objects:
    AddEditWindow {
        id: addEditWindow
    }

    AddTimeDialog {
        id: addTimeDialog
    }

    ReportWindow {
        id: reportWindow
    }

    SettingsWindow {
        id: settingsWindow
    }

    AboutWindow {
        id: aboutWindow
    }

    AreYouSure {
        id: areyousurePopup
    }

    Notification {
        id: notificationPopup
    }

    EditContextMenu {
        id: editContextMenu
    }

    SortMenu {
        id: sortMenu
    }

    OptionsMenu {
        id: optionsMenu
    }

    PasswordEntry {
        id: pwEntry
    }

    PasswordIncorrect {
        id: pwIncorrect
        visible: listView.model.PWwrong
    }

    LoadFileDialog {
        id: loadFile
    }

    SaveFileDialog {
        id: saveFile
    }



//  Top Menu  ////////////////////////////////////////////////////////////
    Rectangle {
        id: topMenu
        width: parent.width
        height: 50
        color: bg
        //border.color: borderColor


        // Open Menu
        RoundButton {
            id: menu
            anchors.verticalCenter: parent.verticalCenter
            x: 20
            radius: 5
            height: 30
            width: 30
            flat: true
            icon.source: "icons/bars-solid.png"
            icon.color: menu.down ? button1 : button2

            ToolTip {
                parent: menu
                visible: parent.hovered
                delay: 1000

                contentItem: Text {
                    text: "Open Menu"
                    font.family: "Roboto"
                    color: txtColor
                }

                background: Rectangle {
                    color: button1
                    border.color: "transparent"
                    radius: 10
                }
            }

            background: Rectangle {
                height: parent.height
                width: parent.width
                radius: parent.radius
                color: menu.down ? button2 : button1
            }

            onClicked: optionsMenu.popup();

        }


        // Show Report window
        RoundButton {
            id: breakdown
            anchors.verticalCenter: parent.verticalCenter
            x: 70
            radius: 5
            height: 30
            width: 30
            flat: true
            icon.source: "icons/calendar-alt-regular.png"
            icon.color: breakdown.down ? button1 : button2

            ToolTip {
                parent: breakdown
                visible: parent.hovered
                delay: 1000

                contentItem: Text {
                    text: "Open Report"
                    font.family: "Roboto"
                    color: txtColor
                }

                background: Rectangle {
                    color: button1
                    border.color: "transparent"
                    radius: 10
                }
            }

            background: Rectangle {
                height: parent.height
                width: parent.width
                radius: parent.radius
                color: breakdown.down ? button2 : button1
            }

            onClicked: {
                if (reportWindow.active) {
                    reportWindow.close();
                }
                else {
                    reportWindow.opened();  // Custom handler
                    reportWindow.show();
                }
            }

        }


        // Sort by title, ascending
        RoundButton {
            id: sorting
            anchors.verticalCenter: parent.verticalCenter
            x: 120
            radius: 5
            height: 30
            width: 30
            flat: true
            icon.source: "icons/sort-alpha-down-solid.png"
            icon.color: sorting.down ? button1 : button2

            ToolTip {
                parent: sorting
                visible: parent.hovered
                delay: 1000

                contentItem: Text {
                    text: "Sort task list..."
                    font.family: "Roboto"
                    //font: control.font
                    color: txtColor
                }

                background: Rectangle {
                    color: button1
                    border.color: "transparent"
                    radius: 10
               }
            }

            background: Rectangle {
                height: parent.height
                width: parent.width
                radius: parent.radius
                color: sorting.down ? button2 : button1
            }

            onClicked: sortMenu.popup();
        }


        // Encryption indicator
        Button {
            id: lock
            anchors.verticalCenter: parent.verticalCenter
            x: 170
            y: 10
            height: 30
            width: 30
            flat: true
            icon.source: "icons/lock-solid.png"
            icon.color: listView.model.FileEncrypted===1 ?  fg : bg

            ToolTip {
                parent: lock
                visible: (parent.hovered && listView.model.FileEncrypted===1)
                delay: 1000

                contentItem: Text {
                    text: "Data file is encrypted"
                    font.family: "Roboto"
                    color: txtColor
                }

                background: Rectangle {
                    color: button1
                    border.color: "transparent"
                    radius: 10
               }
            }

            background: Rectangle {
                height: parent.height
                width: parent.width
                color: "transparent"
            }

        }


        // Add task
        RoundButton {
            id: add
            anchors.verticalCenter: parent.verticalCenter
            x: parent.width - 50
            radius: 5
            height: 30
            width: 30
            flat: true
            icon.source: "icons/plus-circle-solid.png"
            icon.width: 30
            icon.height: 30
            icon.color: add.down ? button1 : button2

            ToolTip {
                parent: add
                visible: parent.hovered
                delay: 1000

                contentItem: Text {
                    text: "Add task"
                    font.family: "Roboto"
                    color: txtColor
                }

                background: Rectangle {
                    color: button1
                    border.color: "transparent"
                    radius: 10
                }
            }

            background: Rectangle {
                height: parent.height
                width: parent.width
                radius: parent.radius
                color: add.down ? button2 : button1
            }

            onClicked: {
                currentEntry = -1
                addEditWindow.createEntry()
            }
        }


    }


//  Separator  ////////////////////////////////////////////////////////////
    Rectangle {
        id: separator1

        width: parent.width - 10
        height: 1
        anchors.top: topMenu.bottom
        x: 5
        color: borderColor
    }


//  Component defining the delegate for tasks  /////////////////////////////
        Component {
            id: taskDelegate
            Rectangle {
                id: rect01
                width: parent.width
                height: 50
                gradient: Gradient {
                    GradientStop { position: 0.0; color: model.taskActive===1 ? taskActiveColor : (ma.containsMouse ? taskHighlight1 : bg) }
                    GradientStop { position: 1.0; color: model.taskActive===1 ? taskActiveColor : (ma.containsMouse ? taskHighlight2 : bg) }
                }

                //border.color: ma.containsMouse ? borderColor : "transparent"
                radius: 5

                anchors {
                    horizontalCenter: parent.horizontalCenter;
                }

                property int checked


                // Task title
                Text {
                    id: text01
                    x: 15
                    text:  model.title     // Need to implement the roleNames() and data() method in the model to access this property
                    font.bold: true
                    font.family: "Roboto"
                    color: txtColor
                    height: parent.height
                    verticalAlignment: Text.AlignVCenter
                }

                // Task time
                Text {
                    id: timeText
                    x: parent.width - 135
                    height: parent.height
                    width: 55
                    verticalAlignment: Text.AlignVCenter
                    horizontalAlignment: Text.AlignRight
                    function timeString() {
                        if (timeToDisplay==0) {
                            return model.TodayString;
                        }
                        else if (timeToDisplay==1) {
                            return model.ThisMonthString;
                         }
                        else if (timeToDisplay==2) {
                            return model.ThisYearString;
                        }
                    }

                    text: timeString()
                    color: txtColor
                    font.bold: true
                }

                // MouseArea covers the task title and the time string
                MouseArea {
                   id: ma
                   anchors.left: parent.left
                   height: parent.height
                   width: parent.width - 80
                   hoverEnabled: true
                   acceptedButtons: Qt.LeftButton //| Qt.RightButton
                   onClicked: {
                       if (mouse.button === Qt.LeftButton)
                       {
                           if (index != rowActive) {
                               if (rowActive!=-1) listView.model.stopTimer(rowActive);
                               listView.model.startTimer(index);
                               rowActive    = index;
                               counting     = 1;
                               TrayManager.updateIcon(0);   // Set regular icon
                               activeTaskID = model.taskID;
                               activeTaskSecToday = model.elapsedSecToday;  // Debug
                           }
                           else {
                               rowActive  = -1;
                               activeTaskID = -1;
                               counting   = 0;
                               TrayManager.updateIcon(1);   // Set alert icon
                               listView.model.stopTimer(index);
                           }
                       }
                   }
                }

                // "Edit" button
                RoundButton {
                    id: edit
                    x: parent.width - 50
                    y: 10
                    radius: 5
                    height: 30
                    width: 30
                    flat: true
                    icon.source: "icons/edit-regular.png"
                    icon.color: edit.down ? button1 : button2

                    background: Rectangle {
                        height: parent.height
                        width: parent.width
                        radius: parent.radius
                        color: edit.down ? button2 : button1
                    }

                    onClicked: {
                        currentEntry = index;
                        editContextMenu.popup();
                    }
                }

            }

        }



//  The list of tasks  ////////////////////////////////////////////////////////////
        ListView {
            id: listView

            anchors.top: separator1.bottom
            width: parent.width
            height: parent.height - topMenu.height - bottomBar.height - separator1.height - separator2.height

            clip: true

            displaced: Transition {
                NumberAnimation { properties: "x,y"; easing.type: Easing.OutQuad }
            }

            focus: true
            boundsBehavior: Flickable.StopAtBounds

            delegate: taskDelegate

            model: TaskModel {
                id: taskModel
            }

            ScrollBar.vertical: ScrollBar { }
        }



//  Separator  ////////////////////////////////////////////////////////////
        Rectangle {
            id: separator2

            width: parent.width - 10
            height: 1
            anchors.top: listView.bottom
            x: 5
            color: borderColor
        }


//  Status bar  ////////////////////////////////////////////////////////////
        Rectangle {
            id: bottomBar
            width: parent.width
            anchors.bottom: parent.bottom
            height: 100
            color: bg

            function timeString() {
                if (timeToDisplay==0) {
                    return listView.model.TotalTimeToday;
                }
                else if (timeToDisplay==1) {
                    return listView.model.TotalTimeThisMonth;
                }
                else if (timeToDisplay==2) {
                    return listView.model.TotalTimeThisYear;
                }
            }
            function timeText() {
                if      (timeToDisplay == 0) return "Today:";
                else if (timeToDisplay == 1) return "This month:";
                else if (timeToDisplay == 2) return "This year:";
            }


            // Alert icon
            Button {
                id: alert
                anchors.verticalCenter: parent.verticalCenter
                x: Math.min(20, parent.width/20)
                height: 50
                width: 50
                visible: counting===0
                icon.source: "icons/exclamation-triangle-solid.png"
                icon.color: "orange"
                icon.width: 50
                icon.height: 50

                background: Rectangle {
                    color: bg
                }

                ToolTip {
                    parent: alert
                    visible: parent.hovered
                    delay: 750

                    contentItem: Text {
                        text: "No Task active!"
                        font.bold: true
                        font.family: "Roboto"
                        color: txtColor
                    }

                    background: Rectangle {
                        color: button1
                        border.color: "transparent"
                        radius: 10
                    }
                }

            }


            // Change time display type (Today / This Month / This Year)
            RoundButton {
                id: timeType
                anchors.verticalCenter: parent.verticalCenter
                x: parent.width - 240  //130
                radius: 5
                height: 30
                width: 90
                flat: true

                contentItem: Text {
                    font.bold: true
                    font.family: "Roboto"
                    font.pixelSize: 14
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: bottomBar.timeText()
                    color: timeType.down ? button1 : button2
                }

                ToolTip {
                    parent: timeType
                    visible: parent.hovered
                    delay: 750

                    contentItem: Text {
                        text: "Change time display"
                        font.family: "Roboto"
                        color: txtColor
                    }

                    background: Rectangle {
                        color: button1
                        border.color: "transparent"
                        radius: 10
                    }
                }

                background: Rectangle {
                    height: parent.height
                    width: parent.width
                    radius: parent.radius
                    color: timeType.down ? button2 : button1
                }

                onClicked: {
                    if (timeToDisplay < 2) {
                        timeToDisplay = timeToDisplay + 1;
                    }
                    else {
                        timeToDisplay = 0;
                    }
                }

            }


            Text {
                id: totalTime
                x: parent.width - 140
                width: 100
                anchors.verticalCenter: parent.verticalCenter
                horizontalAlignment: Text.AlignRight
                color: txtColor
                font.bold: true
                font.pixelSize: 20
                font.family: "Roboto"
                text: bottomBar.timeString()
            }

        }

}
