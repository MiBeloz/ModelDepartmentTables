#include <QByteArray>
#include <QDataStream>
#include <QSet>
#include <QThread>
#include <QtConcurrent/QtConcurrent>
#include <QtTest>

#include "RecordLink.h"

class TestRecordLink : public QObject {
    Q_OBJECT

private slots:
    // ---------- Constructors / Assignment ----------
    void defaultConstructorCreatesNullLikeObject();
    void constructorInitializesAllFields();
    void copyConstructorCopiesAllFields();
    void moveConstructorMovesAndResetsSource();

    // ---------- Setter ----------
    void setterSetsIds();
    void setterChaining();

    // ---------- Adder ----------
    void adderUnitesSets();
    void adderChaining();

    // ---------- Remover ----------
    void removerSubtractsSets();
    void removerChaining();

    // ---------- Getter ----------
    void getterReturnsAllFields();

    // ---------- Comparison operators ----------
    void operatorEqualEqualObjects();
    void operatorEqualDifferentObjects();
    void operatorNotEqual();
    void operatorAssignCopy();
    void operatorAssignMove();
    void operatorAssignSelfAssignment();

    // ---------- swap ----------
    void swapSwapsAllFields();
    void swapSelfSwap();

    // ---------- qHash ----------
    void qHashEqualObjectsHaveEqualHash();
    void qHashDifferentObjectsHaveDifferentHash();

    // ---------- serialize / deserialize ----------
    void serializeDeserializeRoundTrip();
    void deserializeWrongVersionThrows();
    void deserializeNegativeIdThrows();
    void deserializeNegativeSetSizeThrows();
    void deserializeTruncatedStreamThrows();

    // ---------- Thread safety(basic check for crashes) ----------
    void concurrentReadsAndWritesDoNotCrash();
};

// ---------- Helper functions ----------

static RecordLink makeSample() {
    return RecordLink(1,
                      2,
                      3,
                      QSet<qint32> { 10, 11, 12 },
                      QSet<qint32> { 20, 21 },
                      QSet<qint32> { 30 },
                      QSet<qint32> { 40, 41, 42, 43 },
                      QSet<qint32> { 50, 51 },
                      QSet<qint32> { 60 });
}

// ---------- Constructors / Assignment ----------

void TestRecordLink::defaultConstructorCreatesNullLikeObject() {
    RecordLink link = RecordLink::Null;

    QCOMPARE(link.get().idDate(), 0);
    QCOMPARE(link.get().idDrawing(), 0);
    QCOMPARE(link.get().idAmount(), 0);
    QVERIFY(link.get().idExecutors().isEmpty());
    QVERIFY(link.get().idAuthors().isEmpty());
    QVERIFY(link.get().idCastingMaterials().isEmpty());
    QVERIFY(link.get().idModelMaterials().isEmpty());
    QVERIFY(link.get().idMachines().isEmpty());
    QVERIFY(link.get().idNotes().isEmpty());
}

void TestRecordLink::constructorInitializesAllFields() {
    const RecordLink r = makeSample();
    const RecordLink::Getter g = r.get();

    QCOMPARE(g.idDate(), 1);
    QCOMPARE(g.idDrawing(), 2);
    QCOMPARE(g.idAmount(), 3);
    QCOMPARE(g.idExecutors(), (QSet<qint32> { 10, 11, 12 }));
    QCOMPARE(g.idAuthors(), (QSet<qint32> { 20, 21 }));
    QCOMPARE(g.idCastingMaterials(), (QSet<qint32> { 30 }));
    QCOMPARE(g.idModelMaterials(), (QSet<qint32> { 40, 41, 42, 43 }));
    QCOMPARE(g.idMachines(), (QSet<qint32> { 50, 51 }));
    QCOMPARE(g.idNotes(), (QSet<qint32> { 60 }));
}

