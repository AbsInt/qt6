// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>

#include <qtextdocument.h>
#include <qdebug.h>
#ifndef Q_OS_WIN
#  include <private/qtextdocument_p.h>
#endif

#include <qtextobject.h>
#include <qtextcursor.h>

QT_FORWARD_DECLARE_CLASS(QTextDocument)

class tst_QTextBlock : public QObject
{
    Q_OBJECT

private slots:
    void init();
    void cleanup();
    void fragmentOverBlockBoundaries();
    void excludeParagraphSeparatorFragment();
    void backwardsBlockIterator();
    void previousBlock_qtbug18026();
    void removedBlock_qtbug18500();

private:
    QTextDocument *doc;
    QTextCursor cursor;
};

void tst_QTextBlock::init()
{
    doc = new QTextDocument;
    cursor = QTextCursor(doc);
}

void tst_QTextBlock::cleanup()
{
    cursor = QTextCursor();
    delete doc;
    doc = 0;
}

void tst_QTextBlock::fragmentOverBlockBoundaries()
{
    /* this creates two fragments in the piecetable:
     * 1) 'hello<parag separator here>world'
     * 2) '<parag separator>'
     * (they are not united because the former was interested after the latter,
     * hence their position in the pt buffer is the other way around)
     */
    cursor.insertText("Hello");
    cursor.insertBlock();
    cursor.insertText("World");

    cursor.movePosition(QTextCursor::Start);

    const QTextDocument *doc = cursor.block().document();
    QVERIFY(doc);
    // Block separators are always a fragment of their self. Thus:
    // |Hello|\b|World|\b|
#if !defined(Q_OS_WIN)
    QCOMPARE(QTextDocumentPrivate::get(doc)->fragmentMap().numNodes(), 4);
#endif
    QCOMPARE(cursor.block().text(), QString("Hello"));
    cursor.movePosition(QTextCursor::NextBlock);
    QCOMPARE(cursor.block().text(), QString("World"));
}

void tst_QTextBlock::excludeParagraphSeparatorFragment()
{
    QTextCharFormat fmt;
    fmt.setForeground(Qt::blue);
    cursor.insertText("Hello", fmt);

    QTextBlock block = doc->begin();
    QVERIFY(block.isValid());

    QTextBlock::Iterator it = block.begin();

    QTextFragment fragment = it.fragment();
    QVERIFY(fragment.isValid());
    QCOMPARE(fragment.text(), QString("Hello"));

    ++it;
    QVERIFY(it.atEnd());
    QCOMPARE(it, block.end());
}

void tst_QTextBlock::backwardsBlockIterator()
{
    QTextCharFormat fmt;

    fmt.setForeground(Qt::magenta);
    cursor.insertText("A", fmt);

    fmt.setForeground(Qt::red);
    cursor.insertText("A", fmt);

    fmt.setForeground(Qt::magenta);
    cursor.insertText("A", fmt);

    QTextBlock block = doc->begin();
    QVERIFY(block.isValid());

    QTextBlock::Iterator it = block.begin();
    QCOMPARE(it.fragment().position(), 0);
    ++it;
    QCOMPARE(it.fragment().position(), 1);
    ++it;

    QCOMPARE(it.fragment().position(), 2);

    --it;
    QCOMPARE(it.fragment().position(), 1);
    --it;
    QCOMPARE(it.fragment().position(), 0);
}

void tst_QTextBlock::previousBlock_qtbug18026()
{
    QTextBlock last = doc->end().previous();
    QVERIFY(last.isValid());
}

void tst_QTextBlock::removedBlock_qtbug18500()
{
    cursor.insertText("line 1\nline 2\nline 3 \nline 4\n");
    cursor.setPosition(7);
    QTextBlock block = cursor.block();
    cursor.setPosition(21, QTextCursor::KeepAnchor);

    cursor.removeSelectedText();
    QVERIFY(!block.isValid());
}

QTEST_MAIN(tst_QTextBlock)
#include "tst_qtextblock.moc"
