// Copyright (C) 2020 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QJSNUMBERCOERCION_H
#define QJSNUMBERCOERCION_H

#include <QtCore/qglobal.h>
#include <cstring>

QT_BEGIN_NAMESPACE

class QJSNumberCoercion
{
public:
    static constexpr bool isInteger(double d) {
        return equals(d, d) && equals(static_cast<int>(d), d);
    }

    static constexpr int toInteger(double d) {
        if (!equals(d, d))
            return 0;

        const int i = static_cast<int>(d);
        if (equals(i, d))
            return i;

        return QJSNumberCoercion(d).toInteger();
    }

    static constexpr bool equals(double lhs, double rhs)
    {
        QT_WARNING_PUSH
        QT_WARNING_DISABLE_FLOAT_COMPARE
        return lhs == rhs;
        QT_WARNING_POP
    }

private:
    constexpr QJSNumberCoercion(double dbl)
    {
        // the dbl == 0 path is guaranteed constexpr. The other one may or may not be, depending
        // on whether and how the compiler inlines the memcpy.
        // In order to declare the ctor constexpr we need one guaranteed constexpr path.
        if (!equals(dbl, 0))
            memcpy(&d, &dbl, sizeof(double));
    }

    constexpr int sign() const
    {
        return (d >> 63) ? -1 : 1;
    }

    constexpr bool isDenormal() const
    {
        return static_cast<int>((d << 1) >> 53) == 0;
    }

    constexpr int exponent() const
    {
        return static_cast<int>((d << 1) >> 53) - 1023;
    }

    constexpr quint64 significant() const
    {
        quint64 m = (d << 12) >> 12;
        if (!isDenormal())
            m |= (static_cast<quint64>(1) << 52);
        return m;
    }

    constexpr int toInteger()
    {
        int e = exponent() - 52;
        if (e < 0) {
            if (e <= -53)
                return 0;
            return sign() * static_cast<int>(significant() >> -e);
        } else {
            if (e > 31)
                return 0;
            return sign() * (static_cast<int>(significant()) << e);
        }
    }

    quint64 d = 0;
};

QT_END_NAMESPACE

#endif // QJSNUMBERCOERCION_H
