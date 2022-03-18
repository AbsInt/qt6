/****************************************************************************
**
** Copyright (C) 2017 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com
** Contact: https://www.qt.io/licensing/
**
** This file is part of the QtWaylandCompositor module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 or (at your option) any later version
** approved by the KDE Free Qt Foundation. The licenses are as published by
** the Free Software Foundation and appearing in the file LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "qwaylandinputmethodcontrol.h"
#include "qwaylandinputmethodcontrol_p.h"

#include "qwaylandcompositor.h"
#include "qwaylandseat.h"
#include "qwaylandsurface.h"
#include "qwaylandview.h"
#include "qwaylandtextinput.h"
#if QT_WAYLAND_TEXT_INPUT_V4_WIP
#include "qwaylandtextinputv4.h"
#endif // QT_WAYLAND_TEXT_INPUT_V4_WIP
#include "qwaylandqttextinputmethod.h"

#include <QtGui/QInputMethodEvent>

QWaylandInputMethodControl::QWaylandInputMethodControl(QWaylandSurface *surface)
    : QObject(*new QWaylandInputMethodControlPrivate(surface), surface)
{
    connect(d_func()->compositor, &QWaylandCompositor::defaultSeatChanged,
            this, &QWaylandInputMethodControl::defaultSeatChanged);
    QWaylandTextInput *textInput = d_func()->textInput();
    if (textInput) {
        connect(textInput, &QWaylandTextInput::surfaceEnabled, this, &QWaylandInputMethodControl::surfaceEnabled);
        connect(textInput, &QWaylandTextInput::surfaceDisabled, this, &QWaylandInputMethodControl::surfaceDisabled);
        connect(textInput, &QWaylandTextInput::updateInputMethod, this, &QWaylandInputMethodControl::updateInputMethod);
    }

#if QT_WAYLAND_TEXT_INPUT_V4_WIP
    QWaylandTextInputV4 *textInputV4 = d_func()->textInputV4();
    if (textInputV4) {
        connect(textInputV4, &QWaylandTextInputV4::surfaceEnabled, this, &QWaylandInputMethodControl::surfaceEnabled);
        connect(textInputV4, &QWaylandTextInputV4::surfaceDisabled, this, &QWaylandInputMethodControl::surfaceDisabled);
        connect(textInputV4, &QWaylandTextInputV4::updateInputMethod, this, &QWaylandInputMethodControl::updateInputMethod);
    }
#endif // QT_WAYLAND_TEXT_INPUT_V4_WIP

    QWaylandQtTextInputMethod *textInputMethod = d_func()->textInputMethod();
    if (textInputMethod) {
        connect(textInputMethod, &QWaylandQtTextInputMethod::surfaceEnabled, this, &QWaylandInputMethodControl::surfaceEnabled);
        connect(textInputMethod, &QWaylandQtTextInputMethod::surfaceDisabled, this, &QWaylandInputMethodControl::surfaceDisabled);
        connect(textInputMethod, &QWaylandQtTextInputMethod::updateInputMethod, this, &QWaylandInputMethodControl::updateInputMethod);
    }
}

QVariant QWaylandInputMethodControl::inputMethodQuery(Qt::InputMethodQuery query, QVariant argument) const
{
    Q_D(const QWaylandInputMethodControl);

    QWaylandTextInput *textInput = d->textInput();
    if (textInput != nullptr && textInput->focus() == d->surface)
        return textInput->inputMethodQuery(query, argument);

#if QT_WAYLAND_TEXT_INPUT_V4_WIP
    QWaylandTextInputV4 *textInputV4 = d->textInputV4();
    if (textInputV4 != nullptr && textInputV4->focus() == d->surface)
        return textInputV4->inputMethodQuery(query, argument);
#endif // QT_WAYLAND_TEXT_INPUT_V4_WIP

    QWaylandQtTextInputMethod *textInputMethod = d_func()->textInputMethod();
    if (textInputMethod && textInputMethod->focusedSurface() == d->surface)
        return textInputMethod->inputMethodQuery(query, argument);

    return QVariant();
}

void QWaylandInputMethodControl::inputMethodEvent(QInputMethodEvent *event)
{
    Q_D(QWaylandInputMethodControl);

    if (QWaylandTextInput *textInput = d->textInput()) {
        textInput->sendInputMethodEvent(event);
#if QT_WAYLAND_TEXT_INPUT_V4_WIP
    } else if (QWaylandTextInputV4 *textInputV4 = d->textInputV4()) {
        textInputV4->sendInputMethodEvent(event);
#endif // QT_WAYLAND_TEXT_INPUT_V4_WIP
    } else if (QWaylandQtTextInputMethod *textInputMethod = d->textInputMethod()) {
        textInputMethod->sendInputMethodEvent(event);
    } else {
        event->ignore();
    }
}

bool QWaylandInputMethodControl::enabled() const
{
    Q_D(const QWaylandInputMethodControl);

    return d->enabled;
}

void QWaylandInputMethodControl::setEnabled(bool enabled)
{
    Q_D(QWaylandInputMethodControl);

    if (d->enabled == enabled)
        return;

    d->enabled = enabled;
    emit enabledChanged(enabled);
    emit updateInputMethod(Qt::ImQueryInput);
}

void QWaylandInputMethodControl::surfaceEnabled(QWaylandSurface *surface)
{
    Q_D(QWaylandInputMethodControl);

    if (surface == d->surface)
        setEnabled(true);
}

void QWaylandInputMethodControl::surfaceDisabled(QWaylandSurface *surface)
{
    Q_D(QWaylandInputMethodControl);

    if (surface == d->surface)
        setEnabled(false);
}

void QWaylandInputMethodControl::setSurface(QWaylandSurface *surface)
{
    Q_D(QWaylandInputMethodControl);

    if (d->surface == surface)
        return;

    d->surface = surface;

    QWaylandTextInput *textInput = d->textInput();
#if QT_WAYLAND_TEXT_INPUT_V4_WIP
    QWaylandTextInputV4 *textInputV4 = d->textInputV4();
#endif // QT_WAYLAND_TEXT_INPUT_V4_WIP
    QWaylandQtTextInputMethod *textInputMethod = d->textInputMethod();
    setEnabled((textInput && textInput->isSurfaceEnabled(d->surface))
#if QT_WAYLAND_TEXT_INPUT_V4_WIP
               || (textInputV4 && textInputV4->isSurfaceEnabled(d->surface))
#endif // QT_WAYLAND_TEXT_INPUT_V4_WIP
               || (textInputMethod && textInputMethod->isSurfaceEnabled(d->surface)));
}

void QWaylandInputMethodControl::defaultSeatChanged()
{
    Q_D(QWaylandInputMethodControl);

    disconnect(d->textInput(), nullptr, this, nullptr);
#if QT_WAYLAND_TEXT_INPUT_V4_WIP
    disconnect(d->textInputV4(), nullptr, this, nullptr);
#endif // QT_WAYLAND_TEXT_INPUT_V4_WIP
    disconnect(d->textInputMethod(), nullptr, this, nullptr);

    d->seat = d->compositor->defaultSeat();
    QWaylandTextInput *textInput = d->textInput();
#if QT_WAYLAND_TEXT_INPUT_V4_WIP
    QWaylandTextInputV4 *textInputV4 = d->textInputV4();
#endif // QT_WAYLAND_TEXT_INPUT_V4_WIP
    QWaylandQtTextInputMethod *textInputMethod = d->textInputMethod();

    if (textInput) {
        connect(textInput, &QWaylandTextInput::surfaceEnabled, this, &QWaylandInputMethodControl::surfaceEnabled);
        connect(textInput, &QWaylandTextInput::surfaceDisabled, this, &QWaylandInputMethodControl::surfaceDisabled);
    }

#if QT_WAYLAND_TEXT_INPUT_V4_WIP
    if (textInputV4) {
        connect(textInputV4, &QWaylandTextInputV4::surfaceEnabled, this, &QWaylandInputMethodControl::surfaceEnabled);
        connect(textInputV4, &QWaylandTextInputV4::surfaceDisabled, this, &QWaylandInputMethodControl::surfaceDisabled);
    }
#endif // QT_WAYLAND_TEXT_INPUT_V4_WIP

    if (textInputMethod) {
        connect(textInputMethod, &QWaylandQtTextInputMethod::surfaceEnabled, this, &QWaylandInputMethodControl::surfaceEnabled);
        connect(textInputMethod, &QWaylandQtTextInputMethod::surfaceDisabled, this, &QWaylandInputMethodControl::surfaceDisabled);
    }

    setEnabled((textInput && textInput->isSurfaceEnabled(d->surface))
#if QT_WAYLAND_TEXT_INPUT_V4_WIP
               || (textInputV4 && textInputV4->isSurfaceEnabled(d->surface))
#endif // QT_WAYLAND_TEXT_INPUT_V4_WIP
               || (textInputMethod && textInputMethod->isSurfaceEnabled(d->surface)));
}

QWaylandInputMethodControlPrivate::QWaylandInputMethodControlPrivate(QWaylandSurface *surface)
    : compositor(surface->compositor())
    , seat(compositor->defaultSeat())
    , surface(surface)
{
}

QWaylandQtTextInputMethod *QWaylandInputMethodControlPrivate::textInputMethod() const
{
    if (!surface->client()->textInputProtocols().testFlag(QWaylandClient::TextInputProtocol::QtTextInputMethodV1))
        return nullptr;
    return QWaylandQtTextInputMethod::findIn(seat);
}

QWaylandTextInput *QWaylandInputMethodControlPrivate::textInput() const
{
    if (!surface->client()->textInputProtocols().testFlag(QWaylandClient::TextInputProtocol::TextInputV2))
        return nullptr;
    return QWaylandTextInput::findIn(seat);
}

#if QT_WAYLAND_TEXT_INPUT_V4_WIP
QWaylandTextInputV4 *QWaylandInputMethodControlPrivate::textInputV4() const
{
    return QWaylandTextInputV4::findIn(seat);
}
#endif // QT_WAYLAND_TEXT_INPUT_V4_WIP

#include "moc_qwaylandinputmethodcontrol.cpp"
