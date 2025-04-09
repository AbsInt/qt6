// Copyright (C) 2022 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/qtest.h>
#include <QtTest/qsignalspy.h>
#include <QtTest/qtesttouch.h>

#include <QtGui/qclipboard.h>
#include <QtGui/qfontmetrics.h>
#include <QtGui/private/qguiapplication_p.h>
#include <QtGui/qpa/qplatformintegration.h>
#include <QtGui/qtestsupport_gui.h>
#include <QtQuick/qquickview.h>
#include <QtQuickTestUtils/private/qmlutils_p.h>
#include <QtQuickTestUtils/private/viewtestutils_p.h>
#include <QtQuickTestUtils/private/visualtestutils_p.h>
#include <QtQuickTemplates2/private/qquickcontextmenu_p.h>
#include <QtQuickTemplates2/private/qquickmenu_p.h>
#include <QtQuickTemplates2/private/qquickmenuitem_p.h>
#include <QtQuickTemplates2/private/qquickmenuitem_p_p.h>
#include <QtQuickTemplates2/private/qquickpopup_p_p.h>
#include <QtQuickTemplates2/private/qquickpopupwindow_p_p.h>
#include <QtQuickTemplates2/private/qquicktextfield_p.h>
#include <QtQuickControlsTestUtils/private/qtest_quickcontrols_p.h>
#include <QStyleHints>

using namespace QQuickVisualTestUtils;

class tst_QQuickTextField : public QQmlDataTest
{
    Q_OBJECT

public:
    tst_QQuickTextField();

private slots:
    void initTestCase() override;
    void touchscreenDoesNotSelect_data();
    void touchscreenDoesNotSelect();
    void releaseAfterPressAndHold();
    void contextMenuCut();
    void contextMenuCopy();
    void contextMenuPaste();
    void contextMenuDelete();
    void contextMenuSelectAll();

private:
    QScopedPointer<QPointingDevice> touchDevice = QScopedPointer<QPointingDevice>(QTest::createTouchDevice());

    bool hasClipboardSupport =
#if QT_CONFIG(clipboard)
        true;
#else
        false;
#endif
};

tst_QQuickTextField::tst_QQuickTextField()
    : QQmlDataTest(QT_QMLTEST_DATADIR)
{
}

void tst_QQuickTextField::initTestCase()
{
#ifdef Q_OS_ANDROID
    if (QNativeInterface::QAndroidApplication::sdkVersion() > 23)
        QSKIP("Crashes on Android 7+, figure out why (QTBUG-107028)");
#endif
    QQmlDataTest::initTestCase();
    qputenv("QML_NO_TOUCH_COMPRESSION", "1");
    // Showing a native menu is a blocking call, so the test will timeout.
    QCoreApplication::setAttribute(Qt::AA_DontUseNativeMenuWindows);
}

void tst_QQuickTextField::touchscreenDoesNotSelect_data()
{
    QTest::addColumn<QUrl>("src");
    QTest::addColumn<bool>("setEnv");
    QTest::addColumn<bool>("selectByMouse");
    QTest::addColumn<bool>("selectByTouch");
    QTest::newRow("new default") << testFileUrl("mouseselection_default.qml") << false << true << false;
#if QT_VERSION < QT_VERSION_CHECK(7, 0, 0)
    QTest::newRow("putenv") << testFileUrl("mouseselection_default.qml") << true << false << false;
    QTest::newRow("old_import") << testFileUrl("mouseselection_old_default.qml") << false << true << false;
    QTest::newRow("old+putenv") << testFileUrl("mouseselection_old_default.qml") << true << false << false;
    QTest::newRow("old+putenv+selectByMouse") << testFileUrl("mouseselection_old_overridden.qml") << true << true << true;
#endif
}

