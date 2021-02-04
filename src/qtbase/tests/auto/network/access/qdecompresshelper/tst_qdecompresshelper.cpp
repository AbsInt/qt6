/****************************************************************************
**
** Copyright (C) 2020 The Qt Company Ltd.
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

#include <QTest>

#include <QtNetwork/private/qdecompresshelper_p.h>

#include <QtCore/qbytearray.h>

const QString srcDir = QStringLiteral(QT_STRINGIFY(SRC_DIR));

class tst_QDecompressHelper : public QObject
{
    Q_OBJECT

private:
    void sharedDecompress_data();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void encodingSupported();

    void decompress_data();
    void decompress();

    void partialDecompress_data();
    void partialDecompress();

    void countAhead_data();
    void countAhead();
    void countAheadByteDataBuffer_data();
    void countAheadByteDataBuffer();

    void countAheadPartialRead_data();
    void countAheadPartialRead();

    void decompressBigData_data();
    void decompressBigData();

#if QT_POINTER_SIZE >= 8
    void bigZlib();
#endif
};

void tst_QDecompressHelper::initTestCase()
{
    Q_INIT_RESOURCE(gzip);
    Q_INIT_RESOURCE(inflate);
#if QT_CONFIG(zstd)
    Q_INIT_RESOURCE(zstandard);
#endif
}
void tst_QDecompressHelper::cleanupTestCase()
{
#if QT_CONFIG(zstd)
    Q_CLEANUP_RESOURCE(zstandard);
#endif
    Q_CLEANUP_RESOURCE(inflate);
    Q_CLEANUP_RESOURCE(gzip);
}

void tst_QDecompressHelper::encodingSupported()
{
    const QByteArrayList &accepted = QDecompressHelper::acceptedEncoding();

    QVERIFY(QDecompressHelper::isSupportedEncoding("deflate"));
    QVERIFY(accepted.contains("deflate"));
    QVERIFY(QDecompressHelper::isSupportedEncoding("gzip"));
    QVERIFY(accepted.contains("gzip"));
    int expected = 2;

    QVERIFY(accepted.indexOf("gzip") < accepted.indexOf("deflate"));

#if QT_CONFIG(brotli)
    QVERIFY(QDecompressHelper::isSupportedEncoding("br"));
    QVERIFY(accepted.contains("br"));
    ++expected;
#endif

#if QT_CONFIG(zstd)
    QVERIFY(QDecompressHelper::isSupportedEncoding("zstd"));
    QVERIFY(accepted.contains("zstd"));
    ++expected;
#endif
    QCOMPARE(expected, accepted.size());
}

void tst_QDecompressHelper::sharedDecompress_data()
{
    QTest::addColumn<QByteArray>("encoding");
    QTest::addColumn<QByteArray>("data");
    QTest::addColumn<QByteArray>("expected");

    QTest::newRow("gzip-hello-world")
            << QByteArray("gzip")
            << QByteArray::fromBase64("H4sIAAAAAAAAA8tIzcnJVyjPL8pJAQCFEUoNCwAAAA==")
            << QByteArray("hello world");

    // Has two streams. ZLib reports end of stream after the first one, but we need to decompress
    // all of the streams to get the full file.
    QTest::newRow("gzip-multistream-hello-world")
            << QByteArray("gzip")
            << QByteArray::fromBase64(
                       "H4sIAAAAAAAAA8tIzcnJBwCGphA2BQAAAB+LCAAAAAAAAANTKM8vykkBAMtCO0oGAAAA")
            << QByteArray("hello world");

    QTest::newRow("deflate-hello-world")
            << QByteArray("deflate") << QByteArray::fromBase64("eJzLSM3JyVcozy/KSQEAGgsEXQ==")
            << QByteArray("hello world");

#if QT_CONFIG(brotli)
    QTest::newRow("brotli-hello-world")
            << QByteArray("br") << QByteArray::fromBase64("DwWAaGVsbG8gd29ybGQD")
            << QByteArray("hello world");
#endif

#if QT_CONFIG(zstd)
    QTest::newRow("zstandard-hello-world")
            << QByteArray("zstd") << QByteArray::fromBase64("KLUv/QRYWQAAaGVsbG8gd29ybGRoaR6y")
            << QByteArray("hello world");
#endif
}

void tst_QDecompressHelper::decompress_data()
{
    sharedDecompress_data();
}

void tst_QDecompressHelper::decompress()
{
    QDecompressHelper helper;

    QFETCH(QByteArray, encoding);
    QVERIFY(helper.setEncoding(encoding));

    QFETCH(QByteArray, data);
    helper.feed(data);

    QFETCH(QByteArray, expected);
    QByteArray actual(expected.size(), Qt::Uninitialized);
    qsizetype read = helper.read(actual.data(), actual.size());

    QCOMPARE(read, expected.size());
    QCOMPARE(actual, expected);
}

void tst_QDecompressHelper::partialDecompress_data()
{
    sharedDecompress_data();
}

// Test that even though we read 1 byte at a time we
// don't lose data from the decoder's internal storage
void tst_QDecompressHelper::partialDecompress()
{
    QDecompressHelper helper;

    QFETCH(QByteArray, encoding);
    QVERIFY(helper.setEncoding(encoding));

    QFETCH(QByteArray, data);
    helper.feed(data);

    QFETCH(QByteArray, expected);
    QByteArray actual(expected.size(), Qt::Uninitialized);
    qsizetype readTotal = 0;
    while (helper.hasData()) {
        qsizetype read = helper.read(actual.data() + readTotal, 1);
        if (read != 0) // last read might return 0
            QCOMPARE(read, 1); // Make sure we don't suddenly read too much
        readTotal += read;
    }

    QCOMPARE(readTotal, expected.size());
    QCOMPARE(actual, expected);
}

void tst_QDecompressHelper::countAhead_data()
{
    sharedDecompress_data();
}

// Test the double-decompress / count uncompressed size feature.
// We expect that after it has been fed data it will be able to
// tell us the full size of the data when uncompressed.
void tst_QDecompressHelper::countAhead()
{
    QDecompressHelper helper;
    helper.setCountingBytesEnabled(true);

    QFETCH(QByteArray, encoding);
    QVERIFY(helper.setEncoding(encoding));

    QFETCH(QByteArray, data);
    QByteArray firstPart = data.left(data.size() - data.size() / 6);
    QVERIFY(firstPart.size() < data.size()); // sanity check
    QByteArray secondPart = data.mid(firstPart.size());
    helper.feed(firstPart); // feed by copy

    // it's a reasonable assumption that after feeding it the first part
    // should have decompressed something
    QVERIFY(helper.uncompressedSize() > 0);

    helper.feed(std::move(secondPart)); // feed by move

    QFETCH(QByteArray, expected);
    QCOMPARE(helper.uncompressedSize(), expected.size());

    QByteArray actual(helper.uncompressedSize(), Qt::Uninitialized);
    qsizetype read = helper.read(actual.data(), actual.size());

    QCOMPARE(read, expected.size());
    QCOMPARE(actual, expected);
}

void tst_QDecompressHelper::countAheadByteDataBuffer_data()
{
    sharedDecompress_data();
}

void tst_QDecompressHelper::countAheadByteDataBuffer()
{
    QFETCH(QByteArray, encoding);
    QFETCH(QByteArray, data);
    QFETCH(QByteArray, expected);
    { // feed buffer by const-ref
        QDecompressHelper helper;
        helper.setCountingBytesEnabled(true);
        QVERIFY(helper.setEncoding(encoding));

        QByteArray firstPart = data.left(data.size() - data.size() / 6);
        QVERIFY(firstPart.size() < data.size()); // sanity check
        QByteArray secondPart = data.mid(firstPart.size());

        QByteDataBuffer buffer;
        buffer.append(firstPart);
        buffer.append(secondPart);

        helper.feed(buffer);

        QCOMPARE(helper.uncompressedSize(), expected.size());

        QByteArray actual(helper.uncompressedSize(), Qt::Uninitialized);
        qsizetype read = helper.read(actual.data(), actual.size());

        QCOMPARE(read, expected.size());
        QCOMPARE(actual, expected);
    }
    { // Feed buffer by move
        QDecompressHelper helper;
        helper.setCountingBytesEnabled(true);
        QVERIFY(helper.setEncoding(encoding));

        QByteArray firstPart = data.left(data.size() - data.size() / 6);
        QVERIFY(firstPart.size() < data.size()); // sanity check
        QByteArray secondPart = data.mid(firstPart.size());

        QByteDataBuffer buffer;
        buffer.append(firstPart);
        buffer.append(secondPart);

        helper.feed(std::move(buffer));

        QCOMPARE(helper.uncompressedSize(), expected.size());

        QByteArray actual(helper.uncompressedSize(), Qt::Uninitialized);
        qsizetype read = helper.read(actual.data(), actual.size());

        QCOMPARE(read, expected.size());
        QCOMPARE(actual, expected);
    }
}

void tst_QDecompressHelper::countAheadPartialRead_data()
{
    sharedDecompress_data();
}

// Make sure that the size is adjusted as we read data
void tst_QDecompressHelper::countAheadPartialRead()
{
    QDecompressHelper helper;
    helper.setCountingBytesEnabled(true);

    QFETCH(QByteArray, encoding);
    QVERIFY(helper.setEncoding(encoding));

    QFETCH(QByteArray, data);
    QByteArray firstPart = data.left(data.size() - data.size() / 6);
    QVERIFY(firstPart.size() < data.size()); // sanity check
    QByteArray secondPart = data.mid(firstPart.size());
    helper.feed(firstPart);

    // it's a reasonable assumption that after feeding it half the data it
    // should have decompressed something
    QVERIFY(helper.uncompressedSize() > 0);

    helper.feed(secondPart);

    QFETCH(QByteArray, expected);
    QCOMPARE(helper.uncompressedSize(), expected.size());

    QByteArray actual(helper.uncompressedSize(), Qt::Uninitialized);
    qsizetype read = helper.read(actual.data(), 5);
    QCOMPARE(read, 5);
    QCOMPARE(helper.uncompressedSize(), expected.size() - read);
    read += helper.read(actual.data() + read, 1);
    QCOMPARE(read, 6);
    QCOMPARE(helper.uncompressedSize(), expected.size() - read);

    read += helper.read(actual.data() + read, expected.size() - read);

    QCOMPARE(read, expected.size());
    QCOMPARE(actual, expected);
}

void tst_QDecompressHelper::decompressBigData_data()
{
    QTest::addColumn<QByteArray>("encoding");
    QTest::addColumn<QString>("path");
    QTest::addColumn<qint64>("size");

    qint64 fourGiB = 4ll * 1024ll * 1024ll * 1024ll;
    qint64 fiveGiB = 5ll * 1024ll * 1024ll * 1024ll;

    QTest::newRow("gzip-4G") << QByteArray("gzip") << QString(":/4G.gz") << fourGiB;
    QTest::newRow("deflate-5G") << QByteArray("deflate") << QString(":/5GiB.txt.inflate")
                                << fiveGiB;

#if QT_CONFIG(brotli)
    QTest::newRow("brotli-4G") << QByteArray("br") << (srcDir + "/4G.br") << fourGiB;
#endif

#if QT_CONFIG(zstd)
    QTest::newRow("zstandard-4G") << QByteArray("zstd") << (":/4G.zst") << fourGiB;
#endif
}

void tst_QDecompressHelper::decompressBigData()
{
    QFETCH(QString, path);
    QFile file(path);
    QVERIFY(file.open(QIODevice::ReadOnly));

    const qint64 third = file.bytesAvailable() / 3;

    QDecompressHelper helper;
    helper.setArchiveBombDetectionEnabled(false);
    QFETCH(QByteArray, encoding);
    helper.setEncoding(encoding);

    QByteArray output(32 * 1024, Qt::Uninitialized);
    qint64 totalSize = 0;
    while (!file.atEnd()) {
        helper.feed(file.read(third));
        while (helper.hasData()) {
            qsizetype bytesRead = helper.read(output.data(), output.size());
            QVERIFY(bytesRead <= output.size());
            totalSize += bytesRead;
            const auto isZero = [](char c) { return c == '\0'; };
            bool allZero = std::all_of(output.cbegin(), output.cbegin() + bytesRead, isZero);
            QVERIFY(allZero);
        }
    }
    QTEST(totalSize, "size");
}

#if QT_POINTER_SIZE >= 8
void tst_QDecompressHelper::bigZlib()
{
    // ZLib uses unsigned integers as their size type internally which creates some special
    // cases in the internal code that should be tested!
    QFile file(":/5GiB.txt.inflate");
    QVERIFY(file.open(QIODevice::ReadOnly));
    QByteArray compressedData = file.readAll();

    QDecompressHelper helper;
    helper.setArchiveBombDetectionEnabled(false);
    helper.setEncoding("deflate");
    auto firstHalf = compressedData.left(compressedData.size() - 2);
    helper.feed(firstHalf);
    helper.feed(compressedData.mid(firstHalf.size()));

    // We need the whole thing in one go... which is why this test is not available for 32-bit
    qint64 expected = 5ll * 1024ll * 1024ll * 1024ll;
    // This can be replaced with QByteArray after the qsizetype change is merged
    std::unique_ptr<char[]> output(new char[expected]);
    qsizetype size = helper.read(output.get(), expected);
    QCOMPARE(size, expected);
}
#endif

QTEST_MAIN(tst_QDecompressHelper)

#include "tst_qdecompresshelper.moc"
