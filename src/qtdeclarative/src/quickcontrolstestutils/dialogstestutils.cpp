/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
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
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "dialogstestutils_p.h"

#include <QtTest/qsignalspy.h>
#include <QtQuick/private/qquicklistview_p.h>
#include <QtQuickTest/quicktest.h>
#include <QtQuickControls2/qquickstyle.h>
#include <QtQuickTemplates2/private/qquickabstractbutton_p.h>
#include <QtQuickTemplates2/private/qquickapplicationwindow_p.h>
#include <QtQuickTemplates2/private/qquickdialogbuttonbox_p.h>
#include <QtQuickDialogs2QuickImpl/private/qquickfiledialogdelegate_p.h>
#include <QtQuickDialogs2QuickImpl/private/qquickfolderbreadcrumbbar_p.h>
#include <QtQuickDialogs2QuickImpl/private/qquickfolderbreadcrumbbar_p_p.h>

QT_BEGIN_NAMESPACE

bool QQuickDialogTestUtils::verifyFileDialogDelegates(QQuickListView *fileDialogListView,
    const QStringList &expectedFiles, QString &failureMessage)
{
    if (QQuickTest::qIsPolishScheduled(fileDialogListView)) {
        if (!QQuickTest::qWaitForItemPolished(fileDialogListView)) {
            failureMessage = QLatin1String("Failed to polish fileDialogListView");
            return false;
        }
    }

    QStringList actualFiles;
    for (int i = 0; i < fileDialogListView->count(); ++i) {
        auto delegate = qobject_cast<QQuickFileDialogDelegate*>(
            QQuickVisualTestUtils::findViewDelegateItem(fileDialogListView, i));
        if (!delegate) {
            failureMessage = QString::fromLatin1("Delegate at index %1 is null").arg(i);
            return false;
        }

        // Need to call absoluteFilePath on Windows; see comment in dialogtestutil.h.
        actualFiles.append(QFileInfo(delegate->file().toLocalFile()).absoluteFilePath());
    }

    if (actualFiles != expectedFiles) {
        QString expectedFilesStr = QDebug::toString(expectedFiles);
        QString actualFilesStr = QDebug::toString(actualFiles);
        failureMessage = QString::fromLatin1("Mismatch in actual vs expected "
            "delegates in fileDialogListView:\n    expected: %1\n      actual: %2");
        if (failureMessage.size() + expectedFilesStr.size() + actualFilesStr.size() > 1024) {
            // If we've exceeded QTest's character limit for failure messages,
            // just show the number of files.
            expectedFilesStr = QString::number(expectedFiles.size());
            actualFilesStr = QString::number(actualFiles.size());
        }
        failureMessage = failureMessage.arg(expectedFilesStr, actualFilesStr);
        return false;
    }

    return true;
}

bool QQuickDialogTestUtils::verifyBreadcrumbDelegates(QQuickFolderBreadcrumbBar *breadcrumbBar,
    const QUrl &expectedFolder, QString &failureMessage)
{
    if (!breadcrumbBar) {
        failureMessage = QLatin1String("breadcrumbBar is null");
        return false;
    }

    auto breadcrumbBarListView = qobject_cast<QQuickListView*>(breadcrumbBar->contentItem());
    if (!breadcrumbBarListView) {
        failureMessage = QLatin1String("breadcrumbBar's ListView is null");
        return false;
    }

    if (QQuickTest::qIsPolishScheduled(breadcrumbBarListView)) {
        if (!QQuickTest::qWaitForItemPolished(breadcrumbBarListView)) {
            failureMessage = QLatin1String("Failed to polish breadcrumbBarListView");
            return false;
        }
    }

    QStringList actualCrumbs;
    for (int i = 0; i < breadcrumbBarListView->count(); ++i) {
        auto delegate = qobject_cast<QQuickAbstractButton*>(
            QQuickVisualTestUtils::findViewDelegateItem(breadcrumbBarListView, i));
        if (!delegate) {
            // It's a separator or some other non-crumb item.
            continue;
        }

        actualCrumbs.append(delegate->text());
    }

    QStringList expectedCrumbs = QQuickFolderBreadcrumbBarPrivate::crumbPathsForFolder(expectedFolder);
    for (int i = 0; i < expectedCrumbs.size(); ++i) {
        QString &crumbPath = expectedCrumbs[i];
        crumbPath = QQuickFolderBreadcrumbBarPrivate::folderBaseName(crumbPath);
    }

    if (actualCrumbs != expectedCrumbs) {
        failureMessage = QString::fromLatin1("Mismatch in actual vs expected "
            "delegates in breadcrumbBarListView:\n    expected: %1\n      actual: %2")
            .arg(QDebug::toString(expectedCrumbs), QDebug::toString(actualCrumbs));
        return false;
    }

    return true;
}

QQuickAbstractButton *QQuickDialogTestUtils::findDialogButton(QQuickDialogButtonBox *box, const QString &buttonText)
{
    for (int i = 0; i < box->count(); ++i) {
        auto button = qobject_cast<QQuickAbstractButton*>(box->itemAt(i));
        if (button && button->text().toUpper() == buttonText.toUpper())
            return button;
    }
    return nullptr;
}

void QQuickDialogTestUtils::enterText(QWindow *window, const QString &textToEnter)
{
    for (int i = 0; i < textToEnter.size(); ++i) {
        const QChar key = textToEnter.at(i);
        QTest::keyClick(window, key.toLatin1());
    }
}

QT_END_NAMESPACE
