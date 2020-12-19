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
import QtQuick 2.11
import QtQuick.Controls 2.4
import QtQuick.Layouts 1.3
import QtQuick.Window 2.11

Window {
    id: reportWindow
    width: 600
    height: Math.max(menu.height + bar.height + 50 + listDaily.count*40 + sum.height, 400)  // It seems the minimumHeight property is not yet respected when opening the window, so we have to ensure is manually
    minimumWidth: 600
    minimumHeight: 400
    maximumHeight: 1080
    x: appWindow.x < appWindow.screen.width - (appWindow.width + width) ? (appWindow.x + appWindow.width + 10) : (appWindow.x - width - 10)
    y: appWindow.y
    title: "Timekeeper Report"

    color: bg

    property int    displaymode: 0    // 0: hh:mm  1: x,x h
    property int    viewIndex:   0    // 0: List  1: Chart
    property date   currentDate: new Date()
    property int    dateCount:   0    // Index corresponding to day queried for daily breakdown (0 = today)
    property int    monthCount:  currentDate.getMonth() + 1    // Index corresponding to month queried for monthly breakdown (1 = January)
    property int    year:        currentDate.getFullYear();    // Current year for yearly breakdown
    property string date: ""
    property string month: ""
    property bool   reallocating: false
    property int    sourceID: -1      // Source task for time reallocation

    EditContextMenuReport {
        id: editContextMenuReport
    }

    ReallocateDialog {
        id: reallocateDialog
    }


    signal opened()


    onOpened: {
        if (listView.model.settingLastSelectedDate===0) {
            // Start with current date when opening:
            currentDate = new Date();
            dateCount = 0;
            monthCount = currentDate.getMonth() + 1;   // 1 = January
            year = currentDate.getFullYear();
        }
        date=listDaily.model.updateDailyList(dateCount);
        month=listMonthly.model.updateMonthlyList(monthCount, year);
        listYearly.model.updateYearlyList(year);

        //onsole.log("reportWindow.height = ",reportWindow.height);
        //console.log("bar.height          = ",bar.height);
        //console.log("menu.height         = ",menu.height);
        //console.log("sum.height          = ",sum.height);
        //console.log("stack.height        = ",stack.height);
        //console.log("listDaily.count     = ",listDaily.count);
    }


    TabBar {
        id: bar
        y: 0
        width: parent.width
        TabButton {
            text: "Daily"
            font.bold: true
            background: Rectangle {
                color: parent.checked ? blue5 : grey3
            }
            onClicked: {date=listDaily.model.updateDailyList(dateCount)}        // Keep the last selected day when switching tabs
//            Component.onCompleted: {date=listDaily.model.updateDailyList(0)}    // This is only called once on creation of the Window, not every time it is shown
        }
        TabButton {
            text: "Monthly"
            font.bold: true
            background: Rectangle {
                color: parent.checked ? blue5 : grey3
            }
            onClicked: {listMonthly.model.updateMonthlyList(monthCount, year)}
//            Component.onCompleted: {month=listMonthly.model.updateMonthlyList(monthCount)}    // This is only called once on creation of the Window, not every time it is shown
        }
        TabButton {
            text: "Yearly"
            font.bold: true
            background: Rectangle {
                color: parent.checked ? blue5 : grey3
            }
            onClicked: {listYearly.model.updateYearlyList(year)}
        }
    }


    //  Top Menu
        Rectangle {
            id: menu
            anchors.top: bar.bottom
            width: parent.width
            height: 50
            color: bg


            // Switch time display mode
            RoundButton {
                id: switchMode
                anchors.verticalCenter: parent.verticalCenter
                x: 25
                radius: 5
                height: 30
                width: 50
                flat: true

                ToolTip {
                    parent: switchMode
                    visible: parent.hovered
                    delay: 1000

                    contentItem: Text {
                        text: "Switch display mode"
                        font.family: "Roboto"
                        color: txtColor
                    }

                    background: Rectangle {
                        color: button1
                        border.color: "transparent"
                        radius: 10
                    }
                }

                contentItem: Text {
                    font.bold: true
                    font.pixelSize: 12
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    text: displaymode==0 ? "hh:mm" : "1.5"
                    color: switchMode.down ? button1 : button2
                }

                background: Rectangle {
                    height: parent.height
                    width: parent.width
                    radius: parent.radius
                    color: switchMode.down ? button2 : button1
                }

                onClicked: {
                    if (displaymode==0) {
                        displaymode = 1;
                    }
                    else {
                        displaymode = 0;
                    }
                }
            }


            // Jump to today
            RoundButton {
                id: jumpHome
                anchors.verticalCenter: parent.verticalCenter
                x: 90
                radius: 5
                height: 30
                width: 30
                flat: true
                icon.source: "../icons/home-solid.png"
                icon.color: jumpHome.down ? button1 : button2

                ToolTip {
                    parent: jumpHome
                    visible: parent.hovered
                    delay: 1000

                    contentItem: Text {
                        text: "Jump to today"
                        font.family: "Roboto"
                        color: txtColor
                    }

                    background: Rectangle {
                        color: button1
                        border.color: "transparent"
                        radius: 10
                    }
                }

                background: Rectangle {
                    height: parent.height
                    width: parent.width
                    radius: parent.radius
                    color: jumpHome.down ? button2 : button1
                }

                onClicked: {
                    currentDate = new Date();
                    dateCount = 0;
                    date=listDaily.model.updateDailyList(dateCount);
                    monthCount = currentDate.getMonth() + 1;
                    month=listMonthly.model.updateMonthlyList(monthCount, year);
                    year = currentDate.getFullYear();
                    listYearly.model.updateYearlyList(year);
                }
            }

        }


    // Info area to the right of the ListView
/*    Item {
        id: infoArea
        visible: false//reallocating

        x: 400
        y: bar.height + menu.height + 50
        height: stack.height - 50   // Overall stack height minus the date bar
        width: 200

        Rectangle {
            height: 30
            width: parent.width - 20
            x: 10
            y: 0
            radius: 5
            color: fg
            border.color: fg

            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                font.bold: true
                text: "Reallocate time"
                color: "black"
            }
        }

        Rectangle {
            x: 10
            y: 25
            width: parent.width - 20
            height: 150
            color: bg
            border.color: fg

            Text {
                y: 10
                width: parent.width
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.verticalCenter: parent.verticalCenter
                wrapMode: Text.WordWrap
                leftPadding: 2
                rightPadding: 2

                color: fg
                font.bold: true
                font.pixelSize: 12
                text: "Reallocating daily time of task \"" + listView.model.get(currentEntry).title + "\"\n\n Please mark tasks to allocate to and press OK!"

            }
        }

        Rectangle {
            id: footer
            x: 10
            y: 174
            height: 40
            width: parent.width - 20
            color: bg
            border.color: fg

            // Cancel button
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
                        border.color: parent.down ? fg : bg
                        border.width: 2
                        color: parent.down ? bg : fg
                    }
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        color: parent.down ? fg : "black"
                        font.bold: true
                        text: "Cancel"
                    }

                     onClicked: {
                         reallocating = false;
                         listDaily.model.resetAllocate();
                     }

                }

                // OK button
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
                        border.color: parent.down ? fg : bg
                        border.width: 2
                        color: parent.down ? bg : fg
                    }
                    Text {
                        anchors.horizontalCenter: parent.horizontalCenter
                        anchors.verticalCenter: parent.verticalCenter
                        color: parent.down ? fg: "black"
                        font.bold: true
                        text: "OK"
                    }

                    onClicked: {
                        listDaily.model.reallocate(sourceID, dateCount, listView.model.settingWeightedReallocation);
                        reallocating = false;
                        sourceID     = -1;
                        listDaily.model.resetAllocate();
                    }

                }

        }

    }
*/


    // Stack of Daily / Monthly / Yearly breakdowns
    StackLayout {
        id: stack
        anchors.top: menu.bottom
        height: reportWindow.height - bar.height - menu.height - sum.height
        width: parent.width
        currentIndex: bar.currentIndex

        // Daily Stack containing list layout
        StackLayout {
            id: stackDaily
            height: parent.height
            width: parent.width
            currentIndex: viewIndex

            Item {
                Rectangle {
                    id: title1
                    height: 50
                    width: parent.width
                    color: bg

                    RoundButton {
                        id: cycleDown
                        anchors.verticalCenter: parent.verticalCenter
                        x: 90
                        radius: 5
                        height: 30
                        width: 30
                        flat: true

                        contentItem: Text {
                            font.bold: true
                            font.pixelSize: 14
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            text: "-"
                            color: cycleDown.down ? button1 : button2
                        }

                        background: Rectangle {
                            height: parent.height
                            width: parent.width
                            radius: parent.radius
                            color: cycleDown.down ? button2 : button1
                        }

                        onClicked: {
                            if (!reallocating) {
                                dateCount = dateCount - 1;
                                date=listDaily.model.updateDailyList(dateCount);
                            }
                        }
                    }

                    Rectangle {
                        x: 120
                        width: 160
                        height: parent.height
                        anchors.verticalCenter: parent.verticalCenter
                        color: "transparent"
                        Text {
                            width: parent.width
                            height: parent.height
                            font.bold: true
                            font.family: "Roboto"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            color: txtColor

                            text: date                           
                        }

                        MouseArea {
                            anchors.fill: parent
                            onWheel: {
                                //console.log("Wheel: " + wheel.angleDelta)
                               if (!reallocating) {
                                   dateCount = dateCount - wheel.angleDelta.y/120;   // 15 deg step = 120 units of wheel.angleDelta
                                   date=listDaily.model.updateDailyList(dateCount);
                               }
                            }
                        }

                    }

                    RoundButton {
                        id: cycleUp
                        anchors.verticalCenter: parent.verticalCenter
                        x: 280
                        radius: 5
                        height: 30
                        width: 30
                        flat: true

                        contentItem: Text {
                            font.bold: true
                            font.pixelSize: 14
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            text: "+"
                            color: cycleUp.down ? button1 : button2
                        }

                        background: Rectangle {
                            height: parent.height
                            width: parent.width
                            radius: parent.radius
                            color: cycleUp.down ? button2 : button1
                        }

                        onClicked: {
                            if (!reallocating) {
                                dateCount = dateCount + 1;
                                date=listDaily.model.updateDailyList(dateCount);
                            }
                        }
                    }
                }

                ListView {
                    id: listDaily
                    width: 600
                    height: parent.height - title1.height
                    anchors.top: title1.bottom

                    clip: true
                    focus: true
                    boundsBehavior: Flickable.StopAtBounds

                    delegate: listDailyDelegate

                    model: taskModel

                    ScrollBar.vertical: ScrollBar { }
                }
            }

        }


        // Monthly Stack containing list layout
        StackLayout {
            id: stackMonthly
            height: parent.height
            width: parent.width
            currentIndex: viewIndex

            Item {
                Rectangle {
                    id: title2
                    height: 50
                    width: parent.width
                    color: bg

                    RoundButton {
                        id: cycleDownMonth
                        anchors.verticalCenter: parent.verticalCenter
                        x: 90
                        radius: 5
                        height: 30
                        width: 30
                        flat: true

                        contentItem: Text {
                            font.bold: true
                            font.pixelSize: 14
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            text: "-"
                            color: cycleDownMonth.down ? button1 : button2
                        }

                        background: Rectangle {
                            height: parent.height
                            width: parent.width
                            radius: parent.radius
                            color: cycleDownMonth.down ? button2 : button1
                        }

                        onClicked: {
                            if (monthCount > 1) {
                                monthCount = monthCount - 1;
                            }
                            else {
                                monthCount = 1;
                            }
                            month=listMonthly.model.updateMonthlyList(monthCount, year);
                        }
                    }

                    Rectangle {
                        x: 120
                        width: 160
                        height: parent.height
                        anchors.verticalCenter: parent.verticalCenter
                        color: "transparent"
                        Text {
                            width: parent.width
                            height: parent.height
                            font.bold: true
                            font.family: "Roboto"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            color: txtColor

                            text: month + " " + year //currentDate.getFullYear()
                        }

                        MouseArea {
                            anchors.fill: parent
                            onWheel: {
                                //console.log("Wheel: " + wheel.angleDelta)
                                if (monthCount > 0) {
                                    monthCount = Math.min(monthCount - wheel.angleDelta.y/120, 12);   // 15 deg step = 120 units of wheel.angleDelta
                                }
                                else {
                                    monthCount = 1;
                                }
                                month=listMonthly.model.updateMonthlyList(monthCount, year);
                            }
                        }

                    }

                    RoundButton {
                        id: cycleUpMonth
                        anchors.verticalCenter: parent.verticalCenter
                        x: 280
                        radius: 5
                        height: 30
                        width: 30
                        flat: true

                        contentItem: Text {
                            font.bold: true
                            font.pixelSize: 14
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            text: "+"
                            color: cycleUpMonth.down ? button1 : button2
                        }

                        background: Rectangle {
                            height: parent.height
                            width: parent.width
                            radius: parent.radius
                            color: cycleUpMonth.down ? button2 : button1
                        }

                        onClicked: {
                            if (monthCount < 12) {
                                monthCount = monthCount + 1;
                            }
                            else {
                                monthCount = 12;
                            }
                            month=listMonthly.model.updateMonthlyList(monthCount, year);
                        }
                    }
                }

                ListView {
                    id: listMonthly
                    width: 600
                    height: parent.height - title2.height
                    anchors.top: title2.bottom

                    clip: true
                    focus: true
                    boundsBehavior: Flickable.StopAtBounds

                    delegate: listMonthlyDelegate

                    model: taskModel

                    ScrollBar.vertical: ScrollBar { }
                }
            }

        }


        // Yearly Stack containing list layout
        StackLayout {
            id: stackYearly
            height: parent.height
            width: parent.width
            currentIndex: viewIndex

            Item {
                Rectangle {
                    id: title3
                    height: 50
                    width: parent.width
                    color: bg

                    RoundButton {
                        id: cycleDownYear
                        anchors.verticalCenter: parent.verticalCenter
                        x: 90
                        radius: 5
                        height: 30
                        width: 30
                        flat: true

                        contentItem: Text {
                            font.bold: true
                            font.pixelSize: 14
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            text: "-"
                            color: cycleDownYear.down ? button1 : button2
                        }

                        background: Rectangle {
                            height: parent.height
                            width: parent.width
                            radius: parent.radius
                            color: cycleDownYear.down ? button2 : button1
                        }

                        onClicked: {
                            year = year - 1;
                            listYearly.model.updateYearlyList(year);
                        }
                    }

                    Rectangle {
                        x: 120
                        width: 160
                        height: parent.height
                        anchors.verticalCenter: parent.verticalCenter
                        color: "transparent"
                        Text {
                            width: parent.width
                            height: parent.height
                            font.bold: true
                            font.family: "Roboto"
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            color: txtColor

                            text: year
                        }

                        MouseArea {
                            anchors.fill: parent
                            onWheel: {
                                year = year - wheel.angleDelta.y/120;   // 15 deg step = 120 units of wheel.angleDelta
                                listYearly.model.updateYearlyList(year);
                            }
                        }

                    }

                    RoundButton {
                        id: cycleUpYear
                        anchors.verticalCenter: parent.verticalCenter
                        x: 280
                        radius: 5
                        height: 30
                        width: 30
                        flat: true

                        contentItem: Text {
                            font.bold: true
                            font.pixelSize: 14
                            horizontalAlignment: Text.AlignHCenter
                            verticalAlignment: Text.AlignVCenter
                            text: "+"
                            color: cycleUpYear.down ? button1 : button2
                        }

                        background: Rectangle {
                            height: parent.height
                            width: parent.width
                            radius: parent.radius
                            color: cycleUpYear.down ? button2 : button1
                        }

                        onClicked: {
                            year = year + 1;
                            listYearly.model.updateYearlyList(year);
                        }
                    }
                }

                ListView {
                    id: listYearly
                    width: 600
                    height: parent.height - title3.height
                    anchors.top: title3.bottom

                    clip: true
                    focus: true
                    boundsBehavior: Flickable.StopAtBounds

                    delegate: listYearlyDelegate

                    model: taskModel

                    ScrollBar.vertical: ScrollBar { }
                }
            }

        }

    }


    // Total sum over all tasks
    Rectangle {
        id: sum
        anchors.top: stack.bottom
        height: 50
        width: 400
        color: bg

        Text {
            height: parent.height
            width: 50
            x: 15
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft

            font.bold: true
            font.family: "Roboto"
            color: txtColor

            text: "TOTAL:"
        }

        Text {
           id: totalTimeText
           height: parent.height
           width: 50
           x: 265
           verticalAlignment: Text.AlignVCenter
           horizontalAlignment: Text.AlignRight

           function timeStringDecimal() {
               if (bar.currentIndex == 0) return listView.model.TotalSecondsDaily/3600.0;
               else if (bar.currentIndex == 1) return listView.model.TotalSecondsMonthly/3600.0;
               else if (bar.currentIndex == 2) return listView.model.TotalSecondsYearly/3600.0;
           }


           font.family: "Roboto"
           font.bold: true
           text: {
               if (bar.currentIndex == 0) {
                   displaymode==0 ? listView.model.TotalTimeDaily : parseFloat(timeStringDecimal()).toFixed(2) + " h"
               }
               else if (bar.currentIndex == 1) {
                   displaymode==0 ? listView.model.TotalTimeMonthly : parseFloat(timeStringDecimal()).toFixed(2) + " h"
               }
               else if (bar.currentIndex == 2) {
                   displaymode==0 ? listView.model.TotalTimeYearly : parseFloat(timeStringDecimal()).toFixed(2) + " h"
               }
           }

           color: txtColor
        }
    }


    // Days worked
    Rectangle {
        id: daysWorked
        anchors.top: stack.bottom
        anchors.left: sum.right
        height: 50
        width: 200
        visible: bar.currentIndex > 0
        color: bg

        Text {
            height: parent.height
            width: 100
            x: 15
            verticalAlignment: Text.AlignVCenter
            horizontalAlignment: Text.AlignLeft

            font.bold: true
            font.family: "Roboto"
            color: txtColor

            text: "DAYS WORKED:"
        }

        Text {
           id: daysWorkedText
           height: parent.height
           width: 50
           x: 100
           verticalAlignment: Text.AlignVCenter
           horizontalAlignment: Text.AlignRight

           font.family: "Roboto"
           font.bold: true
           text: {
               if (bar.currentIndex == 1) {
                   (listView.model.DaysWorkedMonthly);
               }
               else if (bar.currentIndex == 2) {
                   (listView.model.DaysWorkedYearly);
               }
               else {
                   0;
               }
           }

           color: txtColor
        }
    }


///////  DELEGATES ///////

    // Component defining the delegate for tasks
    Component {
        id: listDailyDelegate
        Rectangle {
            width: 400
            height: 40
            color: bg
            border.color: (reallocating&&model.allocateTime===1) ? "orange" : borderColor
            border.width: (reallocating&&model.allocateTime===1) ? 2 : 1
            radius: 0

            anchors {
                left: parent.left
            }


            // Task title
            Text {
                id: text01
                x: 15
                text:  model.title     // Need to implement the roleNames() and data() method in the model to access this property
                font.bold: true
                font.family: "Roboto"
                color: txtColor
                height: parent.height
                verticalAlignment: Text.AlignVCenter
            }

            // Task time
            Text {
                id: timeText01
                x: 265
                height: parent.height
                width: 50
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignRight

                function timeStringDecimal() {
                    return model.elapsedSecDaily/3600.0;
                }

                text: displaymode==0 ? model.DailyString : parseFloat(timeStringDecimal()).toFixed(2) + " h"
                color: txtColor
            }


            // Clickable area of task
            MouseArea {
               id: ma
               anchors.left: parent.left
               height: parent.height
               width: parent.width - 80
               acceptedButtons: Qt.LeftButton
               onClicked: {
                   if ((mouse.button === Qt.LeftButton)&&(reallocating))
                   {
                       listDaily.model.switchAllocate(index);
                   }
               }
            }


            // "Edit" button
            RoundButton {
                id: edit1
                x: 350
                y: 5
                radius: 5
                height: 30
                width: 30
                flat: true
                icon.source: "../icons/edit-regular.png"
                icon.color: edit1.down ? button1 : button2

                background: Rectangle {
                    height: parent.height
                    width: parent.width
                    radius: parent.radius
                    color: edit1.down ? button2 : button1
                }

                onClicked: {
                    if (!reallocating) {
                        currentEntry = index;
                        editContextMenuReport.popup();
                    }
                }
            }  // RoundButton


            // Histogram Bar
            Rectangle {
                id: histBar
                function histogramLength() {
                    var len = (model.elapsedSecDaily * 180.0) / (listView.model.TotalSecondsDaily + 1);  // Note: TotalSecondsDaily is a QProperty, elapsedSecDaily is a model role
                    //console.log("len = ",len);
                    return len;
                }
                x: 410
                y: 5
                height: 30
                width: 0 + parseInt(histogramLength())
                border.color: fg
                color: taskActiveColor

                Text {
                    function histogramText() {
                        var txt = model.elapsedSecDaily * 100;
                        if (txt > 0) txt = txt / listView.model.TotalSecondsDaily;
                        return txt;
                    }
                    x: histBar.width > 50 ? histBar.width - 45 : histBar.width
                    y: 0
                    height: 30
                    width: 45
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: fg
                    font.pointSize: 8
                    visible: histogramText() > 0 ? true : false

                    text: String(parseFloat(histogramText()).toFixed(1)) + "%"
                }
            }

        }

    }


    // Component defining the delegate for tasks
    Component {
        id: listMonthlyDelegate
        Rectangle {
            width: 400
            height: 40
            color: bg
            border.color: borderColor
            radius: 0

            anchors {
                left: parent.left
            }


            // Task title
            Text {
                id: text02
                x: 15
                text:  model.title     // Need to implement the roleNames() and data() method in the model to access this property
                font.bold: true
                font.family: "Roboto"
                color: txtColor
                height: parent.height
                verticalAlignment: Text.AlignVCenter
            }

            // Task time
            Text {
                id: timeText02
                x: 265
                height: parent.height
                width: 50
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignRight

                function timeStringDecimal() {
                    return model.elapsedSecMonthly/3600.0;
                }

                text: displaymode==0 ? model.MonthlyString : parseFloat(timeStringDecimal()).toFixed(2) + " h"
                color: txtColor
            }

            // Histogram Bar
            Rectangle {
                id: histBar
                function histogramLength() {
                    var len = (model.elapsedSecMonthly * 180.0) / (listView.model.TotalSecondsMonthly + 1);
                    //console.log("len = ",len);
                    return len;
                }
                x: 410
                y: 5
                height: 30
                width: 0 + parseInt(histogramLength())
                border.color: fg
                color: taskActiveColor

                Text {
                    function histogramText() {
                        var txt = model.elapsedSecMonthly * 100;
                        if (txt > 0) txt = txt / listView.model.TotalSecondsMonthly;
                        return txt;
                    }
                    x: histBar.width > 50 ? histBar.width - 45 : histBar.width
                    y: 0
                    height: 30
                    width: 45
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: fg
                    font.pointSize: 8
                    visible: histogramText() > 0 ? true : false

                    text: String(parseFloat(histogramText()).toFixed(1)) + "%"
                }
            }

        }

    }


    // Component defining the delegate for tasks
    Component {
        id: listYearlyDelegate
        Rectangle {
            width: 400
            height: 40
            color: bg
            border.color: borderColor
            radius: 0

            anchors {
                left: parent.left
            }


            // Task title
            Text {
                id: text03
                x: 15
                text:  model.title     // Need to implement the roleNames() and data() method in the model to access this property
                font.bold: true
                font.family: "Roboto"
                color: txtColor
                height: parent.height
                verticalAlignment: Text.AlignVCenter
            }

            // Task time
            Text {
                id: timeText03
                x: 265
                height: parent.height
                width: 50
                verticalAlignment: Text.AlignVCenter
                horizontalAlignment: Text.AlignRight

                function timeStringDecimal() {
                    return model.elapsedSecYearly/3600.0;
                }

                text: displaymode==0 ? model.YearlyString : parseFloat(timeStringDecimal()).toFixed(2) + " h"
                color: txtColor
            }

            // Histogram Bar
            Rectangle {
                id: histBar
                function histogramLength() {
                    var len = (model.elapsedSecYearly * 180.0) / (listView.model.TotalSecondsYearly + 1);
                    //console.log("len = ",len);
                    return len;
                }
                x: 410
                y: 5
                height: 30
                width: 0 + parseInt(histogramLength())
                border.color: fg
                color: taskActiveColor

                Text {
                    function histogramText() {
                        var txt = model.elapsedSecYearly * 100;
                        if (txt > 0) txt = txt / listView.model.TotalSecondsYearly;
                        return txt;
                    }
                    x: histBar.width > 50 ? histBar.width - 45 : histBar.width
                    y: 0
                    height: 30
                    width: 45
                    horizontalAlignment: Text.AlignHCenter
                    verticalAlignment: Text.AlignVCenter
                    color: fg
                    font.pointSize: 8
                    visible: histogramText() > 0 ? true : false

                    text: String(parseFloat(histogramText()).toFixed(1)) + "%"
                }
            }

        }

    }

}
