// Copyright (C) 2025 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

pragma ComponentBehavior: Bound

import QtQuick
import QtQuick.Templates as T
import QtQuick.NativeStyle as NativeStyle

NativeStyle.DefaultSearchField {
    id: control

    readonly property bool __nativeSearchIndicator: searchIndicator.indicator instanceof NativeStyle.SearchField
    readonly property bool __nativeClearIndicator: clearIndicator.indicator instanceof NativeStyle.SearchField
    readonly property Item __focusFrameTarget: control

    contentItem: T.TextField {
        text: control.text

        color: control.palette.text
        selectionColor: control.palette.highlight
        selectedTextColor: control.palette.highlightedText
        verticalAlignment: Text.AlignVCenter

        readonly property Item __focusFrameControl: control
        readonly property bool __ignoreNotCustomizable: true
    }

    NativeStyle.SearchField {
        id: search
        visible: control.__nativeSearchIndicator
        control: control
        subControl: NativeStyle.SearchField.Search
        x: searchIndicator.indicator.x
        y: searchIndicator.indicator.y
        useNinePatchImage: false
    }

    searchIndicator.indicator: Item {
        y: control.topPadding + (control.availableHeight - height) / 2
        implicitWidth: search.width
        implicitHeight: search.height

        readonly property bool __ignoreNotCustomizable: true
    }

    NativeStyle.SearchField {
        id: clear
        visible: control.__nativeClearIndicator
        control: control
        subControl: NativeStyle.SearchField.Clear
        x: clearIndicator.indicator.x
        y: clearIndicator.indicator.y
        useNinePatchImage: false
    }

    clearIndicator.indicator: Item {
        x: control.width - width - 5
        y: control.topPadding + (control.availableHeight - height) / 2
        implicitWidth: clear.width
        implicitHeight: clear.height

        readonly property bool __ignoreNotCustomizable: true
    }
}
