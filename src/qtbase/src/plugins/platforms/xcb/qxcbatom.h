// Copyright (C) 2018 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QXCBATOM_H
#define QXCBATOM_H

#include <xcb/xcb.h>

class QXcbAtom
{
public:
    enum Atom {
        // window-manager <-> client protocols
        WM_PROTOCOLS,
        WM_DELETE_WINDOW,
        WM_TAKE_FOCUS,
        _NET_WM_PING,
        _NET_WM_CONTEXT_HELP,
        _NET_WM_SYNC_REQUEST,
        _NET_WM_SYNC_REQUEST_COUNTER,
        MANAGER, // System tray notification
        _NET_SYSTEM_TRAY_OPCODE, // System tray operation

        // ICCCM window state
        WM_STATE,
        WM_CHANGE_STATE,
        WM_CLASS,
        WM_NAME,

        // Session management
        WM_CLIENT_LEADER,
        WM_WINDOW_ROLE,
        SM_CLIENT_ID,
        WM_CLIENT_MACHINE,

        // Clipboard
        CLIPBOARD,
        INCR,
        TARGETS,
        MULTIPLE,
        TIMESTAMP,
        SAVE_TARGETS,
        CLIP_TEMPORARY,
        _QT_SELECTION,
        _QT_CLIPBOARD_SENTINEL,
        _QT_SELECTION_SENTINEL,
        CLIPBOARD_MANAGER,

        RESOURCE_MANAGER,

        _XSETROOT_ID,

        _QT_SCROLL_DONE,
        _QT_INPUT_ENCODING,

        // Qt/XCB specific
        _QT_CLOSE_CONNECTION,

        _MOTIF_WM_HINTS,

        DTWM_IS_RUNNING,
        ENLIGHTENMENT_DESKTOP,
        _DT_SAVE_MODE,
        _SGI_DESKS_MANAGER,

        // EWMH (aka NETWM)
        _NET_SUPPORTED,
        _NET_VIRTUAL_ROOTS,
        _NET_WORKAREA,

        _NET_MOVERESIZE_WINDOW,
        _NET_WM_MOVERESIZE,

        _NET_WM_NAME,
        _NET_WM_ICON_NAME,
        _NET_WM_ICON,

        _NET_WM_PID,

        _NET_WM_WINDOW_OPACITY,

        _NET_WM_STATE,
        _NET_WM_STATE_ABOVE,
        _NET_WM_STATE_BELOW,
        _NET_WM_STATE_FULLSCREEN,
        _NET_WM_STATE_MAXIMIZED_HORZ,
        _NET_WM_STATE_MAXIMIZED_VERT,
        _NET_WM_STATE_MODAL,
        _NET_WM_STATE_STAYS_ON_TOP,
        _NET_WM_STATE_DEMANDS_ATTENTION,
        _NET_WM_STATE_HIDDEN,

        _NET_WM_USER_TIME,
        _NET_WM_USER_TIME_WINDOW,
        _NET_WM_FULL_PLACEMENT,

        _NET_WM_WINDOW_TYPE,
        _NET_WM_WINDOW_TYPE_DESKTOP,
        _NET_WM_WINDOW_TYPE_DOCK,
        _NET_WM_WINDOW_TYPE_TOOLBAR,
        _NET_WM_WINDOW_TYPE_MENU,
        _NET_WM_WINDOW_TYPE_UTILITY,
        _NET_WM_WINDOW_TYPE_SPLASH,
        _NET_WM_WINDOW_TYPE_DIALOG,
        _NET_WM_WINDOW_TYPE_DROPDOWN_MENU,
        _NET_WM_WINDOW_TYPE_POPUP_MENU,
        _NET_WM_WINDOW_TYPE_TOOLTIP,
        _NET_WM_WINDOW_TYPE_NOTIFICATION,
        _NET_WM_WINDOW_TYPE_COMBO,
        _NET_WM_WINDOW_TYPE_DND,
        _NET_WM_WINDOW_TYPE_NORMAL,
        _KDE_NET_WM_WINDOW_TYPE_OVERRIDE,

        _KDE_NET_WM_FRAME_STRUT,
        _NET_FRAME_EXTENTS,

        _NET_STARTUP_INFO,
        _NET_STARTUP_INFO_BEGIN,
        _NET_STARTUP_ID,

        _NET_SUPPORTING_WM_CHECK,

        _NET_WM_CM_S0,

        _NET_SYSTEM_TRAY_VISUAL,

        _NET_ACTIVE_WINDOW,

        // Property formats
        TEXT,
        UTF8_STRING,
        CARDINAL,

        // Xdnd
        XdndEnter,
        XdndPosition,
        XdndStatus,
        XdndLeave,
        XdndDrop,
        XdndFinished,
        XdndTypelist,
        XdndActionList,

        XdndSelection,

        XdndAware,
        XdndProxy,

        XdndActionCopy,
        XdndActionLink,
        XdndActionMove,
        XdndActionAsk,
        XdndActionPrivate,

        // Xkb
        _XKB_RULES_NAMES,

        // XEMBED
        _XEMBED,
        _XEMBED_INFO,

        // XInput2
        ButtonLeft,
        ButtonMiddle,
        ButtonRight,
        ButtonWheelUp,
        ButtonWheelDown,
        ButtonHorizWheelLeft,
        ButtonHorizWheelRight,
        AbsMTPositionX,
        AbsMTPositionY,
        AbsMTTouchMajor,
        AbsMTTouchMinor,
        AbsMTOrientation,
        AbsMTPressure,
        AbsMTTrackingID,
        MaxContacts,
        RelX,
        RelY,
        // XInput2 tablet
        AbsX,
        AbsY,
        AbsPressure,
        AbsTiltX,
        AbsTiltY,
        AbsWheel,
        AbsDistance,
        WacomSerialIDs,
        INTEGER,
        RelHorizWheel,
        RelVertWheel,
        RelHorizScroll,
        RelVertScroll,

        _XSETTINGS_SETTINGS,

        _COMPIZ_DECOR_PENDING,
        _COMPIZ_DECOR_REQUEST,
        _COMPIZ_DECOR_DELETE_PIXMAP,
        _COMPIZ_TOOLKIT_ACTION,
        _GTK_LOAD_ICONTHEMES,

        AT_SPI_BUS,

        EDID,
        EDID_DATA,
        XFree86_DDC_EDID1_RAWDATA,

        _ICC_PROFILE,

        NAtoms
    };

    QXcbAtom();
    void initialize(xcb_connection_t *connection);

    inline xcb_atom_t atom(QXcbAtom::Atom atom) const { return m_allAtoms[atom]; }
    QXcbAtom::Atom qatom(xcb_atom_t atom) const;

protected:
    void initializeAllAtoms(xcb_connection_t *connection);

private:
    xcb_atom_t m_allAtoms[QXcbAtom::NAtoms];
};

#endif // QXCBATOM_H