void tst_QQuickTextField::touchscreenDoesNotSelect()
{
    QFETCH(QUrl, src);
    QFETCH(bool, setEnv);
    QFETCH(bool, selectByMouse);
    QFETCH(bool, selectByTouch);

    if (setEnv)
        qputenv("QT_QUICK_CONTROLS_TEXT_SELECTION_BEHAVIOR", "old");
    else
        qunsetenv("QT_QUICK_CONTROLS_TEXT_SELECTION_BEHAVIOR");

    QQuickView window;
    QVERIFY(QQuickTest::showView(window, src));

    QQuickTextField *textField = qobject_cast<QQuickTextField *>(window.rootObject());
    QVERIFY(textField != nullptr);
    QCOMPARE(textField->selectByMouse(), selectByMouse);
    textField->setSelectByMouse(true); // enable selection with pre-6.4 import version
    QVERIFY(textField->selectedText().isEmpty());

    if (selectByMouse) {
        // press-drag-and-release from x1 to x2
        const int x1 = textField->leftPadding();
        const int x2 = textField->width() / 2;
        const int y = textField->height() / 2;
        QTest::touchEvent(&window, touchDevice.data()).press(0, QPoint(x1,y), &window);
        QTest::touchEvent(&window, touchDevice.data()).move(0, QPoint(x2,y), &window);
        QTest::touchEvent(&window, touchDevice.data()).release(0, QPoint(x2,y), &window);
        QQuickTouchUtils::flush(&window);
        // if the env var is set, fall back to old behavior: touch swipe _does_ select text if selectByMouse is true
        QCOMPARE(textField->selectedText().isEmpty(), !selectByTouch);
    }
}

void tst_QQuickTextField::releaseAfterPressAndHold()
{
    QQuickView window;
    QVERIFY(QQuickTest::showView(window, testFileUrl("mouseselection_default.qml")));

    QQuickTextField *textField = qobject_cast<QQuickTextField *>(window.rootObject());
    QVERIFY(textField != nullptr);

    QSignalSpy releasedSpy(textField, &QQuickTextField::released);
    QSignalSpy pressAndHoldSpy(textField, &QQuickTextField::pressAndHold);

    const QPoint clickPosition(textField->width() / 2, textField->height() / 2);

    QTest::mousePress(&window, Qt::LeftButton, Qt::NoModifier, clickPosition);
    QTest::qWait(QGuiApplication::styleHints()->mousePressAndHoldInterval() + 100);
    QCOMPARE(pressAndHoldSpy.count(), 1);

    QTest::mouseRelease(&window, Qt::LeftButton, Qt::NoModifier, clickPosition);
    QCOMPARE(releasedSpy.count(), 1);
}

static const auto mementoStr = QLatin1String("Memento");
static const auto moriStr = QLatin1String("mori");
static const auto mementoMoriStr = QLatin1String("Memento mori");

void tst_QQuickTextField::contextMenuCut()
{
    QQuickView window;
    QVERIFY(QQuickTest::showView(window, testFileUrl("contextMenu.qml")));
    window.requestActivate();
    QVERIFY(QTest::qWaitForWindowActive(&window));

    auto *textField = window.rootObject()->property("textField").value<QQuickTextField *>();
    QVERIFY(textField);
    textField->forceActiveFocus();

    // Right click on the TextField to open the context menu.
    // Right-clicking without a selection should result in the Cut menu item being disabled.
    QTest::mouseClick(&window, Qt::RightButton, Qt::NoModifier, mapCenterToWindow(textField));
    auto *contextMenu = textField->findChild<QQuickContextMenu *>();
    QVERIFY(contextMenu);
    QTRY_VERIFY(contextMenu->menu()->isOpened());
    auto *cutMenuItem = qobject_cast<QQuickMenuItem *>(contextMenu->menu()->itemAt(0));
    QCOMPARE(cutMenuItem->text(), "Cut");
    QVERIFY(!cutMenuItem->isEnabled());
    QVERIFY(textField->selectedText().isEmpty());

    // Close the context menu.
    QTest::keyClick(&window, Qt::Key_Escape);
    QTRY_VERIFY(!contextMenu->menu()->isVisible());

    // Right-clicking with a selection should result in the Cut menu item being enabled
    // if Qt was built with clipboard support.
    textField->select(mementoStr.length(), textField->length());
    const QString cutText = QLatin1Char(' ') + moriStr;
    QCOMPARE(textField->selectedText(), cutText);
    QTest::mouseClick(&window, Qt::RightButton, Qt::NoModifier, mapCenterToWindow(textField));
    QTRY_VERIFY(contextMenu->menu()->isOpened());
    QCOMPARE(cutMenuItem->isEnabled(), hasClipboardSupport);
    // QTBUG-133302: the first menu item shouldn't be immediately triggered.
    QCOMPARE(QQuickMenuItemPrivate::get(cutMenuItem)->animateTimer, 0);

    // Click on the Cut menu item (if enabled) and close the menu.
#if QT_CONFIG(clipboard)
    auto *contextMenuPrivate = QQuickPopupPrivate::get(contextMenu->menu());
    QQuickWindow *contextMenuPopupWindow = &window;
    if (contextMenuPrivate->usePopupWindow())
        contextMenuPopupWindow = contextMenuPrivate->popupWindow;
    QTest::mouseClick(contextMenuPopupWindow, Qt::LeftButton, Qt::NoModifier, mapCenterToWindow(cutMenuItem));
    QCOMPARE(textField->text(), mementoStr);
    QCOMPARE(qGuiApp->clipboard()->text(), cutText);
#else
    QTest::keyClick(&window, Qt::Key_Escape);
#endif
    QTRY_VERIFY(!contextMenu->menu()->isVisible());

    // Make the TextField read-only. Cut should no longer be enabled.
    textField->setReadOnly(true);
    textField->selectAll();
    // Right click on the TextField to open the context menu.
    QTest::mouseClick(&window, Qt::RightButton, Qt::NoModifier, mapCenterToWindow(textField));
    QTRY_VERIFY(contextMenu->menu()->isOpened());
    QCOMPARE(cutMenuItem->text(), "Cut");
    QVERIFY(!cutMenuItem->isEnabled());

    // Close the context menu.
    QTest::keyClick(&window, Qt::Key_Escape);
    QTRY_VERIFY(!contextMenu->menu()->isVisible());
}

