// Copyright (C) 2025 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com, author Giuseppe D'Angelo <giuseppe.dangelo@kdab.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtCore/qxptype_traits.h>

#include <QTest>

class tst_qxp_is_detected : public QObject
{
    Q_OBJECT
};

// ALl major compilers have bugs regarding handling accessibility
// from SFINAE contexts; skip tests relying on it.

namespace InnerTypedefTest {

template <typename T>
using HasInnerFooTypedefTest = typename T::Foo;

struct A {};
struct B { using Foo = void; };
class C { using Foo = void; }; // inaccessible
struct D { int Foo; };
struct E { int Foo() const; };
struct F { static void Foo(); };

static_assert(!qxp::is_detected_v<HasInnerFooTypedefTest, A>);
static_assert( qxp::is_detected_v<HasInnerFooTypedefTest, B>);
// static_assert(!qxp::is_detected_v<HasInnerFooTypedefTest, C>); // see above
static_assert(!qxp::is_detected_v<HasInnerFooTypedefTest, D>);
static_assert(!qxp::is_detected_v<HasInnerFooTypedefTest, E>);
static_assert(!qxp::is_detected_v<HasInnerFooTypedefTest, F>);

} // InnerTypedefTest

namespace ReflectionTest {

template <typename T>
using HasPublicConstFooFunctionTest = decltype(std::declval<const T &>().foo());

struct A {};
struct B { void foo(); };
struct C { void foo() const; };
struct D { void foo(int) const; };
struct E { void foo(int = 42) const; };
struct F { void foo() const &&; };
struct G { int foo; };
class H { void foo(); };
class I { void foo() const; };

static_assert(!qxp::is_detected_v<HasPublicConstFooFunctionTest, A>);
static_assert(!qxp::is_detected_v<HasPublicConstFooFunctionTest, B>);
static_assert( qxp::is_detected_v<HasPublicConstFooFunctionTest, C>);
static_assert(!qxp::is_detected_v<HasPublicConstFooFunctionTest, D>);
static_assert( qxp::is_detected_v<HasPublicConstFooFunctionTest, E>);
static_assert(!qxp::is_detected_v<HasPublicConstFooFunctionTest, F>);
static_assert(!qxp::is_detected_v<HasPublicConstFooFunctionTest, G>);
// static_assert(!qxp::is_detected_v<HasPublicConstFooFunctionTest, H>); // see above
// static_assert(!qxp::is_detected_v<HasPublicConstFooFunctionTest, I>); // see above

} // ReflectionTest

namespace InnerTypedefTestFriend
{

struct Helper
{
    template <typename T>
    using HasInnerFooTypedefTest = typename T::Foo;
};

struct A {};
struct B { using Foo = void; };
class C { using Foo = void; }; // inaccessible
class D { friend struct Helper; using Foo = void; };

static_assert(!qxp::is_detected_v<Helper::HasInnerFooTypedefTest, A>);
static_assert( qxp::is_detected_v<Helper::HasInnerFooTypedefTest, B>);
// static_assert(!qxp::is_detected_v<Helper::HasInnerFooTypedefTest, C>); // see above
// static_assert(!qxp::is_detected_v<Helper::HasInnerFooTypedefTest, D>); // see above
}

QTEST_APPLESS_MAIN(tst_qxp_is_detected);

#include "tst_is_detected.moc"
