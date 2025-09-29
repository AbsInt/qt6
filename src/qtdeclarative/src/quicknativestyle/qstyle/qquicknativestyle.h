// Copyright (C) 2020 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QQUICKNATIVESTYLE_H
#define QQUICKNATIVESTYLE_H

#include "qquickstyle.h"

#include <memory>

QT_BEGIN_NAMESPACE

namespace QQC2 {

class QQuickNativeStyle
{
public:
    static void setStyle(QStyle *style) { s_style.reset(style); }
    static QStyle *style() { return s_style.get(); }

private:
    static inline std::unique_ptr<QStyle> s_style;
};

} // namespace QQC2

QT_END_NAMESPACE

#endif // QQUICKNATIVESTYLE_H
