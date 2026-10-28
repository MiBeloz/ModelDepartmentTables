#include <QBuffer>
#include <QDataStream>
#include <QTest>

#include "Exceptions.h"
#include "RecordLink.h"

using mdt::RuntimeError;
using mdt::work::IdsList;
using mdt::work::RecordLink;

class TestRecordLink : public QObject {
    Q_OBJECT

private slots:
    // ---------- Constructors / Assignment ----------
    void defaultConstructorCreatesNull();
    void fullConstructorSetsAllFields();
    void NullIsEmpty();
    void copyConstructorDeepCopies();
    void copyAssignmentDeepCopies();
    void selfAssignmentNoop();
    void moveConstructorTransfersAndResets();
    void moveAssignmentTransfersAndResets();

    // ---------- Comparison operators ----------
    void equalitySameValues();
    void equalityDifferentIds();
    void equalityDifferentSets();
    void inequalityWorks();

    // ---------- swap ----------
    void swapExchangesState();
    void swapWithSelfNoop();

    // ---------- set ----------
    void setIdDateWorks();
    void setIdDrawingWorks();
    void setIdAmountWorks();

    // ---------- addIds / removeIds / replaceIds / getIds ----------
    void addIdsUnitesAllLists_data();
    void addIdsUnitesAllLists();
    void removeIdsSubtracts();
    void replaceIdsReplaces();
    void getIdsReturnsReferenceToInternalSet();

    // ---------- hash ----------
    void hashEqualForEqualObjects();
    void hashDiffersForDifferentObjects();

    // ---------- serialize / deserialize ----------
    void serializeDeserializeRoundTrip();
    void serializeDeserializeEmptyObject();
    void deserializeWrongVersionThrows();
    void deserializeNegativeSetSizeThrows();
    void deserializeTruncatedStreamThrows();
};

// ---------- Helpers ----------

static RecordLink makeSample() {
    return RecordLink(101,
                      202,
                      303,
                      QSet<qint32> { 1, 2, 3 },
                      QSet<qint32> { 4, 5 },
                      QSet<qint32> { 6 },
                      QSet<qint32> { 7, 8, 9 },
                      QSet<qint32> { 10 },
                      QSet<qint32> { 11, 12 });
}

static QByteArray serializeToBytes(const RecordLink& rl) {
    QByteArray bytes;
    QDataStream out(&bytes, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Version::Qt_6_11);
    rl.serialize(out);
    return bytes;
}

static RecordLink deserializeFromBytes(const QByteArray& bytes) {
    QDataStream in(bytes);
    in.setVersion(QDataStream::Version::Qt_6_11);
    RecordLink rl;
    rl.deserialize(in);
    return rl;
}

// ---------- Constructors / Assignment ----------

void TestRecordLink::defaultConstructorCreatesNull() {
    RecordLink rl;
    QCOMPARE(rl.getIdDate(), 0);
    QCOMPARE(rl.getIdDrawing(), 0);
    QCOMPARE(rl.getIdAmount(), 0);
    QVERIFY(rl.getIds(IdsList::Executors).isEmpty());
    QVERIFY(rl.getIds(IdsList::Authors).isEmpty());
    QVERIFY(rl.getIds(IdsList::CastingMaterials).isEmpty());
    QVERIFY(rl.getIds(IdsList::ModelMaterials).isEmpty());
    QVERIFY(rl.getIds(IdsList::Machines).isEmpty());
    QVERIFY(rl.getIds(IdsList::Notes).isEmpty());
}

void TestRecordLink::fullConstructorSetsAllFields() {
    RecordLink rl = makeSample();
    QCOMPARE(rl.getIdDate(), 101);
    QCOMPARE(rl.getIdDrawing(), 202);
    QCOMPARE(rl.getIdAmount(), 303);
    QCOMPARE(rl.getIds(IdsList::Executors), (QSet<qint32> { 1, 2, 3 }));
    QCOMPARE(rl.getIds(IdsList::Authors), (QSet<qint32> { 4, 5 }));
    QCOMPARE(rl.getIds(IdsList::CastingMaterials), (QSet<qint32> { 6 }));
    QCOMPARE(rl.getIds(IdsList::ModelMaterials), (QSet<qint32> { 7, 8, 9 }));
    QCOMPARE(rl.getIds(IdsList::Machines), (QSet<qint32> { 10 }));
    QCOMPARE(rl.getIds(IdsList::Notes), (QSet<qint32> { 11, 12 }));
}

