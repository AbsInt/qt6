// Copyright (C) 2012 Klarälvdalens Datakonsult AB, a KDAB Group company, info@kdab.com, author Marc Mutz <marc.mutz@kdab.com>
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only
#ifndef TESTS_AUTO_CORELIB_TOOLS_MOC_CXX11_EXPLICIT_OVERRIDE_CONTROL_H
#define TESTS_AUTO_CORELIB_TOOLS_MOC_CXX11_EXPLICIT_OVERRIDE_CONTROL_H

#include <QtCore/QObject>

#ifndef Q_MOC_RUN // hide from moc
# define override
# define final
# define sealed
#endif

QT_WARNING_PUSH
QT_WARNING_DISABLE_GCC("-Wsuggest-override")

class ExplicitOverrideControlBase : public QObject
{
    Q_OBJECT
public:
    explicit ExplicitOverrideControlBase(QObject *parent = nullptr)
        : QObject(parent) {}

private Q_SLOTS:
    virtual void pureSlot0() = 0;
    virtual void pureSlot1() = 0;
    virtual void pureSlot2() const = 0;
    virtual void pureSlot3() const = 0;
#if 0 // moc doesn't support volatile slots
    virtual void pureSlot4() volatile = 0;
    virtual void pureSlot5() volatile = 0;
    virtual void pureSlot6() const volatile = 0;
    virtual void pureSlot7() volatile const = 0;
    virtual void pureSlot8() const volatile = 0;
    virtual void pureSlot9() volatile const = 0;
#endif
};

class ExplicitOverrideControlFinalQt : public ExplicitOverrideControlBase
{
    Q_OBJECT
public:
    explicit ExplicitOverrideControlFinalQt(QObject *parent = nullptr)
        : ExplicitOverrideControlBase(parent) {}

private Q_SLOTS:
    void pureSlot0() Q_DECL_FINAL {}
    void pureSlot1() Q_DECL_FINAL {}
    void pureSlot2() const Q_DECL_FINAL {}
    void pureSlot3() Q_DECL_FINAL const {}
#if 0 // moc doesn't support volatile slots
    void pureSlot4() volatile Q_DECL_FINAL {}
    void pureSlot5() Q_DECL_FINAL volatile {}
    void pureSlot6() const volatile Q_DECL_FINAL {}
    void pureSlot7() volatile Q_DECL_FINAL const {}
    void pureSlot8() const Q_DECL_FINAL volatile {}
    void pureSlot9() Q_DECL_FINAL volatile const {}
#endif
};

class ExplicitOverrideControlFinalCxx11 : public ExplicitOverrideControlBase
{
    Q_OBJECT
public:
    explicit ExplicitOverrideControlFinalCxx11(QObject *parent = nullptr)
        : ExplicitOverrideControlBase(parent) {}

private Q_SLOTS:
    void pureSlot0() final {}
    void pureSlot1() final {}
    void pureSlot2() const final {}
    void pureSlot3() final const {}
#if 0 // moc doesn't support volatile slots
    void pureSlot4() volatile final {}
    void pureSlot5() final volatile {}
    void pureSlot6() const volatile final {}
    void pureSlot7() volatile final const {}
    void pureSlot8() const final volatile {}
    void pureSlot9() final volatile const {}
#endif
};

class ExplicitOverrideControlSealed : public ExplicitOverrideControlBase
{
    Q_OBJECT
public:
    explicit ExplicitOverrideControlSealed(QObject *parent = nullptr)
        : ExplicitOverrideControlBase(parent) {}

private Q_SLOTS:
    void pureSlot0() sealed {}
    void pureSlot1() sealed {}
    void pureSlot2() const sealed {}
    void pureSlot3() sealed const {}
#if 0 // moc doesn't support volatile slots
    void pureSlot4() volatile sealed {}
    void pureSlot5() sealed volatile {}
    void pureSlot6() const volatile sealed {}
    void pureSlot7() volatile sealed const {}
    void pureSlot8() const sealed volatile {}
    void pureSlot9() sealed volatile const {}
#endif
};

