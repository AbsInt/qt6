// Copyright (C) 2024 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef QMLUSING_H
#define QMLUSING_H

#include <QtQml/qqml.h>

#include <QtCore/qendian.h>
#include <QtCore/qobject.h>

template<typename T, typename tag>
struct TransparentWrapper
{
    TransparentWrapper(T t_ = T()) : t(t_) {
        static const bool registered = []() {
            return QMetaType::registerConverter<TransparentWrapper, T>(toType)
                    && QMetaType::registerConverter<T, TransparentWrapper>(fromType);
        }();
        Q_ASSERT(registered);
    }

    static T toType(TransparentWrapper t) { return t.t; }
    static TransparentWrapper fromType(T t) { return TransparentWrapper(t); }

    T t;
    operator T &() { return t; }
    operator T() const { return t; }
    TransparentWrapper &operator=(T t_)
    {
        t = t_;
        return *this;
    }
};

using myInt32 = TransparentWrapper<int32_t, struct int_tag>;

struct MyInt32Foreign
{
    Q_GADGET
    QML_FOREIGN(myInt32)
    QML_USING(int32_t)
};

class UsingUserValue
{
    Q_GADGET
    QML_VALUE_TYPE(usingUserValue)
    Q_PROPERTY(myInt32 a READ a WRITE setA)
public:
    myInt32 a() const { return m_a; }
    void setA(myInt32 a) { m_a = a; }

    Q_INVOKABLE myInt32 getB() const { return m_b; }

    Q_INVOKABLE void setB(myInt32 b) { m_b = b; }
    Q_INVOKABLE void setB(const QString &) { m_b = 99; }

private:
    friend bool operator==(const UsingUserValue &lhs, const UsingUserValue &rhs)
    {
        return lhs.m_a == rhs.m_a && lhs.m_b == rhs.m_b;
    }

    friend bool operator!=(const UsingUserValue &lhs, const UsingUserValue &rhs)
    {
        return !(lhs == rhs);
    }

    myInt32 m_a = 24;
    myInt32 m_b = 25;
};

class UsingUserObject : public QObject
{
    Q_OBJECT
    QML_ELEMENT
    Q_PROPERTY(myInt32 a READ a WRITE setA NOTIFY aChanged)
    Q_PROPERTY(UsingUserValue val READ val WRITE setVal NOTIFY valChanged)
public:
    UsingUserObject(QObject *parent = nullptr) : QObject(parent) {}

    myInt32 a() const { return m_a; }
    void setA(myInt32 a)
    {
        if (a == m_a)
            return;
        m_a = a;
        emit aChanged();
    }

    UsingUserValue val() const { return m_val; }
    void setVal(const UsingUserValue &val)
    {
        if (val == m_val)
            return;

        m_val = val;
        emit valChanged();
    }

    Q_INVOKABLE myInt32 getB() const { return m_b; }
    Q_INVOKABLE void setB(myInt32 b) { m_b = b; }
    Q_INVOKABLE void setB(const QString &) { m_b = 101; }

signals:
    void aChanged();
    void valChanged();

private:
    myInt32 m_a = 7;
    myInt32 m_b = 5;
    UsingUserValue m_val;
};

#endif // QMLUSING_H
