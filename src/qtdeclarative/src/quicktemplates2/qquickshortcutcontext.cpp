/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt Quick Templates 2 module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qquickshortcutcontext_p_p.h"
#include "qquickoverlay_p_p.h"
#include "qquicktooltip_p.h"
#include "qquickmenu_p.h"
#include "qquickmenu_p_p.h"
#include "qquickpopup_p.h"

#include <QtCore/qloggingcategory.h>
#include <QtGui/qguiapplication.h>
#include <QtQuick/qquickrendercontrol.h>

QT_BEGIN_NAMESPACE

Q_LOGGING_CATEGORY(lcContextMatcher, "qt.quick.controls.shortcutcontext.matcher")

static bool isBlockedByPopup(QQuickItem *item)
{
    if (!item || !item->window())
        return false;

    QQuickOverlay *overlay = QQuickOverlay::overlay(item->window());
    const auto popups = QQuickOverlayPrivate::get(overlay)->stackingOrderPopups();
    for (QQuickPopup *popup : popups) {
        if (qobject_cast<QQuickToolTip *>(popup))
            continue; // ignore tooltips (QTBUG-60492)
        if (popup->isModal() || popup->closePolicy() & QQuickPopup::CloseOnEscape) {
            return item != popup->popupItem() && !popup->popupItem()->isAncestorOf(item);
        }
    }

    return false;
}

bool QQuickShortcutContext::matcher(QObject *obj, Qt::ShortcutContext context)
{
    QQuickItem *item = nullptr;
    switch (context) {
    case Qt::ApplicationShortcut:
        return true;
    case Qt::WindowShortcut:
        while (obj && !obj->isWindowType()) {
            item = qobject_cast<QQuickItem *>(obj);
            if (item && item->window()) {
                obj = item->window();
                break;
            } else if (QQuickPopup *popup = qobject_cast<QQuickPopup *>(obj)) {
                obj = popup->window();
                item = popup->popupItem();

                if (!obj) {
                    // The popup has no associated window (yet). However, sub-menus,
                    // unlike top-level menus, will not have an associated window
                    // until their parent menu is opened. So, check if this is a sub-menu
                    // so that actions within it can grab shortcuts.
                    if (auto *menu = qobject_cast<QQuickMenu *>(popup)) {
                        auto parentMenu = QQuickMenuPrivate::get(menu)->parentMenu;
                        while (!obj && parentMenu)
                            obj = parentMenu->window();
                    }
                }
                break;
            }
            obj = obj->parent();
        }
        if (QWindow *renderWindow = QQuickRenderControl::renderWindowFor(qobject_cast<QQuickWindow *>(obj)))
            obj = renderWindow;
        qCDebug(lcContextMatcher) << "obj" << obj << "focusWindow" << QGuiApplication::focusWindow()
            << "!isBlockedByPopup(item)" << !isBlockedByPopup(item);
        return obj && obj == QGuiApplication::focusWindow() && !isBlockedByPopup(item);
    default:
        return false;
    }
}

QT_END_NAMESPACE
