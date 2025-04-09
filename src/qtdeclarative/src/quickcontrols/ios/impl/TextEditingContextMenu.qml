// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import QtQuick.Controls.iOS
import QtQuick.Controls.iOS.impl as IOSImpl

Menu {
    id: menu
    popupType: Popup.Native

    required property var control

    IOSImpl.CutAction {
        control: menu.control
    }
    IOSImpl.CopyAction {
        control: menu.control
    }
    IOSImpl.PasteAction {
        control: menu.control
    }
    IOSImpl.DeleteAction {
        control: menu.control
    }

    MenuSeparator {}

    IOSImpl.SelectAllAction {
        control: menu.control
    }
}
