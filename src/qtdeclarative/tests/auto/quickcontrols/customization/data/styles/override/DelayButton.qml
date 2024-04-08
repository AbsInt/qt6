// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

import QtQuick
import "../simple" as Simple

Simple.DelayButton {
    id: control
    objectName: "delaybutton-override"

    contentItem: Item {
        objectName: "delaybutton-contentItem-override"
    }

    background: Item {
        objectName: "delaybutton-background-override"
    }
}
