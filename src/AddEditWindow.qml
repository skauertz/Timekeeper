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
import QtQuick.Layouts 1.0
import QtQuick.Window 2.11

Window {
    id: addEditWindow

    function createEntry() {
        form.title.clear();
        form.description.clear();

        addEditWindow.title = "Add Entry";
        addEditWindow.opened();
        addEditWindow.show();
    }

    function editEntry(entry) {
        form.title.text = entry.title;
        form.description.text = entry.description;

        addEditWindow.title = "Edit Entry";
        addEditWindow.opened();
        addEditWindow.show();
    }

    signal opened()

    onOpened: {
        title.focus = true;
    }

    width: 350
    height: 200
    minimumWidth: 350
    minimumHeight: 200
    maximumWidth: 350
    maximumHeight: 200
    x: appWindow.x < appWindow.screen.width - (appWindow.width + width) ? (appWindow.x + appWindow.width + 10) : (appWindow.x - width - 10)
    y: appWindow.y

    title: addEditWindow.title


    Rectangle {
        id: form
        property int leftEdge: 10
        property int topEdge: 30
        property alias title: title
        property alias description: description
        property string placeholderText: "<enter>"

        width: parent.width
        height: 160
        color: bg
        border.color: fg

        Label {
            id: label1
            x: form.leftEdge
            y: form.topEdge
            width: 90
            height: 40
            verticalAlignment: Qt.AlignVCenter
            text: "Title:"
            font.bold: true
            color: fg
        }

        TextField {
            id: title
            anchors.left: label1.right
            y: form.topEdge
            height: 40
            width: 230
            focus: true
            placeholderText: form.placeholderText
            maximumLength: 32
            background: Rectangle {
                implicitWidth: 230
                implicitHeight: 40
                color: blue5
                border.color: parent.focus ? "orange" : grey4
                border.width: parent.focus ? 2 : 2
            }

            onAccepted: {
                if (currentEntry === -1){
                        listView.model.append(form.title.text, form.description.text);
                }
                else {
                    listView.model.set(currentEntry, form.title.text, form.description.text)
                }
                addEditWindow.close();
            }
        }

        Label {
            id: label2
            x: form.leftEdge
            y: form.topEdge + label1.height
            width: 90
            height: 40
            verticalAlignment: Qt.AlignVCenter
            text: "Description:"
            font.bold: true
            color: fg
        }

        TextField {
            id: description
            anchors.left: label2.right
            y: form.topEdge + title.height
            height: 40
            width: 230
            placeholderText: form.placeholderText
            maximumLength: 128
            background: Rectangle {
                implicitWidth: 230
                implicitHeight: 40
                color: blue5
                border.color: parent.focus ? "orange" : grey4
                border.width: parent.focus ? 2 : 2
            }

            onAccepted: {
                if (currentEntry === -1){
                        listView.model.append(form.title.text, form.description.text);
                }
                else {
                    listView.model.set(currentEntry, form.title.text, form.description.text)
                }
                addEditWindow.close();
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
            x: 100
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
                text: "Cancel"
            }

            onClicked: {
                addEditWindow.close();
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
                text: "OK"
            }

            onClicked: {
                if (currentEntry === -1){
                        listView.model.append(form.title.text, form.description.text);
                }
                else {
                    listView.model.set(currentEntry, form.title.text, form.description.text)
                }
                addEditWindow.close();
            }
        }


    }


}
