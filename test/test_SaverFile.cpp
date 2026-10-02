#include <QFile>
#include <QTemporaryDir>
#include <QtTest>

#include "Exceptions.h"
#include "SaverFile.h"

struct TestData {
    int value = 0;
    QString text;

    bool operator ==(const TestData &other) const {
        return value == other.value && text == other.text;
    }

    void serialize(QDataStream &stream) const {
        stream << value << text;
    }

    void deserialize(QDataStream &stream) {
        stream >> value >> text;
    }
};

struct ThrowingData {
    void serialize(QDataStream &) const {
        throw RuntimeError("serialize failed");
    }
    void deserialize(QDataStream &) {
        throw RuntimeError("deserialize failed");
    }
};

class TestSaverFile : public QObject {
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    // ---------- prepare ----------
    void prepareCreatesTempFile();
    void prepareSetsDirtyAndAllowsWrite();
    void prepareInvalidPathReturnsFalse();
    void prepareThrowsPropagatesAndCleansTemp();
    void prepareCalledTwiceOverwritesTemp();

    // ---------- read ----------
    void readMissingFileReturnsFalse();
    void readRoundTripReturnsSameData();
    void readCorruptedFileReturnsFalse();
    void readThrowsPropagates();
    void readDoesNotModifyStorageOnFailure();

    // ---------- write ----------
    void writeWithoutPrepareReturnsTrue();
    void writeWithoutTempFileReturnsFalse();
    void writeAfterPrepareMovesFile();
    void writeRemovesTempFile();
    void writeTwiceIsIdempotent();

    // ---------- Full Cycle ----------
    void fullCyclePrepareWriteRead();

    // ---------- Destructor ----------
    void destructorRemovesTempFile();

private:
    QTemporaryDir *m_dir = nullptr;
    QString m_filePath;
    QString m_tempPath;
};

void TestSaverFile::init() {
    m_dir = new QTemporaryDir();
    QVERIFY(m_dir->isValid());
    m_filePath = m_dir->filePath("data.bin");
    m_tempPath = m_dir->filePath("data.bin.tmp");
}

void TestSaverFile::cleanup() {
    delete m_dir;
    m_dir = nullptr;
}

// ---------- prepare() ----------

void TestSaverFile::prepareCreatesTempFile() {
    SaverFile<TestData> saver(m_filePath, m_tempPath);
    TestData data { 42, "hello" };

    QVERIFY(saver.prepare(data));
    QVERIFY(QFile::exists(m_tempPath));
}

void TestSaverFile::prepareSetsDirtyAndAllowsWrite() {
    SaverFile<TestData> saver(m_filePath, m_tempPath);
    TestData data { 7, "dirty" };

    QVERIFY(saver.prepare(data));
    QVERIFY(saver.write());
    QVERIFY(QFile::exists(m_filePath));
}

void TestSaverFile::prepareInvalidPathReturnsFalse() {
    const QString badPath = m_dir->filePath("no_such_dir/data.bin.tmp");
    SaverFile<TestData> saver(m_filePath, badPath);

    QVERIFY(!saver.prepare(TestData { 1, "x" }));
}

void TestSaverFile::prepareThrowsPropagatesAndCleansTemp() {
    SaverFile<ThrowingData> saver(m_filePath, m_tempPath);

    QVERIFY_THROWS_EXCEPTION(RuntimeError, Q_UNUSED(saver.prepare(ThrowingData { })));
    QVERIFY(!QFile::exists(m_tempPath));
}

void TestSaverFile::prepareCalledTwiceOverwritesTemp() {
    SaverFile<TestData> saver(m_filePath, m_tempPath);

    QVERIFY(saver.prepare(TestData { 1, "first" }));
    QVERIFY(saver.prepare(TestData { 2, "second" }));
    QVERIFY(saver.write());

    TestData out;
    QVERIFY(saver.read(out));
    QCOMPARE(out.value, 2);
    QCOMPARE(out.text, QString("second"));
}

// ---------- read() ----------

void TestSaverFile::readMissingFileReturnsFalse() {
    SaverFile<TestData> saver(m_filePath, m_tempPath);

    TestData out;
    QVERIFY(!saver.read(out));
}