void tst_QQuickTextField::contextMenuCopy()
{
    QQuickView window;
    QVERIFY(QQuickTest::showView(window, testFileUrl("contextMenu.qml")));
    window.requestActivate();
    QVERIFY(QTest::qWaitForWindowActive(&window));

    auto *textField = window.rootObject()->property("textField").value<QQuickTextField *>();
    QVERIFY(textField);
    textField->forceActiveFocus();

    // Right click on the TextField to open the context menu.
    // Right-clicking without a selection should result in the Copy menu item being disabled.
    QTest::mouseClick(&window, Qt::RightButton, Qt::NoModifier, mapCenterToWindow(textField));
    auto *contextMenu = textField->findChild<QQuickContextMenu *>();
    QVERIFY(contextMenu);
    QTRY_VERIFY(contextMenu->menu()->isOpened());
    auto *copyMenuItem = qobject_cast<QQuickMenuItem *>(contextMenu->menu()->itemAt(1));
    QVERIFY(copyMenuItem);
    QCOMPARE(copyMenuItem->text(), "Copy");
    QVERIFY(!copyMenuItem->isEnabled());

    // Close the context menu.
    QTest::keyClick(&window, Qt::Key_Escape);
    QTRY_VERIFY(!contextMenu->menu()->isVisible());

    // Right-clicking with a selection should result in the Copy menu item being enabled
    // if Qt was built with clipboard support.
    textField->select(0, mementoStr.length());
    QCOMPARE(textField->selectedText(), mementoStr);
    QTest::mouseClick(&window, Qt::RightButton, Qt::NoModifier, mapCenterToWindow(textField));
    QTRY_VERIFY(contextMenu->menu()->isOpened());
    QCOMPARE(copyMenuItem->isEnabled(), hasClipboardSupport);

    // Click on the Copy menu item (if enabled) and close the menu.
#if QT_CONFIG(clipboard)
    auto *contextMenuPrivate = QQuickPopupPrivate::get(contextMenu->menu());
    QQuickWindow *contextMenuPopupWindow = &window;
    if (contextMenuPrivate->usePopupWindow())
        contextMenuPopupWindow = contextMenuPrivate->popupWindow;
    QTest::mouseClick(contextMenuPopupWindow, Qt::LeftButton, Qt::NoModifier, mapCenterToWindow(copyMenuItem));
    QCOMPARE(textField->text(), mementoMoriStr);
    const auto *clipboard = QGuiApplication::clipboard();
    QCOMPARE(clipboard->text(), mementoStr);
#else
    QTest::keyClick(&window, Qt::Key_Escape);
#endif
    QTRY_VERIFY(!contextMenu->menu()->isVisible());

    // Make the TextField read-only. Copy should still be enabled.
    textField->setReadOnly(true);
    // Right click on the TextField to open the context menu.
    QTest::mouseClick(&window, Qt::RightButton, Qt::NoModifier, mapCenterToWindow(textField));
    QTRY_VERIFY(contextMenu->menu()->isOpened());
    QCOMPARE(copyMenuItem->text(), "Copy");
    // Select some text.
    textField->select(0, mementoStr.length());
    QCOMPARE(copyMenuItem->isEnabled(), hasClipboardSupport);

    // Close the context menu.
    QTest::keyClick(&window, Qt::Key_Escape);
    QTRY_VERIFY(!contextMenu->menu()->isVisible());
}

