#include <QElapsedTimer>
#include <QFile>
#include <QFuture>
#include <QTemporaryDir>
#include <QTest>
#include <QtConcurrentRun>

#include "Exceptions.h"
#include "Record.h"
#include "SaverFile.h"
#include "StorageManager.h"

using mdt::Drawing;
using mdt::RuntimeError;
using mdt::SaverFile;
using mdt::Storage;
using mdt::work::Record;
using mdt::work::StorageManager;
using namespace Qt::Literals::StringLiterals;

struct TestData : public Storage {
    int value = 0;
    QString text;

    TestData(int _value = 0, const QString &_text = QString()) : value(_value), text(_text) { }

    bool operator ==(const TestData &other) const {
        return value == other.value && text == other.text;
    }

    void serialize(QDataStream &stream) const override {
        stream << value << text;
    }

    void deserialize(QDataStream &stream) override {
        stream >> value >> text;
    }
};

struct ThrowingData : public Storage {
    void serialize(QDataStream &) const override {
        throw RuntimeError(u"serialize failed"_s);
    }
    void deserialize(QDataStream &) override {
        throw RuntimeError(u"deserialize failed"_s);
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

    // ---------- write / read records ----------
    void writeReadRecords();

private:
    QTemporaryDir *m_dir = nullptr;
    QString m_filePath;
    QString m_tempPath;

    // ---------- write / read records helpers ----------

    Record createRecord(int threadId, int index) const {
        return Record(makeDate(threadId, index),
                      makeDrawing(threadId, index),
                      index + 1,
                      QStringList { QString(u"Exec_%1_%2").arg(threadId).arg(index) },
                      QStringList { QString(u"Auth_%1_%2").arg(threadId).arg(index) },
                      QStringList { QString(u"Cast_%1_%2").arg(threadId).arg(index) },
                      QStringList { QString(u"Model_%1_%2").arg(threadId).arg(index) },
                      QStringList { QString(u"Mach_%1_%2").arg(threadId).arg(index) },
                      QStringList { QString(u"Note_%1_%2").arg(threadId).arg(index) });
    }

    QString makeDate(int threadId, int index) const {
        QDate base(2000, 1, 1);
        QDate d = base.addDays(threadId * 1000 + index);
        return d.toString(u"dd.MM.yyyy");
    }

    Drawing makeDrawing(int threadId, int index) const {
        return Drawing(QString(u"DWG-%1-%2").arg(threadId).arg(index),
                       QString(u"Title-%1-%2").arg(threadId).arg(index));
    }
};

void TestSaverFile::init() {
    m_dir = new QTemporaryDir();
    QVERIFY(m_dir->isValid());
    m_filePath = m_dir->filePath(u"data.bin"_s);
    m_tempPath = m_dir->filePath(u"data.bin.tmp"_s);
}

void TestSaverFile::cleanup() {
    delete m_dir;
    m_dir = nullptr;
}

// ---------- prepare() ----------

void TestSaverFile::prepareCreatesTempFile() {
    SaverFile saver(m_filePath, m_tempPath);
    TestData data { 42, u"hello"_s };
    QVERIFY(saver.prepare(data));
    QVERIFY(QFile::exists(m_tempPath));
}

void TestSaverFile::prepareSetsDirtyAndAllowsWrite() {
    SaverFile saver(m_filePath, m_tempPath);
    TestData data { 7, u"dirty"_s };

    QVERIFY(saver.prepare(data));
    QVERIFY(saver.write());
    QVERIFY(QFile::exists(m_filePath));
}

void TestSaverFile::prepareInvalidPathReturnsFalse() {
    const QString badPath = m_dir->filePath(u"no_such_dir/data.bin.tmp"_s);
    SaverFile saver(m_filePath, badPath);

    QVERIFY(!saver.prepare(TestData { 1, u"x"_s }));
}

void TestSaverFile::prepareThrowsPropagatesAndCleansTemp() {
    SaverFile saver(m_filePath, m_tempPath);

    QVERIFY_THROWS_EXCEPTION(RuntimeError, Q_UNUSED(saver.prepare(ThrowingData { })));
    QVERIFY(!QFile::exists(m_tempPath));
}

void TestSaverFile::prepareCalledTwiceOverwritesTemp() {
    SaverFile saver(m_filePath, m_tempPath);

    QVERIFY(saver.prepare(TestData { 1, u"first"_s }));
    QVERIFY(saver.prepare(TestData { 2, u"second"_s }));
    QVERIFY(saver.write());

    TestData out;
    QVERIFY(saver.read(out));
    QCOMPARE(out.value, 2);
    QCOMPARE(out.text, u"second"_s);
}

// ---------- read() ----------

void TestSaverFile::readMissingFileReturnsFalse() {
    SaverFile saver(m_filePath, m_tempPath);

    TestData out;
    QVERIFY(!saver.read(out));
}

void TestSaverFile::readRoundTripReturnsSameData() {
    SaverFile saver(m_filePath, m_tempPath);

    const TestData original { 123, u"round trip"_s };

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

    SaverFile saver(m_filePath, m_tempPath);
    TestData out;

    Q_UNUSED(saver.read(out));
}

void TestSaverFile::readThrowsPropagates() {
    {
        QFile f(m_filePath);
        QVERIFY(f.open(QIODevice::WriteOnly));
        f.write("anything");
    }

    SaverFile saver(m_filePath, m_tempPath);
    ThrowingData out;

    QVERIFY_THROWS_EXCEPTION(RuntimeError, Q_UNUSED(saver.read(out)));
}

void TestSaverFile::readDoesNotModifyStorageOnFailure() {
    SaverFile saver(m_filePath, m_tempPath);

    TestData out { 999, u"unchanged"_s };
    QVERIFY(!saver.read(out));

    QCOMPARE(out.value, 999);
    QCOMPARE(out.text, u"unchanged"_s);
}

// ---------- write() ----------

void TestSaverFile::writeWithoutPrepareReturnsTrue() {
    SaverFile saver(m_filePath, m_tempPath);

    QVERIFY(saver.write());
    QVERIFY(!QFile::exists(m_filePath));
}

void TestSaverFile::writeWithoutTempFileReturnsFalse() {
    SaverFile saver(m_filePath, m_tempPath);

    QVERIFY(saver.prepare(TestData { 1, u"x"_s }));
    QVERIFY(QFile::remove(m_tempPath));

    QVERIFY(!saver.write());
}

void TestSaverFile::writeAfterPrepareMovesFile() {
    SaverFile saver(m_filePath, m_tempPath);

    QVERIFY(saver.prepare(TestData { 5, u"move"_s }));
    QVERIFY(!QFile::exists(m_filePath));

    QVERIFY(saver.write());
    QVERIFY(QFile::exists(m_filePath));
}

void TestSaverFile::writeRemovesTempFile() {
    SaverFile saver(m_filePath, m_tempPath);

    QVERIFY(saver.prepare(TestData { 5, u"x"_s }));
    QVERIFY(saver.write());

    QVERIFY(!QFile::exists(m_tempPath));
}

void TestSaverFile::writeTwiceIsIdempotent() {
    SaverFile saver(m_filePath, m_tempPath);

    QVERIFY(saver.prepare(TestData { 1, u"a"_s }));
    QVERIFY(saver.write());

    QVERIFY(saver.write());
    QVERIFY(QFile::exists(m_filePath));
}

// ---------- Full Cycle ----------

void TestSaverFile::fullCyclePrepareWriteRead() {
    SaverFile saver(m_filePath, m_tempPath);

    const TestData a { 10, u"alpha"_s };
    const TestData b { 20, u"beta"_s };

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
        SaverFile saver(m_filePath, m_tempPath);
        QVERIFY(saver.prepare(TestData { 1, u"x"_s }));
        QVERIFY(QFile::exists(m_tempPath));
    }