void TestRecordLink::copyConstructorCopiesAllFields() {
    const RecordLink original = makeSample();
    const RecordLink copy(original);
    QVERIFY(copy == original);

    RecordLink mutableCopy(copy);
    mutableCopy.set().idDate(999);
    QCOMPARE(original.get().idDate(), 1);
    QCOMPARE(mutableCopy.get().idDate(), 999);
}

void TestRecordLink::moveConstructorMovesAndResetsSource() {
    RecordLink source = makeSample();
    const RecordLink moved(std::move(source));

    QCOMPARE(moved.get().idDate(), 1);
    QCOMPARE(moved.get().idDrawing(), 2);
    QCOMPARE(moved.get().idAmount(), 3);
    QCOMPARE(moved.get().idExecutors(), (QSet<qint32> { 10, 11, 12 }));
    QCOMPARE(moved.get().idAuthors(), (QSet<qint32> { 20, 21 }));
    QCOMPARE(moved.get().idCastingMaterials(), (QSet<qint32> { 30 }));
    QCOMPARE(moved.get().idModelMaterials(), (QSet<qint32> { 40, 41, 42, 43 }));
    QCOMPARE(moved.get().idMachines(), (QSet<qint32> { 50, 51 }));
    QCOMPARE(moved.get().idNotes(), (QSet<qint32> { 60 }));

    QCOMPARE(source.get().idDate(), 0);
    QCOMPARE(source.get().idDrawing(), 0);
    QCOMPARE(source.get().idAmount(), 0);
    QCOMPARE(source.get().idExecutors(), QSet<qint32>());
    QCOMPARE(source.get().idAuthors(), QSet<qint32>());
    QCOMPARE(source.get().idCastingMaterials(), QSet<qint32>());
    QCOMPARE(source.get().idModelMaterials(), QSet<qint32>());
    QCOMPARE(source.get().idMachines(), QSet<qint32>());
    QCOMPARE(source.get().idNotes(), QSet<qint32>());

    QCOMPARE(source, RecordLink::Null);
}

// ---------- Setter ----------

void TestRecordLink::setterSetsIds() {
    RecordLink r = RecordLink::Null;

    r.set().idDate(100).idDrawing(200).idAmount(300);

    QCOMPARE(r.get().idDate(), 100);
    QCOMPARE(r.get().idDrawing(), 200);
    QCOMPARE(r.get().idAmount(), 300);
}

void TestRecordLink::setterChaining() {
    RecordLink r = RecordLink::Null;
    RecordLink::Setter s = r.set();
    s.idDate(7).idDrawing(8).idAmount(9);
    QCOMPARE(r.get().idDate(), 7);
    QCOMPARE(r.get().idDrawing(), 8);
    QCOMPARE(r.get().idAmount(), 9);
}

// ---------- Adder ----------

void TestRecordLink::adderUnitesSets() {
    RecordLink r = RecordLink::Null;

    r.add()
        .idExecutors({ 1, 2 })
        .idAuthors({ 3, 4 })
        .idCastingMaterials({ 5 })
        .idModelMaterials({ 6, 7 })
        .idMachines({ 8 })
        .idNotes({ 9 });

    QCOMPARE(r.get().idExecutors(), (QSet<qint32> { 1, 2 }));
    QCOMPARE(r.get().idAuthors(), (QSet<qint32> { 3, 4 }));
    QCOMPARE(r.get().idCastingMaterials(), (QSet<qint32> { 5 }));
    QCOMPARE(r.get().idModelMaterials(), (QSet<qint32> { 6, 7 }));
    QCOMPARE(r.get().idMachines(), (QSet<qint32> { 8 }));
    QCOMPARE(r.get().idNotes(), (QSet<qint32> { 9 }));

    r.add().idExecutors({ 2, 3 });
    QCOMPARE(r.get().idExecutors(), QSet<qint32>({ 1, 2, 3 }));
}

void TestRecordLink::adderChaining() {
    RecordLink r = RecordLink::Null;
    RecordLink::Adder a = r.add();
    a.idExecutors({ 1 }).idAuthors({ 2 }).idNotes({ 3, 4 });
    QCOMPARE(r.get().idExecutors(), (QSet<qint32> { 1 }));
    QCOMPARE(r.get().idAuthors(), (QSet<qint32> { 2 }));
    QCOMPARE(r.get().idNotes(), (QSet<qint32> { 3, 4 }));
}

