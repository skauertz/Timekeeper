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
    // This is a child of reportWindow, not the main window
    id: reallocate

    width: 180
    height: 220
    x: parent.width - width - 10
    y: bar.height + menu.height + 50

    property int reallocateAll: 0     // Reallocate for all days (1) or only current day (0)

    bottomPadding: -1
    topPadding: -1
    leftPadding: 0
    rightPadding: 0

    modal: false
    closePolicy: Popup.NoAutoClose

    header: Rectangle {
        height: 30
        width: parent.width
        border.color: fg
        color: blue5

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            font.bold: true
            text: "Reallocate time"
            color: "black"
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
            wrapMode: Text.WordWrap

            color: fg
            font.bold: true
            font.pixelSize: 12
            text: {
                if (reallocateAll) {"Reallocating all times of task \"" + listView.model.get(currentEntry).title + "\"\n\n Please mark tasks to allocate to!"}
                else {"Reallocating daily time of task \"" + listView.model.get(currentEntry).title + "\"\n\n Please mark tasks to allocate to!"}
            }

        }
    }


    footer: Rectangle {
        height: 40
        width: parent.width
        color: bg
        border.color: fg

            Button {
                height: 30
                width: 60
                x: 15
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
                    color: parent.down ? blue5 : "black"
                    font.bold: true
                    text: "Cancel"
                }
                 DialogButtonBox.buttonRole: DialogButtonBox.RejectRole

                 onClicked: {
                     listDaily.model.resetAllocate();
                     reallocating = false;
                     reallocate.close();
                 }

            }

            Button {
                height: 30
                width: 60
                x: 105
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
                    color: parent.down ? blue5 : "black"
                    font.bold: true
                    text: "OK"
                }
                DialogButtonBox.buttonRole: DialogButtonBox.AcceptRole

                onClicked: {
                    if (reallocateAll) {listDaily.model.reallocateAll(sourceID, dateCount, listView.model.settingWeightedReallocation);}
                    else               {listDaily.model.reallocate(sourceID, dateCount, listView.model.settingWeightedReallocation);}
                    reallocating = false;
                    sourceID     = -1;
                    listDaily.model.resetAllocate();
                    reallocate.close();
                }

            }

    }

}
