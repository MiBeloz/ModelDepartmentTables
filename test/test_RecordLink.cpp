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
    void defaultConstructor_createsNullLikeObject();
    void constructor_initializesAllFields();
    void copyConstructor_copiesAllFields();
    void moveConstructor_movesAndResetsSource();

    // ---------- Setter ----------
    void setter_setsIds();
    void setter_chaining();

    // ---------- Adder ----------
    void adder_unitesSets();
    void adder_chaining();

    // ---------- Remover ----------
    void remover_subtractsSets();
    void remover_chaining();

    // ---------- Getter ----------
    void getter_returnsAllFields();

    // ---------- Comparison operators ----------
    void operatorEqual_equalObjects();
    void operatorEqual_differentObjects();
    void operatorNotEqual();
    void operatorAssign_copy();
    void operatorAssign_move();
    void operatorAssign_selfAssignment();

    // ---------- swap ----------
    void swap_swapsAllFields();
    void swap_selfSwap();

    // ---------- qHash ----------
    void qHash_equalObjectsHaveEqualHash();
    void qHash_differentObjectsHaveDifferentHash();

    // ---------- serialize / deserialize ----------
    void serialize_deserialize_roundTrip();
    void deserialize_wrongVersion_throws();
    void deserialize_negativeId_throws();
    void deserialize_negativeSetSize_throws();
    void deserialize_truncatedStream_throws();

    // ---------- Thread safety(basic check for crashes) ----------
    void concurrentReadsAndWrites_doNotCrash();
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

