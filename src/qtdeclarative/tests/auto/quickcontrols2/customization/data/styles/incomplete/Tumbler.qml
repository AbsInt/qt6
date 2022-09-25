// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

import QtQuick
import QtQuick.Templates as T

T.Tumbler {
    id: control
    objectName: "tumbler-incomplete"

    contentItem: ListView {
        objectName: "tumbler-contentItem-incomplete"
    }

    background: Item {
        objectName: "tumbler-background-incomplete"
    }
}
