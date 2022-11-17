// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import QtQuick 2.12
import QtQuick.Window 2.3

Item {
    width: 640
    height: 450

    property alias tableView: tableView
    property Component delegate: tableViewDelegate

    TableView {
        id: tableView
        width: 600
        height: 400
        anchors.margins: 1
        clip: true
        delegate: tableViewDelegate
        columnSpacing: 1
        rowSpacing: 1
    }

    Component {
        id: tableViewDelegate
        Rectangle {
            implicitWidth: tableView.width
            implicitHeight: 50
            objectName: "tableViewDelegate"
            color: "lightgray"
            border.width: 1
            Text {
                anchors.centerIn: parent
                text: modelData
            }
        }
    }

}
