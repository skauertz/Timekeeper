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

Dialog {
    id: notif
    width: 320
    height: 150
    x: parent.width / 2 - width / 2
    y: parent.height / 2 - height / 2

    property int type: 1    // 1: Removing password from unencrypted file, 2: CSV file written, 3: Error while saving

    bottomPadding: -1
    topPadding: -1
    leftPadding: 0
    rightPadding: 0

    modal: true

    header: Rectangle {
        height: 30
        width: parent.width
        color: "orange"
        border.color: fg

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            font.bold: true
            text: {
                switch(type) {
                case 1:
                    "Oops"
                    break;
                case 2:
                    if (optionsMenu.csvname != "") {
                        "Success"
                    }
                    else {
                        "Oops"
                    }
                    break;
                case 3:
                    "Oops"
                    break;
                }
            }
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
                    "The current file is not encrypted!"
                    break;
                case 2:
                    if (optionsMenu.csvname != "") {
                        "CSV written to \"" + optionsMenu.csvname + "\""
                    }
                    else {
                        "Error writing CSV file!"
                    }
                    break;
                case 3:
                    "Error saving file!"
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
            height: 20
            width: 60
            x: 120
            anchors.verticalCenter: parent.verticalCenter
            background: Rectangle {
                height: 30
                width: 60
                anchors.verticalCenter: parent.verticalCenter
                radius: 15
                border.color: parent.down ? blue5 : blue1
                border.width: 2
                color: parent.down ? blue1 : blue5
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                color: parent.down ? fg: "black"
                font.bold: true
                text: "OK"
            }

            onClicked: {
                notif.close();
            }

        }

    }

}