void tst_QQuickTextField::contextMenuPaste()
{
    QQuickView window;
    QVERIFY(QQuickTest::showView(window, testFileUrl("contextMenu.qml")));
    window.requestActivate();
    QVERIFY(QTest::qWaitForWindowActive(&window));

    auto *textField = window.rootObject()->property("textField").value<QQuickTextField *>();
    QVERIFY(textField);
    textField->forceActiveFocus();

#if QT_CONFIG(clipboard)
    auto *clipboard = QGuiApplication::clipboard();
    clipboard->setText(" 🫠");
#endif

    // Right click on the TextField to open the context menu.
    QTest::mouseClick(&window, Qt::RightButton, Qt::NoModifier, mapCenterToWindow(textField));
    auto *contextMenu = textField->findChild<QQuickContextMenu *>();
    QVERIFY(contextMenu);
    QTRY_VERIFY(contextMenu->menu()->isOpened());
    auto *pasteMenuItem = qobject_cast<QQuickMenuItem *>(contextMenu->menu()->itemAt(2));
    QVERIFY(pasteMenuItem);
    QCOMPARE(pasteMenuItem->text(), "Paste");
    QCOMPARE(pasteMenuItem->isEnabled(), hasClipboardSupport);

    // Click on the Paste menu item (if enabled) and close the menu.
#if QT_CONFIG(clipboard)
    auto *contextMenuPrivate = QQuickPopupPrivate::get(contextMenu->menu());
    QQuickWindow *contextMenuPopupWindow = &window;
    if (contextMenuPrivate->usePopupWindow())
        contextMenuPopupWindow = contextMenuPrivate->popupWindow;
    QTest::mouseClick(contextMenuPopupWindow, Qt::LeftButton, Qt::NoModifier, mapCenterToWindow(pasteMenuItem));
    QCOMPARE(textField->text(), mementoMoriStr + clipboard->text());
#else
    QTest::keyClick(&window, Qt::Key_Escape);
#endif
    QTRY_VERIFY(!contextMenu->menu()->isVisible());

    // Make the TextField read-only. Paste should no longer be enabled.
    textField->setReadOnly(true);
    // Right click on the TextField to open the context menu.
    QTest::mouseClick(&window, Qt::RightButton, Qt::NoModifier, mapCenterToWindow(textField));
    QTRY_VERIFY(contextMenu->menu()->isOpened());
    QCOMPARE(pasteMenuItem->text(), "Paste");
    QVERIFY(!pasteMenuItem->isEnabled());
    textField->setReadOnly(false);

    // Close the context menu.
    QTest::keyClick(&window, Qt::Key_Escape);
    QTRY_VERIFY(!contextMenu->menu()->isVisible());
}