// ---------- Remover ----------

void TestRecordLink::removerSubtractsSets() {
    RecordLink r = makeSample();

    r.remove()
        .removeExecutors({ 10 })
        .removeAuthors({ 20 })
        .removeCastingMaterials({ 30 })
        .removeModelMaterials({ 40, 41 })
        .removeMachines({ 50 })
        .removeNotes({ 60 });

    QCOMPARE(r.get().idExecutors(), (QSet<qint32> { 11, 12 }));
    QCOMPARE(r.get().idAuthors(), (QSet<qint32> { 21 }));
    QCOMPARE(r.get().idCastingMaterials(), (QSet<qint32> { }));
    QCOMPARE(r.get().idModelMaterials(), (QSet<qint32> { 42, 43 }));
    QCOMPARE(r.get().idMachines(), (QSet<qint32> { 51 }));
    QCOMPARE(r.get().idNotes(), (QSet<qint32> { }));

    r.remove().removeExecutors({ 999 });
    QCOMPARE(r.get().idExecutors(), (QSet<qint32> { 11, 12 }));
}

void TestRecordLink::removerChaining() {
    RecordLink r = makeSample();
    RecordLink::Remover rm = r.remove();
    rm.removeExecutors({ 10, 11, 12 }).removeAuthors({ 20, 21 });
    QVERIFY(r.get().idExecutors().isEmpty());
    QVERIFY(r.get().idAuthors().isEmpty());
}

// ---------- Getter ----------

void TestRecordLink::getterReturnsAllFields() {
    const RecordLink r = makeSample();
    const auto g = r.get();

    QCOMPARE(g.idDate(), 1);
    QCOMPARE(g.idDrawing(), 2);
    QCOMPARE(g.idAmount(), 3);
    QCOMPARE(g.idExecutors(), (QSet<qint32> { 10, 11, 12 }));
    QCOMPARE(g.idAuthors(), (QSet<qint32> { 20, 21 }));
    QCOMPARE(g.idCastingMaterials(), (QSet<qint32> { 30 }));
    QCOMPARE(g.idModelMaterials(), (QSet<qint32> { 40, 41, 42, 43 }));
    QCOMPARE(g.idMachines(), (QSet<qint32> { 50, 51 }));
    QCOMPARE(g.idNotes(), (QSet<qint32> { 60 }));
}

// ---------- Comparison operators ----------

void TestRecordLink::operatorEqualEqualObjects() {
    const RecordLink a = makeSample();
    const RecordLink b = makeSample();
    QVERIFY(a == b);
    QVERIFY(!(a != b));
}

void TestRecordLink::operatorEqualDifferentObjects() {
    RecordLink a = makeSample();
    RecordLink b = makeSample();
    b.set().idDate(999);
    QVERIFY(a != b);
    QVERIFY(!(a == b));
}

void TestRecordLink::operatorNotEqual() {
    RecordLink a = makeSample();
    RecordLink b = makeSample();
    b.add().idExecutors({ 999 });
    QVERIFY(a != b);
}

void TestRecordLink::operatorAssignCopy() {
    RecordLink a = makeSample();
    RecordLink b = RecordLink::Null;

    b = a;
    QVERIFY(a == b);

    b.set().idDate(777);
    QCOMPARE(a.get().idDate(), 1);
}

void TestRecordLink::operatorAssignMove() {
    RecordLink a = makeSample();
    RecordLink b = RecordLink::Null;

    b = std::move(a);

    QCOMPARE(b.get().idDate(), 1);
    QCOMPARE(b.get().idExecutors(), (QSet<qint32> { 10, 11, 12 }));
    QCOMPARE(a, RecordLink::Null);
}