void TestRecordLink::NullIsEmpty() {
    QCOMPARE(RecordLink::Null.getIdDate(), 0);
    QCOMPARE(RecordLink::Null.getIdDrawing(), 0);
    QCOMPARE(RecordLink::Null.getIdAmount(), 0);
    QVERIFY(RecordLink::Null.getIds(IdsList::Executors).isEmpty());
    QCOMPARE(RecordLink::Null, RecordLink { });
}

void TestRecordLink::copyConstructorDeepCopies() {
    RecordLink original = makeSample();
    RecordLink copy(original);

    QCOMPARE(copy, original);

    copy.addIds(IdsList::Executors, { 999 });
    QVERIFY(!original.getIds(IdsList::Executors).contains(999));
}

void TestRecordLink::copyAssignmentDeepCopies() {
    RecordLink original = makeSample();
    RecordLink other;
    other = original;

    QCOMPARE(other, original);

    other.setIdDate(777);
    QCOMPARE(original.getIdDate(), 101);
}

void TestRecordLink::selfAssignmentNoop() {
    RecordLink rl = makeSample();
    RecordLink& ref = rl;
    rl = ref;
    QCOMPARE(rl, makeSample());
}

void TestRecordLink::moveConstructorTransfersAndResets() {
    RecordLink source = makeSample();
    RecordLink moved(std::move(source));

    QCOMPARE(moved, makeSample());

    QCOMPARE(source.getIdDate(), 0);
    QCOMPARE(source.getIdDrawing(), 0);
    QCOMPARE(source.getIdAmount(), 0);
}

void TestRecordLink::moveAssignmentTransfersAndResets() {
    RecordLink source = makeSample();
    RecordLink target;
    target = std::move(source);

    QCOMPARE(target, makeSample());
    QCOMPARE(source.getIdDate(), 0);
    QCOMPARE(source.getIdDrawing(), 0);
    QCOMPARE(source.getIdAmount(), 0);
}

// ---------- Comparison operators ----------

void TestRecordLink::equalitySameValues() {
    QCOMPARE(makeSample(), makeSample());
}

void TestRecordLink::equalityDifferentIds() {
    RecordLink a = makeSample();
    RecordLink b = makeSample();
    b.setIdAmount(999);
    QVERIFY(a != b);
}

void TestRecordLink::equalityDifferentSets() {
    RecordLink a = makeSample();
    RecordLink b = makeSample();
    b.addIds(IdsList::Notes, { 12345 });
    QVERIFY(a != b);
}

void TestRecordLink::inequalityWorks() {
    RecordLink a = makeSample();
    RecordLink b;
    QVERIFY(a != b);
    QVERIFY(!(a == b));
}

// ---------- swap ----------

void TestRecordLink::swapExchangesState() {
    RecordLink a = makeSample();
    RecordLink b;
    b.setIdDate(1);

    a.swap(b);

    QCOMPARE(a.getIdDate(), 1);
    QVERIFY(a.getIds(IdsList::Executors).isEmpty());
    QCOMPARE(b, makeSample());
}

void TestRecordLink::swapWithSelfNoop() {
    RecordLink a = makeSample();
    a.swap(a);
    QCOMPARE(a, makeSample());
}

// ---------- set ----------

void TestRecordLink::setIdDateWorks() {
    RecordLink rl;
    rl.setIdDate(42);
    QCOMPARE(rl.getIdDate(), 42);
}

void TestRecordLink::setIdDrawingWorks() {
    RecordLink rl;
    rl.setIdDrawing(43);
    QCOMPARE(rl.getIdDrawing(), 43);
}

void TestRecordLink::setIdAmountWorks() {
    RecordLink rl;
    rl.setIdAmount(44);
    QCOMPARE(rl.getIdAmount(), 44);
}

// ---------- addIds / removeIds / replaceIds / getIds ----------

void TestRecordLink::addIdsUnitesAllLists_data() {
    QTest::addColumn<IdsList>("list");
    QTest::addColumn<QSet<qint32>>("initial");
    QTest::addColumn<QSet<qint32>>("toAdd");
    QTest::addColumn<QSet<qint32>>("expected");

    QTest::newRow("Executors") << IdsList::Executors << QSet<qint32> { 1 } << QSet<qint32> { 2, 3 }
                               << QSet<qint32> { 1, 2, 3 };
    QTest::newRow("Authors") << IdsList::Authors << QSet<qint32> { 10 } << QSet<qint32> { 20 }
                             << QSet<qint32> { 10, 20 };
    QTest::newRow("CastingMaterials") << IdsList::CastingMaterials << QSet<qint32> { }
                                      << QSet<qint32> { 5 } << QSet<qint32> { 5 };
    QTest::newRow("ModelMaterials") << IdsList::ModelMaterials << QSet<qint32> { 7 }
                                    << QSet<qint32> { 7 } << QSet<qint32> { 7 };
    QTest::newRow("Machines") << IdsList::Machines << QSet<qint32> { 1, 2 } << QSet<qint32> { 2, 3 }
                              << QSet<qint32> { 1, 2, 3 };
    QTest::newRow("Notes") << IdsList::Notes << QSet<qint32> { } << QSet<qint32> { }
                           << QSet<qint32> { };
}