void tst_QQuickTextField::contextMenuDelete()
{
    QQuickView window;
    QVERIFY(QQuickTest::showView(window, testFileUrl("contextMenu.qml")));
    window.requestActivate();
    QVERIFY(QTest::qWaitForWindowActive(&window));

    auto *textField = window.rootObject()->property("textField").value<QQuickTextField *>();
    QVERIFY(textField);
    textField->forceActiveFocus();

    // Right click on the TextField to open the context menu.
    // Right-clicking without a selection should result in the Delete menu item being disabled.
    QTest::mouseClick(&window, Qt::RightButton, Qt::NoModifier, mapCenterToWindow(textField));
    auto *contextMenu = textField->findChild<QQuickContextMenu *>();
    QVERIFY(contextMenu);
    QTRY_VERIFY(contextMenu->menu()->isOpened());
    auto *deleteMenuItem = qobject_cast<QQuickMenuItem *>(contextMenu->menu()->itemAt(3));
    QCOMPARE(deleteMenuItem->text(), "Delete");
    QVERIFY(!deleteMenuItem->isEnabled());

    // Close the context menu.
    QTest::keyClick(&window, Qt::Key_Escape);
    QTRY_VERIFY(!contextMenu->menu()->isVisible());

    // Right-clicking with a selection should result in the Delete menu item being enabled.
    textField->select(mementoStr.length(), textField->length());
    QCOMPARE(textField->selectedText(), QLatin1Char(' ') + moriStr);
    QTest::mouseClick(&window, Qt::RightButton, Qt::NoModifier, mapCenterToWindow(textField));
    QTRY_VERIFY(contextMenu->menu()->isOpened());
    QVERIFY(deleteMenuItem->isEnabled());

    // Click on the Delete menu item and close the menu.
    auto *contextMenuPrivate = QQuickPopupPrivate::get(contextMenu->menu());
    QQuickWindow *contextMenuPopupWindow = &window;
    if (contextMenuPrivate->usePopupWindow())
        contextMenuPopupWindow = contextMenuPrivate->popupWindow;
    QTest::mouseClick(contextMenuPopupWindow, Qt::LeftButton, Qt::NoModifier, mapCenterToWindow(deleteMenuItem));
    QCOMPARE(textField->text(), mementoStr);
    QTRY_VERIFY(!contextMenu->menu()->isVisible());

    // Make the TextField read-only. Delete should no longer be enabled.
    textField->setReadOnly(true);
    textField->selectAll();
    // Right click on the TextField to open the context menu.
    QTest::mouseClick(&window, Qt::RightButton, Qt::NoModifier, mapCenterToWindow(textField));
    QTRY_VERIFY(contextMenu->menu()->isOpened());
    QCOMPARE(deleteMenuItem->text(), "Delete");
    QVERIFY(!deleteMenuItem->isEnabled());

    // Close the context menu.
    QTest::keyClick(&window, Qt::Key_Escape);
    QTRY_VERIFY(!contextMenu->menu()->isVisible());
}

void tst_QQuickTextField::contextMenuSelectAll()
{
    QQuickView window;
    QVERIFY(QQuickTest::showView(window, testFileUrl("contextMenu.qml")));
    window.requestActivate();
    QVERIFY(QTest::qWaitForWindowActive(&window));

    auto *textField = window.rootObject()->property("textField").value<QQuickTextField *>();
    QVERIFY(textField);
    textField->forceActiveFocus();

    // Right click on the TextField to open the context menu.
    QTest::mouseClick(&window, Qt::RightButton, Qt::NoModifier, mapCenterToWindow(textField));
    auto *contextMenu = textField->findChild<QQuickContextMenu *>();
    QVERIFY(contextMenu);
    QTRY_VERIFY(contextMenu->menu()->isOpened());
    auto *selectAllMenuItem = qobject_cast<QQuickMenuItem *>(contextMenu->menu()->itemAt(5));
    QVERIFY(selectAllMenuItem);
    QCOMPARE(selectAllMenuItem->text(), "Select All");

    // Click on the Select All menu item and close the menu.
    auto *contextMenuPrivate = QQuickPopupPrivate::get(contextMenu->menu());
    QQuickWindow *contextMenuPopupWindow = &window;
    if (contextMenuPrivate->usePopupWindow())
        contextMenuPopupWindow = contextMenuPrivate->popupWindow;
    QTest::mouseClick(contextMenuPopupWindow, Qt::LeftButton, Qt::NoModifier, mapCenterToWindow(selectAllMenuItem));
    QCOMPARE(textField->selectedText(), mementoMoriStr);
    QTRY_VERIFY(!contextMenu->menu()->isVisible());

    // Make the TextField read-only. Select All should still be enabled.
    textField->setReadOnly(true);
    // Right click on the TextField to open the context menu.
    QTest::mouseClick(&window, Qt::RightButton, Qt::NoModifier, mapCenterToWindow(textField));
    QTRY_VERIFY(contextMenu->menu()->isOpened());
    QCOMPARE(selectAllMenuItem->text(), "Select All");
    QVERIFY(selectAllMenuItem->isEnabled());
    textField->setReadOnly(false);

    // Close the context menu.
    QTest::keyClick(&window, Qt::Key_Escape);
    QTRY_VERIFY(!contextMenu->menu()->isVisible());
}

QTEST_QUICKCONTROLS_MAIN(tst_QQuickTextField)

#include "tst_qquicktextfield.moc"