void TestRecordLink::operatorAssignSelfAssignment() {
    RecordLink a = makeSample();
    a = a;
    QCOMPARE(a.get().idDate(), 1);
    QCOMPARE(a.get().idExecutors(), (QSet<qint32> { 10, 11, 12 }));
}

// ---------- swap ----------

void TestRecordLink::swapSwapsAllFields() {
    RecordLink a = makeSample();
    RecordLink b = RecordLink::Null;

    const RecordLink aCopy = a;
    const RecordLink bCopy = b;

    a.swap(b);
    QCOMPARE(a, bCopy);
    QCOMPARE(b, aCopy);
}

void TestRecordLink::swapSelfSwap() {
    RecordLink a = makeSample();
    a.swap(a);
    QCOMPARE(a.get().idDate(), 1);
    QCOMPARE(a.get().idExecutors(), (QSet<qint32> { 10, 11, 12 }));
}

// ---------- qHash ----------

void TestRecordLink::qHashEqualObjectsHaveEqualHash() {
    const RecordLink a = makeSample();
    const RecordLink b = makeSample();
    QCOMPARE(a.hash(), b.hash());
    QCOMPARE(qHash(a), qHash(b));
}

void TestRecordLink::qHashDifferentObjectsHaveDifferentHash() {
    RecordLink a = makeSample();
    RecordLink b = makeSample();
    b.set().idDate(12345);
    QVERIFY(a.hash() != b.hash());
}

// ---------- serialize / deserialize ----------

void TestRecordLink::serializeDeserializeRoundTrip() {
    const RecordLink original = makeSample();

    QByteArray data;
    {
        QDataStream out(&data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Version::Qt_6_11);
        original.serialize(out);
    }

    RecordLink restored(RecordLink::Null);
    {
        QDataStream in(&data, QIODevice::ReadOnly);
        in.setVersion(QDataStream::Version::Qt_6_11);
        restored.deserialize(in);
    }

    QVERIFY(original == restored);
    QCOMPARE(original.hash(), restored.hash());
}

void TestRecordLink::deserializeWrongVersionThrows() {
    QByteArray data;
    {
        QDataStream out(&data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Version::Qt_4_0);
    }

    RecordLink r = RecordLink::Null;
    QDataStream in(&data, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Version::Qt_6_11);

    QVERIFY_THROWS_EXCEPTION(RuntimeError, r.deserialize(in));
}

void TestRecordLink::deserializeNegativeIdThrows() {
    QByteArray data;
    {
        QDataStream out(&data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Version::Qt_6_11);
        out << out.version();
        out << static_cast<qint32>(-1);
    }

    RecordLink r = RecordLink::Null;
    QDataStream in(&data, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Version::Qt_6_11);

    QVERIFY_THROWS_EXCEPTION(RuntimeError, r.deserialize(in));
}

void TestRecordLink::deserializeNegativeSetSizeThrows() {
    QByteArray data;
    {
        QDataStream out(&data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Version::Qt_6_11);
        out << out.version();
        out << static_cast<qint32>(0) << static_cast<qint32>(0) << static_cast<qint32>(0);
        out << static_cast<qint32>(-5);
    }

    RecordLink r = RecordLink::Null;
    QDataStream in(&data, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Version::Qt_6_11);

    QVERIFY_THROWS_EXCEPTION(RuntimeError, r.deserialize(in));
}

void TestRecordLink::deserializeTruncatedStreamThrows() {
    QByteArray data;
    {
        QDataStream out(&data, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Version::Qt_6_11);
        out << out.version();
        out << static_cast<qint32>(1);
    }

    RecordLink r = RecordLink::Null;
    QDataStream in(&data, QIODevice::ReadOnly);
    in.setVersion(QDataStream::Version::Qt_6_11);

    QVERIFY_THROWS_EXCEPTION(RuntimeError, r.deserialize(in));
}

// ---------- Thread safety(basic check for crashes) ----------

