// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
import QtQuick
import QtQuick.Controls

Rectangle {
    id: secondaryRectangle

    property int gridRotation: 0

    color: "blue"

    Text {
        id: title

        text: "Second QML View"
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

        Grid {
            id: grid

            columns: 3
            rows: 3
            spacing: 50
            rotation: gridRotation
            anchors.horizontalCenter: parent.horizontalCenter

            Repeater {
                id: repeater

                model: [
                    "green",
                    "lightblue",
                    "grey",
                    "red",
                    "black",
                    "white",
                    "pink",
                    "yellow",
                    "orange"
                ]

                Rectangle {
                    required property string modelData

                    height: 50
                    width: 50
                    color: modelData
                }
            }
        }
    }
}
