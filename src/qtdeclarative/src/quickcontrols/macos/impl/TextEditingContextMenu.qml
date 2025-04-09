// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

import QtQuick.Controls.macOS
import QtQuick.Controls.macOS.impl as MacOSImpl

Menu {
    id: menu
    popupType: Popup.Window

    required property var control

    MacOSImpl.CutAction {
        control: menu.control
    }
    MacOSImpl.CopyAction {
        control: menu.control
    }
    MacOSImpl.PasteAction {
        control: menu.control
    }
    MacOSImpl.DeleteAction {
        control: menu.control
    }

    MenuSeparator {}

    MacOSImpl.SelectAllAction {
        control: menu.control
    }
}
