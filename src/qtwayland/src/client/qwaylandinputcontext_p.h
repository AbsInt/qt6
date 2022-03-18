/****************************************************************************
**
** Copyright (C) 2016 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWaylandClient module of the Qt Toolkit.
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


#ifndef QWAYLANDINPUTCONTEXT_H
#define QWAYLANDINPUTCONTEXT_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <qpa/qplatforminputcontext.h>

#include <QPointer>

#include "qwaylandtextinputinterface_p.h"
#include <qtwaylandclientglobal_p.h>
#if QT_CONFIG(xkbcommon)
#include <xkbcommon/xkbcommon-compose.h>
#endif

struct wl_callback;
struct wl_callback_listener;

QT_BEGIN_NAMESPACE

namespace QtWaylandClient {

class QWaylandDisplay;

class QWaylandInputContext : public QPlatformInputContext
{
    Q_OBJECT
public:
    explicit QWaylandInputContext(QWaylandDisplay *display);
    ~QWaylandInputContext() override;

    bool isValid() const override;

    void reset() override;
    void commit() override;
    void update(Qt::InputMethodQueries) override;

    void invokeAction(QInputMethod::Action, int cursorPosition) override;

    void showInputPanel() override;
    void hideInputPanel() override;
    bool isInputPanelVisible() const override;
    QRectF keyboardRect() const override;

    QLocale locale() const override;
    Qt::LayoutDirection inputDirection() const override;

    void setFocusObject(QObject *object) override;

#if QT_CONFIG(xkbcommon)
    bool filterEvent(const QEvent *event) override;

    // This invokable is called from QXkbCommon::setXkbContext().
    Q_INVOKABLE void setXkbContext(struct xkb_context *context) { m_XkbContext = context; }
#endif

private:
    QWaylandTextInputInterface *textInput() const;

    QWaylandDisplay *mDisplay = nullptr;
    QPointer<QWindow> mCurrentWindow;

#if QT_CONFIG(xkbcommon)
    void ensureInitialized();

    bool m_initialized = false;
    QObject *m_focusObject = nullptr;
    xkb_compose_table *m_composeTable = nullptr;
    xkb_compose_state *m_composeState = nullptr;
    struct xkb_context *m_XkbContext = nullptr;
#endif
};

}

QT_END_NAMESPACE

#endif // QWAYLANDINPUTCONTEXT_H