void TestSaverFile::readRoundTripReturnsSameData() {
    SaverFile<TestData> saver(m_filePath, m_tempPath);

    const TestData original { 123, "round trip" };

    QVERIFY(saver.prepare(original));
    QVERIFY(saver.write());

    TestData out;
    QVERIFY(saver.read(out));
    QCOMPARE(out, original);
}

void TestSaverFile::readCorruptedFileReturnsFalse() {
    {
        QFile f(m_filePath);
        QVERIFY(f.open(QIODevice::WriteOnly));
        f.write("garbage-not-a-valid-stream");
    }

    SaverFile<TestData> saver(m_filePath, m_tempPath);
    TestData out;

    Q_UNUSED(saver.read(out));
}

void TestSaverFile::readThrowsPropagates() {
    {
        QFile f(m_filePath);
        QVERIFY(f.open(QIODevice::WriteOnly));
        f.write("anything");
    }

    SaverFile<ThrowingData> saver(m_filePath, m_tempPath);
    ThrowingData out;

    QVERIFY_THROWS_EXCEPTION(RuntimeError, Q_UNUSED(saver.read(out)));
}

void TestSaverFile::readDoesNotModifyStorageOnFailure() {
    SaverFile<TestData> saver(m_filePath, m_tempPath);

    TestData out { 999, "unchanged" };
    QVERIFY(!saver.read(out));

    QCOMPARE(out.value, 999);
    QCOMPARE(out.text, QString("unchanged"));
}

// ---------- write() ----------

void TestSaverFile::writeWithoutPrepareReturnsTrue() {
    SaverFile<TestData> saver(m_filePath, m_tempPath);

    QVERIFY(saver.write());
    QVERIFY(!QFile::exists(m_filePath));
}

void TestSaverFile::writeWithoutTempFileReturnsFalse() {
    SaverFile<TestData> saver(m_filePath, m_tempPath);

    QVERIFY(saver.prepare(TestData { 1, "x" }));
    QVERIFY(QFile::remove(m_tempPath));

    QVERIFY(!saver.write());
}

void TestSaverFile::writeAfterPrepareMovesFile() {
    SaverFile<TestData> saver(m_filePath, m_tempPath);

    QVERIFY(saver.prepare(TestData { 5, "move" }));
    QVERIFY(!QFile::exists(m_filePath));

    QVERIFY(saver.write());
    QVERIFY(QFile::exists(m_filePath));
}

void TestSaverFile::writeRemovesTempFile() {
    SaverFile<TestData> saver(m_filePath, m_tempPath);

    QVERIFY(saver.prepare(TestData { 5, "x" }));
    QVERIFY(saver.write());

    QVERIFY(!QFile::exists(m_tempPath));
}

void TestSaverFile::writeTwiceIsIdempotent() {
    SaverFile<TestData> saver(m_filePath, m_tempPath);

    QVERIFY(saver.prepare(TestData { 1, "a" }));
    QVERIFY(saver.write());

    QVERIFY(saver.write());
    QVERIFY(QFile::exists(m_filePath));
}

// ---------- Full Cycle ----------

void TestSaverFile::fullCyclePrepareWriteRead() {
    SaverFile<TestData> saver(m_filePath, m_tempPath);

    const TestData a { 10, "alpha" };
    const TestData b { 20, "beta" };

    QVERIFY(saver.prepare(a));
    QVERIFY(saver.write());

    TestData out;
    QVERIFY(saver.read(out));
    QCOMPARE(out, a);

    QVERIFY(saver.prepare(b));
    QVERIFY(saver.write());

    QVERIFY(saver.read(out));
    QCOMPARE(out, b);
}

// ---------- Destructor ----------

void TestSaverFile::destructorRemovesTempFile() {
    {
        SaverFile<TestData> saver(m_filePath, m_tempPath);
        QVERIFY(saver.prepare(TestData { 1, "x" }));
        QVERIFY(QFile::exists(m_tempPath));
    }

    QVERIFY(!QFile::exists(m_tempPath));
}

QTEST_APPLESS_MAIN(TestSaverFile)
#include "test_SaverFile.moc"