void TestRecordLink::concurrentReadsAndWritesDoNotCrash() {
    constexpr int threadCount = 8;
    constexpr int opsPerThread = 3000;
    constexpr int timeoutMs = 60000;

    RecordLink link(RecordLink::Null);

    QMutex startMutex;
    QWaitCondition startCondition;
    bool startFlag = false;

    QVector<QThread*> threads;
    threads.reserve(threadCount);

    for (int t = 0; t < threadCount; ++t) {
        QThread* thread = QThread::create([&, t]() {
            {
                QMutexLocker locker(&startMutex);
                while (!startFlag) {
                    startCondition.wait(&startMutex);
                }
            }

            QRandomGenerator rng(static_cast<quint32>(0xBADC0DE + t));

            for (int i = 0; i < opsPerThread; ++i) {
                const int op = rng.bounded(100);
                const qint32 id = static_cast<qint32>(rng.bounded(50));
                const qint32 id2 = static_cast<qint32>(rng.bounded(50));
                const qint32 id3 = static_cast<qint32>(rng.bounded(50));

                QSet<qint32> ids;
                const int setSize = rng.bounded(1, 6);
                for (int k = 0; k < setSize; ++k) {
                    ids.insert(static_cast<qint32>(rng.bounded(50)));
                }

                if (op < 15) {
                    // set
                    link.set().idDate(id).idDrawing(id2).idAmount(id3);
                } else if (op < 30) {
                    // add
                    link.add()
                        .idExecutors(ids)
                        .idAuthors(ids)
                        .idCastingMaterials(ids)
                        .idModelMaterials(ids)
                        .idMachines(ids)
                        .idNotes(ids);
                } else if (op < 45) {
                    // remove
                    link.remove()
                        .removeExecutors(ids)
                        .removeAuthors(ids)
                        .removeCastingMaterials(ids)
                        .removeModelMaterials(ids)
                        .removeMachines(ids)
                        .removeNotes(ids);
                } else if (op < 60) {
                    // get
                    (void)link.get().idDate();
                    (void)link.get().idDrawing();
                    (void)link.get().idAmount();
                    (void)link.get().idExecutors();
                    (void)link.get().idAuthors();
                    (void)link.get().idCastingMaterials();
                    (void)link.get().idModelMaterials();
                    (void)link.get().idMachines();
                    (void)link.get().idNotes();
                } else if (op < 70) {
                    RecordLink other(1, 2, 3, ids, ids, ids, ids, ids, ids);
                    (void)(link == other);
                    (void)(link != other);
                    (void)link.hash();
                    (void)qHash(link);
                } else if (op < 80) {
                    // swap
                    RecordLink other(id, id2, id3, ids, ids, ids, ids, ids, ids);
                    link.swap(other);
                } else if (op < 88) {
                    RecordLink copy(link);
                    (void)copy.get().idDate();
                } else if (op < 93) {
                    RecordLink moved(std::move(link));
                    // moved
                    link = std::move(moved);
                } else if (op < 97) {
                    // serialize
                    QByteArray bytes;
                    QDataStream out(&bytes, QIODevice::WriteOnly);
                    out.setVersion(QDataStream::Qt_6_0);
                    link.serialize(out);
                } else {
                    // deserialize
                    QByteArray bytes;
                    QDataStream out(&bytes, QIODevice::WriteOnly);
                    out.setVersion(QDataStream::Qt_6_0);
                    link.serialize(out);

                    QDataStream in(&bytes, QIODevice::ReadOnly);
                    in.setVersion(QDataStream::Qt_6_0);
                    link.deserialize(in);
                }
            }
        });
        threads.append(thread);
    }

    QElapsedTimer timer;
    timer.start();

    {
        QMutexLocker locker(&startMutex);
        startFlag = true;
        startCondition.wakeAll();
    }

    for (QThread* thread : threads) {
        thread->start();
    }

    for (QThread* thread : threads) {
        QVERIFY2(thread->wait(timeoutMs),
                 "Поток не завершился за отведённое время — возможен дедлок");
        delete thread;
    }
}

QTEST_MAIN(TestRecordLink)
#include "test_RecordLink.moc"
