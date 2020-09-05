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
import QtQuick 2.0
import QtQuick.Controls 2.4

Menu {
    id: contextMenu

    Text {
        width: parent.width

        text: listView.model.get(currentEntry).title
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
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
        text: "Edit title/description"
        onTriggered: {
            addEditWindow.editEntry(listView.model.get(currentEntry));
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
        text: "Add/subtract time"
        onTriggered: {
            addTimeDialog.day = 0;
            addTimeDialog.open();
        }
    }

    Action {
        text: "Reset time (today)"
        onTriggered: {
            areyousurePopup.type = 3;
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
        text: "Reset time (total)"
        onTriggered: {
            areyousurePopup.type = 2;
            areyousurePopup.open();
        }
    }


    Action {
        text: "Delete task"
        onTriggered: {
            areyousurePopup.type = 1;
            areyousurePopup.open();
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
        implicitWidth: 210
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
            implicitWidth: 210
            implicitHeight: 30
            opacity: enabled ? 1 : 0.3
            color: menuItem.highlighted ? menuHighlight : bg
        }

    }

    background: Rectangle {
        implicitWidth: 210
        implicitHeight: 40
        color: blue5
        border.color: fg
        radius: 2
    }
}