void TestRecordLink::addIdsUnitesAllLists() {
    QFETCH(IdsList, list);
    QFETCH(QSet<qint32>, initial);
    QFETCH(QSet<qint32>, toAdd);
    QFETCH(QSet<qint32>, expected);

    RecordLink rl;
    rl.replaceIds(list, initial);
    rl.addIds(list, toAdd);

    QCOMPARE(rl.getIds(list), expected);

    for (auto other : { IdsList::Executors,
                        IdsList::Authors,
                        IdsList::CastingMaterials,
                        IdsList::ModelMaterials,
                        IdsList::Machines,
                        IdsList::Notes }) {
        if (other != list) {
            QVERIFY(rl.getIds(other).isEmpty());
        }
    }
}

void TestRecordLink::removeIdsSubtracts() {
    RecordLink rl = makeSample();
    rl.removeIds(IdsList::Executors, { 2, 3, 100 });
    QCOMPARE(rl.getIds(IdsList::Executors), (QSet<qint32> { 1 }));
}

void TestRecordLink::replaceIdsReplaces() {
    RecordLink rl = makeSample();
    rl.replaceIds(IdsList::Notes, { 100, 200 });
    QCOMPARE(rl.getIds(IdsList::Notes), (QSet<qint32> { 100, 200 }));
}

void TestRecordLink::getIdsReturnsReferenceToInternalSet() {
    RecordLink rl = makeSample();
    const QSet<qint32>& ref = rl.getIds(IdsList::Machines);

    const QSet<qint32>& ref2 = rl.getIds(IdsList::Machines);
    QCOMPARE(&ref, &ref2);
}

// ---------- hash ----------

void TestRecordLink::hashEqualForEqualObjects() {
    RecordLink a = makeSample();
    RecordLink b = makeSample();
    QCOMPARE(a.hash(), b.hash());
    QCOMPARE(qHash(a), qHash(b));
}

void TestRecordLink::hashDiffersForDifferentObjects() {
    RecordLink a = makeSample();
    RecordLink b = makeSample();
    b.setIdDate(a.getIdDate() + 1);

    QVERIFY(a.hash() != b.hash());
}

// ---------- serialize / deserialize ----------

void TestRecordLink::serializeDeserializeRoundTrip() {
    RecordLink original = makeSample();

    QByteArray bytes = serializeToBytes(original);
    QVERIFY(!bytes.isEmpty());

    RecordLink restored = deserializeFromBytes(bytes);
    QCOMPARE(restored, original);
}

void TestRecordLink::serializeDeserializeEmptyObject() {
    RecordLink original;

    QByteArray bytes = serializeToBytes(original);
    RecordLink restored = deserializeFromBytes(bytes);

    QCOMPARE(restored, original);
}

void TestRecordLink::deserializeWrongVersionThrows() {
    QByteArray bytes;
    {
        QDataStream out(&bytes, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Version::Qt_6_11);
        out << static_cast<int>(QDataStream::Version::Qt_6_11);
    }

    QDataStream in(bytes);
    in.setVersion(QDataStream::Version::Qt_6_0);

    RecordLink rl;
    QVERIFY_THROWS_EXCEPTION(RuntimeError, rl.deserialize(in));
}

void TestRecordLink::deserializeNegativeSetSizeThrows() {
    QByteArray bytes;
    {
        QDataStream out(&bytes, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Version::Qt_6_11);
        out << static_cast<int>(QDataStream::Version::Qt_6_11);
        out << qint32(1) << qint32(2) << qint32(3);
        out << qint32(-1);
    }

    QDataStream in(bytes);
    in.setVersion(QDataStream::Version::Qt_6_11);

    RecordLink rl;
    QVERIFY_THROWS_EXCEPTION(RuntimeError, rl.deserialize(in));
}

void TestRecordLink::deserializeTruncatedStreamThrows() {
    RecordLink original = makeSample();
    QByteArray bytes = serializeToBytes(original);

    bytes.chop(bytes.size() / 2);

    QDataStream in(bytes);
    in.setVersion(QDataStream::Version::Qt_6_11);

    RecordLink rl;
    QVERIFY_THROWS_EXCEPTION(RuntimeError, rl.deserialize(in));
}

QTEST_APPLESS_MAIN(TestRecordLink)
#include "test_RecordLink.moc"
