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

Menu {
    id: options

    property string csvname: ""

    Action {
        text: "Load file..."
        onTriggered: {
            //console.log("rowCount = "+listView.model.rowCount());
            if (listView.model.rowCount() > 0) {
                // Ask if user wants to save only if there are some tasks:
                areyousurePopup.type = 9;
                areyousurePopup.open();
            }
            else {
                loadFile.open();
            }
        }
    }


    Action {
        text: "Save file"
        onTriggered: {
            if (listView.model.SaveFileFull==="") {
                saveFile.mode = 0;
                saveFile.open();
            }
            else {
                listView.model.save_data(listView.model.SaveFileFull);
            }
        }
    }


     Action {
        text: "Save file as..."
        onTriggered: {
            saveFile.mode = 0;
            saveFile.open();
        }
     }


     Action {
        text: "Export to CSV"
        onTriggered: {
            csvname=listView.model.csvWriter();
            //console.log("csvname = ",csvname);
            notificationPopup.type = 2;
            notificationPopup.open();

        }
     }

     Action {
        text: "Close file..."
        onTriggered: {
            areyousurePopup.type = 6;
            areyousurePopup.open();
        }
     }

     MenuSeparator {
        contentItem: Rectangle {
            implicitWidth: 150
            implicitHeight: 1
            color: borderColor
        }
        background: Rectangle {
            implicitWidth: 150
            implicitHeight: 1
            color: bg
        }
     }


    Action {
       text: "Set/change password..."
       onTriggered: {
           pwEntry.mode  = 0;
           pwEntry.open();
       }
    }

    Action {
       text: "Remove encryption..."
       onTriggered: {
           if (listView.model.FileEncrypted===1) {
               areyousurePopup.type = 7;
               areyousurePopup.open();
           }
           else {
               notificationPopup.type = 1;
               notificationPopup.open();
           }
       }
    }

    MenuSeparator {
       contentItem: Rectangle {
           implicitWidth: 150
           implicitHeight: 1
           color: borderColor
       }
       background: Rectangle {
           implicitWidth: 150
           implicitHeight: 1
           color: bg
       }
    }

    Action {
        text: "Reset all times..."
        onTriggered: {
            areyousurePopup.type = 5;
            areyousurePopup.open();
        }
    }


    MenuSeparator {
        contentItem: Rectangle {
            implicitWidth: 150
            implicitHeight: 1
            color: borderColor
        }
        background: Rectangle {
            implicitWidth: 150
            implicitHeight: 1
            color: bg
        }
    }


    Action {
        text: "Settings..."
        onTriggered: {
            settingsWindow.opened()
            settingsWindow.show()
        }
    }


    Action {
        text: "Help"
        onTriggered: {
            listView.model.openHelp();
        }
    }


    Action {
        text: "About"
        onTriggered: {
            aboutWindow.opened()
            aboutWindow.show()
        }
    }


    MenuSeparator {
        contentItem: Rectangle {
            implicitWidth: 150
            implicitHeight: 1
            color: borderColor
        }
        background: Rectangle {
            implicitWidth: 150
            implicitHeight: 1
            color: bg
        }
    }


    Action {
        text: "Quit"
        onTriggered: {
            appWindow.close()
        }
    }



    modal: true
    dim: false
    topPadding: 1
    bottomPadding: 1
    leftPadding: 1
    rightPadding: 1

    delegate: MenuItem {
        id: menuItem
        implicitWidth: 180
        implicitHeight: 30

        contentItem: Text {
            text: menuItem.text
            font: menuItem.font
            opacity: enabled ? 1.0 : 0.3
            color: menuItem.highlighted ? "black" : fg
            horizontalAlignment: Text.AlignLeft
            verticalAlignment: Text.AlignVCenter
            elide: Text.ElideRight
        }
        background: Rectangle {
            implicitWidth: 200
            implicitHeight: 30
            opacity: enabled ? 1 : 0.3
            color: menuItem.highlighted ? menuHighlight : bg
        }

    }

    background: Rectangle {
        implicitWidth: 180
        implicitHeight: 40
        color: bg
        border.color: fg
        radius: 2
    }
}
