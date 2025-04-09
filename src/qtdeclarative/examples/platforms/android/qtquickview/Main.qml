// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
import QtQuick
import QtQuick.Controls

Rectangle {
    id: mainRectangle

    property string colorStringFormat: "#1CB669"

    signal onClicked()

    color: colorStringFormat

    Text {
        id: helloText

        text: "First QML View"
        color: "white"
        font.pointSize: 72
        width: parent.width
        font.bold: true
        wrapMode: Text.Wrap
        horizontalAlignment: Text.AlignHCenter
        anchors {
            top: parent.top
            topMargin: 50
            horizontalCenter: parent.horizontalCenter
        }
    }

    Column {
        anchors.centerIn: parent
        width: parent.width
        spacing: 30

        Text {
            id: changeColorText

            text: "Tap button to change Java view background color"
            wrapMode: Text.Wrap
            color: "white"
            width: parent.width
            horizontalAlignment: Text.AlignHCenter
            anchors.horizontalCenter: parent.horizontalCenter
        }

        Button {
            id: button

            anchors.horizontalCenter: parent.horizontalCenter

            onClicked: mainRectangle.onClicked()

            background: Rectangle {
                id: buttonBackground

                radius: 14
                color: "#6200EE"
                opacity: button.down ? 0.6 : 1
                scale: button.down ? 0.9 : 1
            }

            contentItem: Text {
                id: buttonText

                text: "CHANGE COLOR"
                color: "white"
                font.pixelSize: 58
                minimumPixelSize: 10
                fontSizeMode: Text.Fit
                font.bold: true
                wrapMode: Text.Wrap
                horizontalAlignment: Text.AlignHCenter
                verticalAlignment: Text.AlignVCenter
            }
        }
    }
}
