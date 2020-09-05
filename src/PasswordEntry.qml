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
    id: pwEntry

    property string filename: ""
    property int    mode: 0         // 0: Query new password and update in model, 1: Query password and load file
    property int    visi: 0         // Visiblity of password

    width: 320
    height: 180
    x: parent.width / 2 - width / 2
    y: parent.height / 2 - height / 2

    bottomPadding: -1
    topPadding: -1
    leftPadding: 0
    rightPadding: 0

    focus: true
    modal: true
    closePolicy: Dialog.CloseOnEscape

    onOpened: {
        pwText.clear();
        pwText.focus = true;
        visi = 0;
    }

    onRejected: {
        pwText.clear();
    }

    header: Rectangle {
        height: 30
        width: parent.width
        color: grey3
        border.color: fg

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            font.bold: true
            color: blue5
            text: "Enter Password"
        }
    }

    contentItem: Rectangle {
        id: form
        property int leftEdge: 35
        property int topEdge: 10
        width: parent.width
        height: parent.height
        color: bg
        border.color: fg
        radius: 0

        Text {
            x: 0
            y: form.topEdge
            width: parent.width
            height: 45
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.WordWrap

            color: fg
            font.bold: true
            text: {
                   "Please enter password for file \"" + listView.model.SaveFile + "\":"
            }
        }

        Rectangle {
            id: input
            x: form.leftEdge
            y: form.topEdge + 55
            width: 220
            height: 25
            color: blue5
            border.color: fg
            border.width: 2

            TextInput {
                id: pwText
                width: parent.width
                height: parent.height
                focus: true
                clip: true
                activeFocusOnTab: true
                selectByMouse: true
                maximumLength: 32
                horizontalAlignment: TextInput.AlignHCenter
                verticalAlignment: TextInput.AlignVCenter
                text: ""
                echoMode: {
                    if (visi===0) TextInput.Password;
                    else          TextInput.Normal;
                }
                //passwordMaskDelay: 300

                onAccepted: {
                    if (pwEntry.mode===1) {
                        listView.model.load_data(pwText.text,pwEntry.filename);
                    }
                    else {
                        listView.model.set_password(pwText.text);
                    }
                    pwEntry.close();
                }
            }

            Button {
                id: eye
                x: 240
                y: 2
                height: 21
                width: 21
                icon.source: visi===0 ? "../icons/eye-regular.png" : "../icons/eye-slash-regular.png"
                icon.color: fg
                icon.width: 19
                icon.height: 19

                bottomPadding: 0
                topPadding: 0
                leftPadding: 0
                rightPadding: 0

                focusPolicy: Qt.NoFocus

                background: Rectangle {
                    width: parent.width
                    height: parent.height
                    radius: 2
                    color: bg
                    border.color: parent.focus ? "orange" : fg
                    border.width: 1
                }

                onClicked: {
                    if (visi===0) {
                        visi = 1;
                    }
                    else {
                        visi = 0;
                    }
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
                 if (mode===1) listView.model.removeAll();  // If user clicks Cancel while loading file, clear all references to the file to prevent overwriting it with empty data
                 pwEntry.close()
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
                text: "OK"
            }

            onClicked: {
                if (pwEntry.mode===1) {
                    listView.model.load_data(pwText.text,pwEntry.filename);
                }
                else {
                    listView.model.set_password(pwText.text);
                }
                pwEntry.close();
            }

        }

    }


}

