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
    id: settingsWindow

    // Variables for old settings (for Cancel functionality):
    property int oldOptionLastSelectedDate: optionLastSelected.checkState
    property int oldOptionWeightedReallocation: optionWeighted.checkState
    property int oldOptionAutosave: optionAutosave.checkState
    property int oldOptionMinimizeToTray: optionMinimizeToTray.checkState
    property int oldOptionStartingView: optionStartingView.currentIndex
    property int oldOptionColorScheme: optionColorScheme.currentIndex

    width: 400
    height: 400
    minimumHeight: height
    minimumWidth: width
    maximumHeight: height
    maximumWidth: width
    x: appWindow.x < appWindow.screen.width - (appWindow.width + width) ? (appWindow.x + appWindow.width + 10) : (appWindow.x - width - 10)
    y: appWindow.y
    title: "Timekeeper Settings"

    color: bg

    signal opened()

    onOpened: {
        //console.log("Settings opened");
        optionLastSelected.checkState   = listView.model.settingLastSelectedDate;
        optionWeighted.checkState       = listView.model.settingWeightedReallocation;
        optionAutosave.checkState       = listView.model.settingAutosave;
        optionMinimizeToTray.checkState = listView.model.settingMinimizeToTray;
        optionStartingView.currentIndex = listView.model.settingStartingView;
        optionColorScheme.currentIndex  = listView.model.settingColorScheme;
        oldOptionLastSelectedDate       = optionLastSelected.checkState;
        oldOptionWeightedReallocation   = optionWeighted.checkState;
        oldOptionAutosave               = optionAutosave.checkState;
        oldOptionMinimizeToTray         = optionMinimizeToTray.checkState;
        oldOptionStartingView           = optionStartingView.currentIndex;
        oldOptionColorScheme            = optionColorScheme.currentIndex;
    }


    Rectangle {
        id: form
        property int leftEdge: 20
        property int topEdge: 30

        height: 360
        width: parent.width
        color: bg
        border.color: fg

        // Keep last selected date in Report window
        CheckBox {
            id: optionLastSelected
            x: form.leftEdge
            y: form.topEdge
            width: 40
            height: 30
            checked: false
            text: "Remember last selected date in Report window"

            indicator: Rectangle {
              implicitWidth: 18
              implicitHeight: 18
              x: optionLastSelected.leftPadding
              y: parent.height / 2 - height / 2
              radius: 3
              border.color: parent.focus ? "orange" : fg

              Rectangle {
                  width: 10
                  height: 10
                  x: 4
                  y: 4
                  radius: 2
                  color: optionLastSelected.down ? "grey" : "black"
                  visible: optionLastSelected.checked
              }
            }

            contentItem: Text {
                 text: optionLastSelected.text
                 font.family: "Roboto"
                 font.bold: true
                 color: fg
                 verticalAlignment: Text.AlignVCenter
                 leftPadding: optionLastSelected.indicator.width + optionLastSelected.spacing
             }

            onToggled: {

            }
        }


        // Reallocate equally or weighted
        CheckBox {
            id: optionWeighted
            x: form.leftEdge
            y: optionLastSelected.y + optionLastSelected.height + 20
            width: 40
            height: 30
            checked: false
            text: "Weighted reallocation of time"

            indicator: Rectangle {
              implicitWidth: 18
              implicitHeight: 18
              x: optionWeighted.leftPadding
              y: parent.height / 2 - height / 2
              radius: 3
              border.color: parent.focus ? "orange" : fg

              Rectangle {
                  width: 10
                  height: 10
                  x: 4
                  y: 4
                  radius: 2
                  color: optionWeighted.down ? "grey" : "black"
                  visible: optionWeighted.checked
              }
            }

            contentItem: Text {
                 text: optionWeighted.text
                 font.family: "Roboto"
                 font.bold: true
                 color: fg
                 verticalAlignment: Text.AlignVCenter
                 leftPadding: optionWeighted.indicator.width + optionWeighted.spacing
             }

            onToggled: {

            }
        }

        // Autosave on exit
        CheckBox {
            id: optionAutosave
            x: form.leftEdge
            y: optionWeighted.y + optionWeighted.height + 20
            width: 40
            height: 30
            checked: false
            text: "Autosave on exit"

            indicator: Rectangle {
              implicitWidth: 18
              implicitHeight: 18
              x: optionAutosave.leftPadding
              y: parent.height / 2 - height / 2
              radius: 3
              border.color: parent.focus ? "orange" : fg

              Rectangle {
                  width: 10
                  height: 10
                  x: 4
                  y: 4
                  radius: 2
                  color: optionAutosave.down ? "grey" : "black"
                  visible: optionAutosave.checked
              }
            }

            contentItem: Text {
                 text: optionAutosave.text
                 font.family: "Roboto"
                 font.bold: true
                 color: fg
                 verticalAlignment: Text.AlignVCenter
                 leftPadding: optionAutosave.indicator.width + optionAutosave.spacing
             }

            onToggled: {

            }
        }


        // Minimize to tray:
        CheckBox {
            id: optionMinimizeToTray
            x: form.leftEdge
            y: optionAutosave.y + optionAutosave.height + 20
            width: 40
            height: 30
            checked: false
            text: "Minimize to system tray"

            indicator: Rectangle {
              implicitWidth: 18
              implicitHeight: 18
              x: optionMinimizeToTray.leftPadding
              y: parent.height / 2 - height / 2
              radius: 3
              border.color: parent.focus ? "orange" : fg

              Rectangle {
                  width: 10
                  height: 10
                  x: 4
                  y: 4
                  radius: 2
                  color: optionMinimizeToTray.down ? "grey" : "black"
                  visible: optionMinimizeToTray.checked
              }
            }

            contentItem: Text {
                 text: optionMinimizeToTray.text
                 font.family: "Roboto"
                 font.bold: true
                 color: fg
                 verticalAlignment: Text.AlignVCenter
                 leftPadding: optionMinimizeToTray.indicator.width + optionMinimizeToTray.spacing
             }

            onToggled: {

            }
        }


          // Starting view selection
          Label {
              id: option3label
              x: form.leftEdge
              y: optionMinimizeToTray.y + optionMinimizeToTray.height + 20
              width: 130
              height: 30
              verticalAlignment: Qt.AlignVCenter
              horizontalAlignment: Qt.AlignLeft
              text: "Starting view:"
              font.family: "Roboto"
              font.bold: true
              color: fg
          }

          ComboBox {
              id: optionStartingView
              x: form.leftEdge + option3label.width
              y: optionMinimizeToTray.y + optionMinimizeToTray.height + 20

              model: ["Today","This month","This year"]

              background: Rectangle {
                  implicitWidth: 120
                  implicitHeight: 30
                  color: blue5
//                  border.color: control.pressed ? "#17a81a" : "#21be2b"
                  border.width: parent.focus ? 2 : 1
                  border.color: parent.focus ? "orange" : fg
                  radius: 2
              }

              wheelEnabled: true
          }

          // Color scheme selection
          Label {
              id: option4label
              x: form.leftEdge
              y: optionStartingView.y + optionStartingView.height + 20
              width: 130
              height: 30
              verticalAlignment: Qt.AlignVCenter
              horizontalAlignment: Qt.AlignLeft
              text: "Color scheme:"
              font.family: "Roboto"
              font.bold: true
              color: fg
          }

          ComboBox {
              id: optionColorScheme
              x: form.leftEdge + option3label.width
              y: optionStartingView.y + optionStartingView.height + 20

              model: ["Dark","Light"]

              background: Rectangle {
                  implicitWidth: 120
                  implicitHeight: 30
                  color: blue5
                  border.width: parent.focus ? 2 : 1
                  border.color: parent.focus ? "orange" : fg
                  radius: 2
              }

              wheelEnabled: true

              onActivated: {
                  // Make the color scheme switch when the box is scrolled through
                  listView.model.settingColorScheme  = optionColorScheme.currentIndex;
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
            x: 130
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
               // Set back to old values
                optionLastSelected.checkState   = oldOptionLastSelectedDate;
                optionWeighted.checkState       = oldOptionWeightedReallocation;
                optionAutosave.checkState       = oldOptionAutosave;
                optionMinimizeToTray.checkState = oldOptionMinimizeToTray;
                optionStartingView.currentIndex = oldOptionStartingView;
                optionColorScheme.currentIndex  = oldOptionColorScheme;
                listView.model.settingLastSelectedDate     = optionLastSelected.checkState;   // 0: Unchecked, 1: Partially Checked, 2: Checked
                listView.model.settingWeightedReallocation = optionWeighted.checkState;       // 0: Unchecked, 1: Partially Checked, 2: Checked
                listView.model.settingAutosave             = optionAutosave.checkState;       // 0: Unchecked, 1: Partially Checked, 2: Checked
                listView.model.settingMinimizeToTray       = optionMinimizeToTray.checkState; // 0: Unchecked, 1: Partially Checked, 2: Checked
                listView.model.settingStartingView         = optionStartingView.currentIndex;
                listView.model.settingColorScheme          = optionColorScheme.currentIndex;
                settingsWindow.close();
            }

        }

        Button {
            height: 20
            width: 60
            x: 210
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
                // Apply new values
                listView.model.settingLastSelectedDate     = optionLastSelected.checkState;   // 0: Unchecked, 1: Partially Checked, 2: Checked
                listView.model.settingWeightedReallocation = optionWeighted.checkState;       // 0: Unchecked, 1: Partially Checked, 2: Checked
                listView.model.settingAutosave             = optionAutosave.checkState;       // 0: Unchecked, 1: Partially Checked, 2: Checked
                listView.model.settingMinimizeToTray       = optionMinimizeToTray.checkState; // 0: Unchecked, 1: Partially Checked, 2: Checked
                listView.model.settingStartingView         = optionStartingView.currentIndex;
                listView.model.settingColorScheme          = optionColorScheme.currentIndex;

                // Let the system tray know if we changed the setting:
                TrayManager.settingMinimizeToTray = listView.model.settingMinimizeToTray;
                TrayManager.updateSettings();

                settingsWindow.close();
            }
        }

    }


}
