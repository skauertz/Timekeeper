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
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import QtQuick.Window 2.11


Window {
    id: aboutWindow

    width: 450
    height: 460
    minimumWidth: 450
    minimumHeight: 460
    x: appWindow.x < appWindow.screen.width - (appWindow.width + width) ? (appWindow.x + appWindow.width + 10) : (appWindow.x - width - 10)
    y: appWindow.y
    title: "About Timekeeper"

    color: bg

    signal opened()

    onOpened: {
        //console.log("About opened");
    }

    property int view: 1     // 1: About, 2: License, 3: Credits


    Rectangle {
        id: form

        height: aboutWindow.height - 40
        width: parent.width
        color: bg
        border.color: fg
        //border.width: 3


        Text {
            x: 0
            y: 30
            height: 40
            width: parent.width
            text: "Timekeeper"
            font.family: "Roboto"
            font.bold: true
            font.pixelSize: 26
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            color: fg
        }

        Text {
            x: 0
            y: 60
            height: 40
            width: parent.width
            text: "A simple time-tracking app"
            font.family: "Roboto"
            font.bold: true
            font.pixelSize: 14
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            color: fg
        }

        Text {
            x: 0
            y: 110
            height: 40
            width: parent.width
            text: "Version 1.0"
            font.family: "Roboto"
            font.bold: true
            font.pixelSize: 14
            horizontalAlignment: Qt.AlignHCenter
            verticalAlignment: Qt.AlignVCenter
            color: fg
        }


////////////////////////////////////////////////////////////////////////////
        StackView {
            id: stack
            x: 0
            y: 180
            width: 430
            height: 240
            initialItem: aboutView
        }


        Item {
            id: aboutView
            visible: stack.depth === 1
            height: stack.height
            width: stack.width

            Text {
                x: 10
                y: 100
                height: 20
                width: parent.width
                text: {
                    if ((listView.model.SaveFileFull.length===0)) {
                        "Current datafile location:\n-"
                    }
                    else {
                        "Current datafile location:\n" + listView.model.SaveFileFull
                    }
                }
                font.family: "Roboto"
                font.bold: false
                font.pixelSize: 12
                horizontalAlignment: Qt.AlignLeft
                wrapMode: Text.WordWrap
                color: fg
            }

            Text {
                x: 10
                y: 170
                height: 20
                width: parent.width
                text: "Logfile location:\n" + listView.model.LogFileFull
                font.family: "Roboto"
                font.bold: false
                font.pixelSize: 12
                horizontalAlignment: Qt.AlignLeft
                wrapMode: Text.WordWrap
                color: fg
            }

        }


        Item {
            id: licenseView
            visible: stack.depth == 2
            height: stack.height
            width: stack.width

            Text {
                x: 10
                y: 0
                height: 240
                width: 430
                textFormat: Text.StyledText
                text: "Copyright (C) 2020 by Sebastian Kauertz.<br><br>This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation, either version 3 of the License, or (at your option) any later version.<br><br>This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for more details.<br><br>You should have received a copy of the GNU General Public License along with this program. If not, see <a href=\"https://www.gnu.org/licenses\">https://www.gnu.org/licenses</a>.<br>"
                font.family: "Roboto"
                font.bold: false
                font.pixelSize: 12
                linkColor: fg
                horizontalAlignment: Qt.AlignLeft
                verticalAlignment: Qt.AlignBottom
                wrapMode: Text.WordWrap
                color: fg
                //onLinkActivated: console.log(link + " link activated")
                onLinkActivated: Qt.openUrlExternally("https://www.gnu.org/licenses")

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton // Do not react to clicks - this is managed by onLinkActivated
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }

        }


        Item {
            id: creditsView
            visible: stack.depth === 3
            height: stack.height
            width: stack.width

            Text {
                x: 10
                y: 0
                height: 240
                width: 430
                textFormat: Text.StyledText
                text: "Development by Sebastian Kauertz.<br><br>Source code available at <a href=\"https://github.com/skauertz/Timekeeper\">https://github.com/skauertz/Timekeeper</a> <br><br>Timekeeper is using <a href=\"https://qt.io\">Qt</a> licensed under LGPL version 3.<br><br>Icons by <a href=\"https://fontawesome.com/license/free\">Fontawesome</a>.<br><br>Cryptography routines from <a href=\"https://rweather.github.io/arduinolibs/index.html\">Arduino Cryptography Library</a>.<br>"
                font.family: "Roboto"
                font.bold: false
                font.pixelSize: 12
                linkColor: fg
                horizontalAlignment: Qt.AlignLeft
                verticalAlignment: Qt.AlignBottom
                wrapMode: Text.WordWrap
                color: fg
                onLinkActivated: Qt.openUrlExternally(link)

                MouseArea {
                    anchors.fill: parent
                    acceptedButtons: Qt.NoButton // Do not react to clicks - this is managed by onLinkActivated
                    cursorShape: parent.hoveredLink ? Qt.PointingHandCursor : Qt.ArrowCursor
                }
            }

        }


    }



    Rectangle {
        height: 40
        width: parent.width
        color: bg
        border.color: fg
        anchors.top: form.bottom

        Button {
            height: 20
            width: 60
            x: 150
            anchors.verticalCenter: parent.verticalCenter
            background: Rectangle {
                height: 30
                width: 60
                radius: 15
                anchors.verticalCenter: parent.verticalCenter
                border.color: parent.down ? blue5 : (parent.focus ? "orange" : blue1)
                border.width: 2
                color: parent.down ? blue1 : blue5
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                color: parent.down ? blue5 : "black"
                font.bold: true
                text: "Close"
            }

            onClicked: {
                aboutWindow.close();
            }
        }

        Button {
            height: 20
            width: 60
            x: 240
            anchors.verticalCenter: parent.verticalCenter
            background: Rectangle {
                height: 30
                width: 60
                radius: 15
                anchors.verticalCenter: parent.verticalCenter
                border.color: parent.down ? blue5 : (parent.focus ? "orange" : blue1)
                border.width: 2
                color: parent.down ? blue1 : blue5
            }
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                color: parent.down ? blue5 : "black"
                font.bold: true
                text: view === 1 ? "License" : (view === 2 ? "Credits" : "About")
            }

            onClicked: {
                if (view===1) {
                    if (!stack.busy) {
                        stack.push(licenseView);
                        view = 2;
                    }
                }
                else if (view===2) {
                    if (!stack.busy) {
                        stack.push(creditsView);
                        view = 3;
                    }
                }
                else if (view===3) {
                    if (!stack.busy) {
                        stack.pop();
                        stack.pop();
                        view = 1;
                    }
                }
            }
        }



    }

}
