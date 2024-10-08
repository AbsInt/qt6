// Copyright (C) 2015 Olivier Goffart <ogoffart@woboq.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only


#include <QTest>

#include <QtCore/qobject.h>
#include <QtCore/qmetaobject.h>

class tst_QMetaEnum : public QObject
{
    Q_OBJECT
public:
    enum SuperEnum { SuperValue1 = 1, SuperValue2 = INT_MIN };
    enum Flag { Flag1 = 1, Flag2 = INT_MIN };
    Q_DECLARE_FLAGS(Flags, Flag)
    Q_ENUM(SuperEnum)
    Q_FLAG(Flags)

private slots:
    void fromType();
    void valuesToKeys_data();
    void valuesToKeys();
    void defaultConstructed();
};

void tst_QMetaEnum::fromType()
{
    QMetaEnum meta = QMetaEnum::fromType<SuperEnum>();
    QVERIFY(meta.isValid());
    QVERIFY(!meta.isFlag());
    QCOMPARE(meta.name(), "SuperEnum");
    QCOMPARE(meta.enumName(), "SuperEnum");
    QCOMPARE(meta.enclosingMetaObject(), &staticMetaObject);
    QCOMPARE(meta.keyCount(), 2);

    meta = QMetaEnum::fromType<Flags>();
    QVERIFY(meta.isValid());
    QVERIFY(meta.isFlag());
    QCOMPARE(meta.name(), "Flags");
    QCOMPARE(meta.enumName(), "Flag");
    QCOMPARE(meta.enclosingMetaObject(), &staticMetaObject);
    QCOMPARE(meta.keyCount(), 2);
}

Q_DECLARE_METATYPE(Qt::WindowFlags)

void tst_QMetaEnum::valuesToKeys_data()
{
   QTest::addColumn<QMetaEnum>("me");
   QTest::addColumn<quint64>("flags");
   QTest::addColumn<QByteArray>("expected");

   QTest::newRow("Window")
       << QMetaEnum::fromType<Qt::WindowFlags>()
       << quint64(Qt::Window)
       << QByteArrayLiteral("Window");

   // Verify that Qt::Dialog does not cause 'Window' to appear in the output.
   QTest::newRow("Frameless_Dialog")
       << QMetaEnum::fromType<Qt::WindowFlags>()
       << quint64(Qt::Dialog | Qt::FramelessWindowHint)
       << QByteArrayLiteral("Dialog|FramelessWindowHint");

   // Similarly, Qt::WindowMinMaxButtonsHint should not show up as
   // WindowMinimizeButtonHint|WindowMaximizeButtonHint
   QTest::newRow("Tool_MinMax_StaysOnTop")
       << QMetaEnum::fromType<Qt::WindowFlags>()
       << quint64(Qt::Tool | Qt::WindowMinMaxButtonsHint | Qt::WindowStaysOnTopHint)
       << QByteArrayLiteral("Tool|WindowMinMaxButtonsHint|WindowStaysOnTopHint");

   QTest::newRow("INT_MIN")
           << QMetaEnum::fromType<Flags>()
           << quint64(uint(Flag2))
           << QByteArrayLiteral("Flag2");
}

void tst_QMetaEnum::valuesToKeys()
{
    QFETCH(QMetaEnum, me);
    QFETCH(quint64, flags);
    QFETCH(QByteArray, expected);

    QCOMPARE(me.valueToKeys(flags), expected);
    bool ok = false;
    QCOMPARE(uint(me.keysToValue(expected, &ok)), flags);
    QVERIFY(ok);
}

void tst_QMetaEnum::defaultConstructed()
{
    QMetaEnum e;
    QVERIFY(!e.isValid());
    QVERIFY(!e.isScoped());
    QVERIFY(!e.isFlag());
    QCOMPARE(e.name(), QByteArray());
}

static_assert(QtPrivate::IsQEnumHelper<tst_QMetaEnum::SuperEnum>::Value);
static_assert(QtPrivate::IsQEnumHelper<Qt::WindowFlags>::Value);
static_assert(QtPrivate::IsQEnumHelper<Qt::Orientation>::Value);
static_assert(!QtPrivate::IsQEnumHelper<int>::Value);
static_assert(!QtPrivate::IsQEnumHelper<QObject>::Value);
static_assert(!QtPrivate::IsQEnumHelper<QObject*>::Value);
static_assert(!QtPrivate::IsQEnumHelper<void>::Value);

QTEST_MAIN(tst_QMetaEnum)
#include "tst_qmetaenum.moc"