class ExplicitOverrideControlOverrideQt : public ExplicitOverrideControlBase
{
    Q_OBJECT
public:
    explicit ExplicitOverrideControlOverrideQt(QObject *parent = nullptr)
        : ExplicitOverrideControlBase(parent) {}

private Q_SLOTS:
    void pureSlot0() override {}
    void pureSlot1() override {}
    void pureSlot2() const override {}
    void pureSlot3() override const {}
#if 0 // moc doesn't support volatile slots
    void pureSlot4() volatile override {}
    void pureSlot5() override volatile {}
    void pureSlot6() const volatile override {}
    void pureSlot7() volatile override const {}
    void pureSlot8() const override volatile {}
    void pureSlot9() override volatile const {}
#endif
};

class ExplicitOverrideControlOverrideCxx11 : public ExplicitOverrideControlBase
{
    Q_OBJECT
public:
    explicit ExplicitOverrideControlOverrideCxx11(QObject *parent = nullptr)
        : ExplicitOverrideControlBase(parent) {}

private Q_SLOTS:
    void pureSlot0() override {}
    void pureSlot1() override {}
    void pureSlot2() const override {}
    void pureSlot3() override const {}
#if 0 // moc doesn't support volatile slots
    void pureSlot4() volatile override {}
    void pureSlot5() override volatile {}
    void pureSlot6() const volatile override {}
    void pureSlot7() volatile override const {}
    void pureSlot8() const override volatile {}
    void pureSlot9() override volatile const {}
#endif
};

class ExplicitOverrideControlFinalQtOverrideQt : public ExplicitOverrideControlBase
{
    Q_OBJECT
public:
    explicit ExplicitOverrideControlFinalQtOverrideQt(QObject *parent = nullptr)
        : ExplicitOverrideControlBase(parent) {}

private Q_SLOTS:
    void pureSlot0() Q_DECL_FINAL override {}
    void pureSlot1() override Q_DECL_FINAL {}
    void pureSlot2() override const Q_DECL_FINAL {}
    void pureSlot3() Q_DECL_FINAL const override {}
#if 0 // moc doesn't support volatile slots
    void pureSlot4() volatile Q_DECL_FINAL override {}
    void pureSlot5() override Q_DECL_FINAL volatile {}
    void pureSlot6() override const volatile Q_DECL_FINAL {}
    void pureSlot7() volatile override Q_DECL_FINAL const {}
    void pureSlot8() const Q_DECL_FINAL override volatile {}
    void pureSlot9() Q_DECL_FINAL volatile const override {}
#endif
};

class ExplicitOverrideControlFinalCxx11OverrideCxx11 : public ExplicitOverrideControlBase
{
    Q_OBJECT
public:
    explicit ExplicitOverrideControlFinalCxx11OverrideCxx11(QObject *parent = nullptr)
        : ExplicitOverrideControlBase(parent) {}

private Q_SLOTS:
    void pureSlot0() final override {}
    void pureSlot1() override final {}
    void pureSlot2() override const final {}
    void pureSlot3() final const override {}
#if 0 // moc doesn't support volatile slots
    void pureSlot4() volatile final override {}
    void pureSlot5() override final volatile {}
    void pureSlot6() const volatile final override {}
    void pureSlot7() volatile final override const {}
    void pureSlot8() const override final volatile {}
    void pureSlot9() override final volatile const {}
#endif
};

class ExplicitOverrideControlSealedOverride : public ExplicitOverrideControlBase
{
    Q_OBJECT
public:
    explicit ExplicitOverrideControlSealedOverride(QObject *parent = nullptr)
        : ExplicitOverrideControlBase(parent) {}

private Q_SLOTS:
    void pureSlot0() sealed override {}
    void pureSlot1() override sealed {}
    void pureSlot2() override const sealed {}
    void pureSlot3() sealed const override {}
#if 0 // moc doesn't support volatile slots
    void pureSlot4() volatile sealed override {}
    void pureSlot5() sealed override volatile {}
    void pureSlot6() const override volatile sealed {}
    void pureSlot7() volatile sealed override const {}
    void pureSlot8() const sealed volatile override {}
    void pureSlot9() override sealed volatile const {}
#endif
};

QT_WARNING_POP

#ifndef Q_MOC_RUN
# undef final
# undef sealed
# undef override
#endif

#endif // TESTS_AUTO_CORELIB_TOOLS_MOC_CXX11_EXPLICIT_OVERRIDE_CONTROL_H
