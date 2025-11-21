// Copyright (C) 2021 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only WITH Qt-GPL-exception-1.0
// Qt-Security score:significant

#ifndef QQMLJSANNOTATION_P_H
#define QQMLJSANNOTATION_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists purely as an
// implementation detail.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.

#include <QtCore/qglobal.h>
#include <QtCore/qhash.h>

#include <variant>

QT_BEGIN_NAMESPACE

QT_ENABLE_P0846_SEMANTICS_FOR(get_if)

struct QQQmlJSDeprecation
{
    QString reason;
};

struct QQmlJSAnnotation
{
    using Value = std::variant<QString, double>;

    QString name;
    QHash<QString, Value> bindings;

    bool isDeprecation() const;
    QQQmlJSDeprecation deprecation() const;

    friend bool operator==(const QQmlJSAnnotation &a, const QQmlJSAnnotation &b) noexcept
    {
        return a.name == b.name &&
               a.bindings == b.bindings;
    }

    friend bool operator!=(const QQmlJSAnnotation &a, const QQmlJSAnnotation &b) noexcept
    {
        return !(a == b);
    }

    friend size_t qHash(const QQmlJSAnnotation &annotation, size_t seed = 0) noexcept
    {
        QtPrivate::QHashCombine combine(seed);
        seed = combine(seed, annotation.name);

        for (auto it = annotation.bindings.constBegin(); it != annotation.bindings.constEnd(); ++it) {
            size_t h = combine(seed, it.key());
            // use + to keep the result independent of the ordering of the keys

            const auto &var = it.value();

            if (var.valueless_by_exception())
                continue;

            if (auto v = get_if<double>(&var))
                seed += combine(h, *v);
            else if (auto v = get_if<QString>(&var))
                seed += combine(h, *v);
            else
                Q_UNREACHABLE();
        }

        return seed;
    }
};

QT_END_NAMESPACE

#endif // QQMLJSANNOTATION_P_H
