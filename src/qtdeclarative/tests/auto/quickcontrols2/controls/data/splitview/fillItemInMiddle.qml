// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Controls

SplitView {
    anchors.fill: parent

    Rectangle {
        objectName: "salmon"
        color: objectName
        implicitWidth: 25
        implicitHeight: 25
    }
    Rectangle {
        objectName: "navajowhite"
        color: objectName
        implicitWidth: 100
        implicitHeight: 100

        SplitView.fillWidth: true
    }
    Rectangle {
        objectName: "steelblue"
        color: objectName
        implicitWidth: 200
        implicitHeight: 200
    }
}