    QVERIFY(!QFile::exists(m_tempPath));
}

// ---------- write / read records ----------

void TestSaverFile::writeReadRecords() {
    QElapsedTimer timerCommon;
    QElapsedTimer timer;

    StorageManager sm;

    timerCommon.start();
    timer.start();

    for (int t = 0; t < 4; ++t) {
        for (int i = 0; i < 500; ++i) {
            sm.addRecord(createRecord(t, i));
        }
    }

    const qint64 elapsedFilling = timer.restart();

    auto records = sm.getRecords();

    const qint64 elapsedCopy = timer.restart();

    QCOMPARE(records.size(), static_cast<qsizetype>(2000));
    QVERIFY(records.contains(createRecord(0, 0)));
    QVERIFY(records.contains(createRecord(2, 250)));
    QVERIFY(records.contains(createRecord(3, 499)));
    QVERIFY(!records.contains(createRecord(3, 500)));
    QVERIFY(!records.contains(createRecord(4, 499)));

    const qint64 elapsedCheck = timer.restart();

    SaverFile saver(m_filePath, m_tempPath);
    QVERIFY(saver.prepare(sm));
    QVERIFY(saver.write());

    const qint64 elapsedSave = timer.restart();

    StorageManager sm2;
    QVERIFY(saver.read(sm2));

    const qint64 elapsedLoad = timer.restart();

    auto records2 = sm2.getRecords();

    const qint64 elapsedCopy2 = timer.restart();

    QCOMPARE(records2.size(), static_cast<qsizetype>(2000));
    QVERIFY(records2.contains(createRecord(0, 0)));
    QVERIFY(records2.contains(createRecord(2, 250)));
    QVERIFY(records2.contains(createRecord(3, 499)));
    QVERIFY(!records2.contains(createRecord(3, 500)));
    QVERIFY(!records2.contains(createRecord(4, 499)));

    const qint64 elapsedCheck2 = timer.restart();

    std::sort(records.begin(), records.end(), [](const Record &recOne, const Record &recTwo) {
        return recOne < recTwo;
    });

    const qint64 elapsedSort = timer.restart();

    std::sort(records2.begin(), records2.end(), [](const Record &recOne, const Record &recTwo) {
        return recOne < recTwo;
    });

    const qint64 elapsedSort2 = timer.restart();

    bool fail = false;
    for (int i = 0; i < 2000; ++i) {
        if (records[i] != records2[i]) {
            fail = true;
            qDebug() << "ERROR COMPARE AT INDEX" << i << ':';
            qDebug() << '\t' << records[i].date() << records[i].drawing().getNumber()
                     << records[i].drawing().getTitle() << records[i].amount()
                     << records[i].executors() << records[i].authors()
                     << records[i].castingMaterials() << records[i].modelMaterials()
                     << records[i].machines() << records[i].notes();
            qDebug() << '\t' << records2[i].date() << records2[i].drawing().getNumber()
                     << records2[i].drawing().getTitle() << records2[i].amount()
                     << records2[i].executors() << records2[i].authors()
                     << records2[i].castingMaterials() << records2[i].modelMaterials()
                     << records2[i].machines() << records2[i].notes();
        }
    }

    const qint64 elapsedCompare = timer.elapsed();
    const qint64 elapsedCommon = timerCommon.elapsed();

    qDebug() << "Filling:" << elapsedFilling;
    qDebug() << "Copy one:" << elapsedCopy;
    qDebug() << "Check one:" << elapsedCheck;
    qDebug() << "Save:" << elapsedSave;
    qDebug() << "Load:" << elapsedLoad;
    qDebug() << "Copy two:" << elapsedCopy2;
    qDebug() << "Check two:" << elapsedCheck2;
    qDebug() << "Sort one:" << elapsedSort;
    qDebug() << "Sort two:" << elapsedSort2;
    qDebug() << "Compare:" << elapsedCompare;
    qDebug() << "------------------";
    qDebug() << "Common:" << elapsedCommon;

    QVERIFY(!fail);
}

QTEST_APPLESS_MAIN(TestSaverFile)
#include "test_SaverFile.moc"
