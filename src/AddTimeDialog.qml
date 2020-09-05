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
import QtQuick 2.9
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.0

Dialog {
    id: addTimeDialog

    property int day: 0           // Date to be modified (relative days from today's date)
    property int addsubtract: 1   // 1: Add time, -1: Subtract time
    property int hours: 0
    property int minutes: 0
    property int seconds: 0

    onOpened: {
        // Start with adding time by default:
        addsubtract = 1;
        hours   = 0;
        minutes = 0;
        seconds = 0;
        //console.log("day = ",day);
        hoursText.focus = true;
    }

    width: 340
    height: 200
    x: parent.width / 2 - width / 2
    y: parent.height / 2 - height / 2

    focus: true
    modal: true

    bottomPadding: -1
    topPadding: -1
    leftPadding: 0
    rightPadding: 0

    header: Rectangle {
        height: 30
        width: parent.width
        border.color: fg
        color: grey3

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            font.bold: true
            color: blue5
            text: "Add / subtract time"
        }
    }



    contentItem: Rectangle {
        id: form
        property int leftEdge: 110
        property int topEdge: 60

        width: parent.width
        height: 150
        color: bg
        border.color: fg

        Text {
            x: 10
            y: 10
            width: 320
            height: 50
            horizontalAlignment: Text.AlignHCenter
            verticalAlignment: Text.AlignVCenter
            wrapMode: Text.WordWrap

            color: fg
            font.bold: true
            text: "Enter time to add to task \"" + listView.model.get(currentEntry).title + "\":"

        }

        // Top row: Legend
        Label {
            id: label1
            x: form.leftEdge
            y: form.topEdge
            width: 40
            height: 30
            verticalAlignment: Qt.AlignVCenter
            horizontalAlignment: Qt.AlignHCenter
            text: "HH"
            font.bold: true
            color: fg
        }
        Label {
            id: label2
            anchors.left: label1.right
            y: form.topEdge
            width: 10
            height: 30
            verticalAlignment: Qt.AlignVCenter
            horizontalAlignment: Qt.AlignHCenter
            text: ":"
            font.bold: true
            color: fg
        }
        Label {
            id: label3
            anchors.left: label2.right
            y: form.topEdge
            width: 40
            height: 30
            verticalAlignment: Qt.AlignVCenter
            horizontalAlignment: Qt.AlignHCenter
            text: "mm"
            font.bold: true
            color: fg
        }
        Label {
            id: label4
            anchors.left: label3.right
            y: form.topEdge
            width: 10
            height: 30
            verticalAlignment: Qt.AlignVCenter
            horizontalAlignment: Qt.AlignHCenter
            text: ":"
            font.bold: true
            color: fg
        }
        Label {
            id: label5
            anchors.left: label4.right
            y: form.topEdge
            width: 40
            height: 30
            verticalAlignment: Qt.AlignVCenter
            horizontalAlignment: Qt.AlignHCenter
            text: "ss"
            font.bold: true
            color: fg
        }

        // Bottom row: Entry
        Button {
            id: plusminus
            //anchors.right: hours.left
            x: form.leftEdge - 35
            y: form.topEdge + label1.height
            height: 25
            width: 25
            icon.source: addsubtract == 1 ? "../icons/plus-square-regular.png" : "../icons/minus-square-regular.png"
            icon.color: fg
            icon.width: 25
            icon.height: 25

            bottomPadding: 0
            topPadding: 0
            leftPadding: 0
            rightPadding: 0

            background: Rectangle {
                width: parent.width
                height: parent.height
                radius: 2
                color: button1
                border.color: parent.focus ? "orange" : button1
                border.width: parent.focus ? 2 : 1
            }

            onClicked: {
                if (addsubtract == 1) {
                    addsubtract = -1;
                }
                else {
                    addsubtract = 1;
                }
            }
        }
        Rectangle {
            id: frame1
            x: form.leftEdge
            y: form.topEdge + label1.height
            width: 40
            height: 25
            color: blue5
            border.color: fg
            border.width: 2
            TextInput {
                id: hoursText
                width: parent.width
                height: parent.height
                focus: true
                activeFocusOnTab: true
                selectByMouse: true
                maximumLength: 2
                horizontalAlignment: TextInput.AlignHCenter
                verticalAlignment: TextInput.AlignVCenter
                text: ""
                validator: IntValidator{bottom: 0; top: 24;}
                onAccepted: {
                    if (!hoursText.acceptableInput)   hours   = 0;
                    else                              hours   = addsubtract * parseInt(hoursText.text);
                    if (!minutesText.acceptableInput) minutes = 0;
                    else                              minutes = addsubtract * parseInt(minutesText.text);
                    if (!secondsText.acceptableInput) seconds = 0;
                    else                              seconds = addsubtract * parseInt(secondsText.text);

                    if (currentEntry != -1){
                        listView.model.add_time(currentEntry, day, hours, minutes, seconds)
                    }
                    hoursText.text = "";
                    minutesText.text = "";
                    secondsText.text = "";
                    addTimeDialog.close();
                }
            }
        }
        Label {
            id: label6
            anchors.left: frame1.right
            y: form.topEdge + label2.height
            width: 10
            height: 25
            verticalAlignment: Qt.AlignVCenter
            horizontalAlignment: Qt.AlignHCenter
            text: ":"
            font.bold: true
        }
        Rectangle {
            id: frame2
            anchors.left: label6.right
            y: form.topEdge + label3.height
            width: 40
            height: 25
            color: blue5
            border.color: fg
            border.width: 2
            TextInput {
                id: minutesText
                width: parent.width
                height: parent.height
                activeFocusOnTab: true
                selectByMouse: true
                maximumLength: 2
                horizontalAlignment: TextInput.AlignHCenter
                verticalAlignment: TextInput.AlignVCenter
                text: ""
                validator: IntValidator{bottom: 0; top: 60;}
                onAccepted: {
                    if (!hoursText.acceptableInput)   hours   = 0;
                    else                              hours   = addsubtract * parseInt(hoursText.text);
                    if (!minutesText.acceptableInput) minutes = 0;
                    else                              minutes = addsubtract * parseInt(minutesText.text);
                    if (!secondsText.acceptableInput) seconds = 0;
                    else                              seconds = addsubtract * parseInt(secondsText.text);

                    if (currentEntry != -1){
                        listView.model.add_time(currentEntry, day, hours, minutes, seconds)
                    }
                    hoursText.text = "";
                    minutesText.text = "";
                    secondsText.text = "";
                    addTimeDialog.close();
                }
            }
        }
        Label {
            id: label7
            anchors.left: frame2.right
            y: form.topEdge + label4.height
            width: 10
            height: 25
            verticalAlignment: Qt.AlignVCenter
            horizontalAlignment: Qt.AlignHCenter
            text: ":"
            font.bold: true
        }
        Rectangle {
            id: frame3
            anchors.left: label7.right
            y: form.topEdge + label5.height
            width: 40
            height: 25
            color: blue5
            border.color: fg
            border.width: 2
            TextInput {
                id: secondsText
                width: parent.width
                height: parent.height
                activeFocusOnTab: true
                selectByMouse: true
                maximumLength: 2
                horizontalAlignment: TextInput.AlignHCenter
                verticalAlignment: TextInput.AlignVCenter
                text: ""
                validator: IntValidator{bottom: 0; top: 60;}
                onAccepted: {
                    if (!hoursText.acceptableInput)   hours   = 0;
                    else                              hours   = addsubtract * parseInt(hoursText.text);
                    if (!minutesText.acceptableInput) minutes = 0;
                    else                              minutes = addsubtract * parseInt(minutesText.text);
                    if (!secondsText.acceptableInput) seconds = 0;
                    else                              seconds = addsubtract * parseInt(secondsText.text);

                    if (currentEntry != -1){
                        listView.model.add_time(currentEntry, day, hours, minutes, seconds)
                    }
                    hoursText.text = "";
                    minutesText.text = "";
                    secondsText.text = "";
                    addTimeDialog.close();
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
                    hoursText.text = "";
                    minutesText.text = "";
                    secondsText.text = "";
                    addTimeDialog.close();
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
                    if (!hoursText.acceptableInput)   hours   = 0;
                    else                              hours   = addsubtract * parseInt(hoursText.text);
                    if (!minutesText.acceptableInput) minutes = 0;
                    else                              minutes = addsubtract * parseInt(minutesText.text);
                    if (!secondsText.acceptableInput) seconds = 0;
                    else                              seconds = addsubtract * parseInt(secondsText.text);

                    if (currentEntry != -1){
                        listView.model.add_time(currentEntry, day, hours, minutes, seconds)
                    }
                    //console.log("OK clicked for entry: ",currentEntry, "  ",hours,":",minutes,":",seconds);
                    hoursText.text = "";
                    minutesText.text = "";
                    secondsText.text = "";
                    addTimeDialog.close();
                }
            }


    }

}