void TestRecordLink::defaultConstructor_createsNullLikeObject() {
    RecordLink link;

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

void TestRecordLink::constructor_initializesAllFields() {
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

void TestRecordLink::copyConstructor_copiesAllFields() {
    const RecordLink original = makeSample();
    const RecordLink copy(original);

    QVERIFY(copy == original);
    // Изменение копии не влияет на оригинал
    RecordLink mutableCopy(copy);
    mutableCopy.set().idDate(999);
    QCOMPARE(original.get().idDate(), 1);
    QCOMPARE(mutableCopy.get().idDate(), 999);
}

void TestRecordLink::moveConstructor_movesAndResetsSource() {
    RecordLink source = makeSample();
    const RecordLink moved(std::move(source));

    QCOMPARE(moved.get().idDate(), 1);
    QCOMPARE(moved.get().idDrawing(), 2);
    QCOMPARE(moved.get().idAmount(), 3);
    QCOMPARE(moved.get().idExecutors(), (QSet<qint32> { 10, 11, 12 }));

    // Источник должен быть сброшен (по контракту — только скалярные поля)
    QCOMPARE(source.get().idDate(), 0);
    QCOMPARE(source.get().idDrawing(), 0);
    QCOMPARE(source.get().idAmount(), 0);
}

// ---------- Setter ----------

void TestRecordLink::setter_setsIds() {
    RecordLink r = RecordLink::Null;

    r.set().idDate(100).idDrawing(200).idAmount(300);

    QCOMPARE(r.get().idDate(), 100);
    QCOMPARE(r.get().idDrawing(), 200);
    QCOMPARE(r.get().idAmount(), 300);
}

void TestRecordLink::setter_chaining() {
    RecordLink r = RecordLink::Null;
    RecordLink::Setter s = r.set();
    s.idDate(7).idDrawing(8).idAmount(9);
    QCOMPARE(r.get().idDate(), 7);
    QCOMPARE(r.get().idDrawing(), 8);
    QCOMPARE(r.get().idAmount(), 9);
}

// ---------- Adder ----------

void TestRecordLink::adder_unitesSets() {
    RecordLink r = RecordLink::Null;

    r.add()
        .idExecutors({ 1, 2 })
        .idAuthors({ 3, 4 })
        .idCastingMaterials({ 5 })
        .idModelMaterials({ 6, 7 })
        .idMachines({ 8 })
        .idNotes({ 9 });

    // Повторное добавление — объединение
    r.add().idExecutors({ 2, 3 });

    QCOMPARE(r.get().idExecutors(), (QSet<qint32> { 1, 2, 3 }));
    QCOMPARE(r.get().idAuthors(), (QSet<qint32> { 3, 4 }));
    QCOMPARE(r.get().idCastingMaterials(), (QSet<qint32> { 5 }));
    QCOMPARE(r.get().idModelMaterials(), (QSet<qint32> { 6, 7 }));
    QCOMPARE(r.get().idMachines(), (QSet<qint32> { 8 }));
    QCOMPARE(r.get().idNotes(), (QSet<qint32> { 9 }));
}

void TestRecordLink::adder_chaining() {
    RecordLink r = RecordLink::Null;
    RecordLink::Adder a = r.add();
    a.idExecutors({ 1 }).idAuthors({ 2 }).idNotes({ 3 });
    QCOMPARE(r.get().idExecutors(), (QSet<qint32> { 1 }));
    QCOMPARE(r.get().idAuthors(), (QSet<qint32> { 2 }));
    QCOMPARE(r.get().idNotes(), (QSet<qint32> { 3 }));
}

// ---------- Remover ----------

void TestRecordLink::remover_subtractsSets() {
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
}

void TestRecordLink::remover_chaining() {
    RecordLink r = makeSample();
    RecordLink::Remover rm = r.remove();
    rm.removeExecutors({ 10, 11, 12 }).removeAuthors({ 20, 21 });
    QVERIFY(r.get().idExecutors().isEmpty());
    QVERIFY(r.get().idAuthors().isEmpty());
}

// ---------- Getter ----------

void TestRecordLink::getter_returnsAllFields() {
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

void TestRecordLink::operatorEqual_equalObjects() {
    const RecordLink a = makeSample();
    const RecordLink b = makeSample();
    QVERIFY(a == b);
    QVERIFY(!(a != b));
}

void TestRecordLink::operatorEqual_differentObjects() {
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

void TestRecordLink::operatorAssign_copy() {
    RecordLink a = makeSample();
    RecordLink b = RecordLink::Null;

    b = a;

    QVERIFY(a == b);
    // Изменение b не влияет на a
    b.set().idDate(777);
    QCOMPARE(a.get().idDate(), 1);
}

void TestRecordLink::operatorAssign_move() {
    RecordLink a = makeSample();
    RecordLink b = RecordLink::Null;

    b = std::move(a);

    QCOMPARE(b.get().idDate(), 1);
    QCOMPARE(b.get().idExecutors(), (QSet<qint32> { 10, 11, 12 }));
    QCOMPARE(a.get().idDate(), 0);
}

void TestRecordLink::operatorAssign_selfAssignment() {
    RecordLink a = makeSample();
    a = a;
    QCOMPARE(a.get().idDate(), 1);
    QCOMPARE(a.get().idExecutors(), (QSet<qint32> { 10, 11, 12 }));
}

// ---------- swap ----------

void TestRecordLink::swap_swapsAllFields() {
    RecordLink a = makeSample();
    RecordLink b = RecordLink::Null;

    a.swap(b);

    QCOMPARE(a.get().idDate(), 0);
    QCOMPARE(b.get().idDate(), 1);
    QCOMPARE(b.get().idExecutors(), (QSet<qint32> { 10, 11, 12 }));
}

void TestRecordLink::swap_selfSwap() {
    RecordLink a = makeSample();
    a.swap(a);
    QCOMPARE(a.get().idDate(), 1);
    QCOMPARE(a.get().idExecutors(), (QSet<qint32> { 10, 11, 12 }));
}

// ---------- qHash ----------

void TestRecordLink::qHash_equalObjectsHaveEqualHash() {
    const RecordLink a = makeSample();
    const RecordLink b = makeSample();
    QCOMPARE(a.hash(), b.hash());
    QCOMPARE(qHash(a), qHash(b));
}

void TestRecordLink::qHash_differentObjectsHaveDifferentHash() {
    RecordLink a = makeSample();
    RecordLink b = makeSample();
    b.set().idDate(12345);
    // Теоретически может совпасть, но с такими данными — крайне маловероятно
    QVERIFY(a.hash() != b.hash());
}

// ---------- serialize / deserialize ----------

void TestRecordLink::serialize_deserialize_roundTrip() {
    const RecordLink original = makeSample();

    QByteArray data;
    {
        QDataStream out(&data, QIODevice::WriteOnly);
        original.serialize(out);
    }

    RecordLink restored(RecordLink::Null);
    {
        QDataStream in(&data, QIODevice::ReadOnly);
        restored.deserialize(in);
    }

    QVERIFY(original == restored);
    QCOMPARE(original.hash(), restored.hash());
}

void TestRecordLink::deserialize_wrongVersion_throws() {
    QByteArray data;
    {
        QDataStream out(&data, QIODevice::WriteOnly);
        out << quint32(0xDEADBEEF); // заведомо неверная версия
    }

    RecordLink r = RecordLink::Null;
    QDataStream in(&data, QIODevice::ReadOnly);

    QVERIFY_EXCEPTION_THROWN(r.deserialize(in), RuntimeError);
}

void TestRecordLink::deserialize_negativeId_throws() {
    QByteArray data;
    {
        QDataStream out(&data, QIODevice::WriteOnly);
        out << REQUIRED_SERIALIZATION_VERSION;
        out << qint32(-1); // отрицательный idDate
    }

    RecordLink r = RecordLink::Null;
    QDataStream in(&data, QIODevice::ReadOnly);

    QVERIFY_EXCEPTION_THROWN(r.deserialize(in), RuntimeError);
}

void TestRecordLink::deserialize_negativeSetSize_throws() {
    QByteArray data;
    {
        QDataStream out(&data, QIODevice::WriteOnly);
        out << REQUIRED_SERIALIZATION_VERSION;
        out << qint32(0) << qint32(0) << qint32(0); // три скалярных id
        out << qint32(-5);                          // отрицательный размер первого множества
    }

    RecordLink r = RecordLink::Null;
    QDataStream in(&data, QIODevice::ReadOnly);

    QVERIFY_EXCEPTION_THROWN(r.deserialize(in), RuntimeError);
}

void TestRecordLink::deserialize_truncatedStream_throws() {
    QByteArray data;
    {
        QDataStream out(&data, QIODevice::WriteOnly);
        out << REQUIRED_SERIALIZATION_VERSION;
        out << qint32(1); // только часть данных
    }

    RecordLink r = RecordLink::Null;
    QDataStream in(&data, QIODevice::ReadOnly);

    QVERIFY_EXCEPTION_THROWN(r.deserialize(in), RuntimeError);
}

// ---------- Thread safety(basic check for crashes) ----------

void TestRecordLink::concurrentReadsAndWrites_doNotCrash() {
    RecordLink r = makeSample();
    constexpr int kIterations = 5000;

    QThread* writerThread = QThread::create([&r]() {
        for (int i = 0; i < kIterations; ++i) {
            r.set().idDate(i);
            r.add().idExecutors({ i });
            r.remove().removeExecutors({ i - 1 });
        }
    });

    QThread* readerThread = QThread::create([&r]() {
        for (int i = 0; i < kIterations; ++i) {
            const auto id = r.get().idDate();
            Q_UNUSED(id);
            const auto set = r.get().idExecutors();
            Q_UNUSED(set);
            r.hash();
        }
    });

    writerThread->start();
    readerThread->start();

    QVERIFY(writerThread->wait(10000));
    QVERIFY(readerThread->wait(10000));

    delete writerThread;
    delete readerThread;
}

QTEST_MAIN(TestRecordLink)
#include "test_RecordLink.moc"
