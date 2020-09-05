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
    id: pwIncorrect

    width: 320
    height: 120
    x: parent.width / 2 - width / 2
    y: parent.height / 2 - height / 2 - 50

    bottomPadding: -1
    topPadding: -1
    leftPadding: 0
    rightPadding: 0

    focus: true
    modal: true
    closePolicy: Dialog.CloseOnEscape

    onRejected: {

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
            text: "Error"
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
            text: "Password incorrect!"
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
            x: 80
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
                 if (pwEntry.mode===1) listView.model.removeAll();  // If user clicks Cancel while loading file, clear all references to the file to prevent overwriting it with empty data
                 pwIncorrect.close()
             }

        }


        Button {
            height: 20
            width: 60
            x: 180
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
                text: "Retry"
            }

            onClicked: {
                pwEntry.mode  = 1;   // Query password & load file
                pwEntry.open();
                pwIncorrect.close();
            }

        }

    }


}
