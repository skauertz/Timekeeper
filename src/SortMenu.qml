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
    id: sort


    Action {
        text: "Sort by task name (A-Z)"
        onTriggered: {
            listView.model.sort(0,0);  // title, Ascending order
            rowActive = listView.model.row(activeTaskID);  // Update the row that is active
        }
    }


    Action {
        text: "Sort by task name (Z-A)"
        onTriggered: {
            listView.model.sort(0,1);  // title, Descending order
            rowActive = listView.model.row(activeTaskID);  // Update the row that is active
        }
    }

    MenuSeparator {
        contentItem: Rectangle {
            implicitWidth: 210
            implicitHeight: 1
            color: borderColor
        }
        background: Rectangle {
            implicitWidth: 210
            implicitHeight: 1
            color: bg
        }
    }

    Action {
        text: "Sort by total yearly time (incr.)"
        onTriggered: {
            listView.model.sort(3,0);  // elapsedSeconds, Ascending order
            rowActive = listView.model.row(activeTaskID);  // Update the row that is active
        }
    }

    Action {
        text: "Sort by total yearly time (decr.)"
        onTriggered: {
            listView.model.sort(3,1);  // elapsedSeconds, Descending order
            rowActive = listView.model.row(activeTaskID);  // Update the row that is active
        }
    }

    MenuSeparator {
        contentItem: Rectangle {
            implicitWidth: 210
            implicitHeight: 1
            color: borderColor
        }
        background: Rectangle {
            implicitWidth: 210
            implicitHeight: 1
            color: bg
        }
    }

        Action {
        text: "Sort by displayed time (incr.)"
        onTriggered: {
            switch(timeToDisplay) {
            case 0:
                listView.model.sort(1,0);  // daily elapsedSeconds, Ascending order
                break;
            case 1:
                listView.model.sort(2,0);  // monthly elapsedSeconds, Ascending order
                break;
            case 2:
                listView.model.sort(3,0);  // yearly elapsedSeconds, Ascending order
                break;
            }
            rowActive = listView.model.row(activeTaskID);  // Update the row that is active
        }
    }

    Action {
        text: "Sort by displayed time (decr.)"
        onTriggered: {
            switch(timeToDisplay) {
            case 0:
                listView.model.sort(1,1);  // daily elapsedSeconds, Descending order
                break;
            case 1:
                listView.model.sort(2,1);  // monthly elapsedSeconds, Descending order
                break;
            case 2:
                listView.model.sort(3,1);  // yearly elapsedSeconds, Descending order
                break;
            }
            rowActive = listView.model.row(activeTaskID);  // Update the row that is active
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
        color: bg
        border.color: fg
        radius: 2
    }
}
