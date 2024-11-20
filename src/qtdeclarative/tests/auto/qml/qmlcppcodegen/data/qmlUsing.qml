// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

pragma Strict

import QtQml
import TestTypes as T

T.UsingUserObject {
    id: self
    property int valA: val.a
    // property int valB: val.getB()
    property int myA: a
    property int myB: getB()
    property int myA2: self.a
    property int myB2: self.getB()

    function twiddle() {
        val.a = 55
        // val.setB(56)
        a = 57
        setB(58)
        self.a = 59
        self.setB(60)
    }
}
