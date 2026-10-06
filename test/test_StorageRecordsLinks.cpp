#include <QByteArray>
#include <QDataStream>
#include <QtTest>

#include "Exceptions.h"
#include "RecordLink.h"
#include "StorageRecordsLinks.h"

class TestStorageRecordsLinks : public QObject {
    Q_OBJECT

private slots:
    // ---------- helpers ----------
    RecordLink makeLink(int v);

    // ---------- Constructors / Assignment ----------
    void defaultConstructor();
    void copyConstructor();
    void moveConstructor();
    void copyAssignment();
    void moveAssignment();
    void selfAssignment();

    // ---------- Comparison operators ----------
    void equality();
    void inequality();
    void equalityWithDifferentCommitState();

    // ---------- swap ----------
    void swapMembers();
    void selfSwap();

    // ---------- add / remove / get ----------
    void addInsert();
    void addDuplicate();
    void removeExisting();
    void removeNonExisting();
    void getReturnsTmp();

    // ---------- reset / commit ----------
    void commitCopiesTmpToMain();
    void resetRestoresFromMain();
    void commitSetsFlagTrue();
    void addRemoveSetsFlagFalse();

    // ---------- count ----------
    void countAndCountCommitted();

    // ---------- clear ----------
    void clearEmptiesTmpAndSetsFlagFalse();

    // ---------- serialization ----------
    void serializeDeserializeRoundTrip();
    void deserializeWrongVersionThrows();
    void deserializeCorruptDataThrows();
    void deserializeNegativeSizeThrows();
    void serializeThrowsOnBadStream();
};

// ---------- helper ----------
RecordLink TestStorageRecordsLinks::makeLink(int v) {
    return RecordLink(v,
                      v + 1,
                      v + 2,
                      { v },
                      { v, v + 1 },
                      { v + 1, v },
                      { v, v, v },
                      { v + 1 },
                      { v + 1, v + 1 });
}

// ---------- Constructors / Assignment ----------
void TestStorageRecordsLinks::defaultConstructor() {
    StorageRecordsLinks s;
    QCOMPARE(s.count(), qsizetype(0));
    QCOMPARE(s.countCommitted(), qsizetype(0));
    QVERIFY(s.get().isEmpty());
}

void TestStorageRecordsLinks::copyConstructor() {
    StorageRecordsLinks a;
    a.add(makeLink(1));
    a.add(makeLink(2));
    a.commit();
    a.add(makeLink(3));

    StorageRecordsLinks b(a);
    QCOMPARE(b.count(), a.count());
    QCOMPARE(b.countCommitted(), a.countCommitted());
    QCOMPARE(b.get(), a.get());
    QVERIFY(b == a);

    b.add(makeLink(4));
    QVERIFY(b != a);
}

void TestStorageRecordsLinks::moveConstructor() {
    StorageRecordsLinks a;
    a.add(makeLink(1));
    a.add(makeLink(2));
    a.commit();

    const auto expectedTmp = a.get();
    const auto expectedCommitted = a.countCommitted();

    StorageRecordsLinks b(std::move(a));

    QCOMPARE(b.get(), expectedTmp);
    QCOMPARE(b.countCommitted(), expectedCommitted);

    QCOMPARE(a.count(), qsizetype(0));
    QCOMPARE(a.countCommitted(), qsizetype(0));
}

void TestStorageRecordsLinks::copyAssignment() {
    StorageRecordsLinks a;
    a.add(makeLink(1));
    a.add(makeLink(2));
    a.commit();

    StorageRecordsLinks b;
    b.add(makeLink(99));
    b.commit();

    b = a;

    QVERIFY(b == a);
    QCOMPARE(b.get(), a.get());
    QCOMPARE(b.count(), a.count());
    QCOMPARE(b.countCommitted(), a.countCommitted());

    b.add(makeLink(3));
    QVERIFY(b != a);
}

void TestStorageRecordsLinks::moveAssignment() {
    StorageRecordsLinks a;
    a.add(makeLink(1));
    a.add(makeLink(2));
    a.commit();

    const auto expectedTmp = a.get();
    const auto expectedCommitted = a.countCommitted();

    StorageRecordsLinks b;
    b.add(makeLink(99));
    b.commit();

    b = std::move(a);

    QCOMPARE(b.get(), expectedTmp);
    QCOMPARE(b.countCommitted(), expectedCommitted);

    QCOMPARE(a.count(), qsizetype(0));
    QCOMPARE(a.countCommitted(), qsizetype(0));
}

void TestStorageRecordsLinks::selfAssignment() {
    StorageRecordsLinks a;
    a.add(makeLink(1));
    a.add(makeLink(2));
    a.commit();
    a.add(makeLink(3));

    const auto snapshotTmp = a.get();
    const auto snapshotCommitted = a.countCommitted();

    // self copy
    StorageRecordsLinks &ref1 = a;
    a = ref1;
    QCOMPARE(a.get(), snapshotTmp);
    QCOMPARE(a.countCommitted(), snapshotCommitted);

    // self move
    StorageRecordsLinks &ref2 = a;
    a = std::move(ref2);
    QCOMPARE(a.get(), snapshotTmp);
    QCOMPARE(a.countCommitted(), snapshotCommitted);
}

// ---------- Comparison operators ----------
void TestStorageRecordsLinks::equality() {
    StorageRecordsLinks a;
    a.add(makeLink(1));
    a.add(makeLink(2));
    a.commit();

    StorageRecordsLinks b;
    b.add(makeLink(2));
    b.add(makeLink(1));
    b.commit();

    QVERIFY(a == b);
    QVERIFY(!(a != b));
}

void TestStorageRecordsLinks::inequality() {
    StorageRecordsLinks a;
    a.add(makeLink(1));
    a.commit();

    StorageRecordsLinks b;
    b.add(makeLink(2));
    b.commit();

    QVERIFY(a != b);
    QVERIFY(!(a == b));
}

void TestStorageRecordsLinks::equalityWithDifferentCommitState() {
    StorageRecordsLinks a;
    a.add(makeLink(1));
    a.commit();
    a.add(makeLink(2));

    StorageRecordsLinks b;
    b.add(makeLink(1));
    b.add(makeLink(2));
    b.commit();

    QVERIFY(a != b);
}

// ---------- swap ----------
void TestStorageRecordsLinks::swapMembers() {
    StorageRecordsLinks a;
    a.add(makeLink(1));
    a.commit();
    a.add(makeLink(2));

    StorageRecordsLinks b;
    b.add(makeLink(10));
    b.commit();

    const auto aTmp = a.get();
    const auto aCommitted = a.countCommitted();
    const auto bTmp = b.get();
    const auto bCommitted = b.countCommitted();

    a.swap(b);

    QCOMPARE(a.get(), bTmp);
    QCOMPARE(a.countCommitted(), bCommitted);
    QCOMPARE(b.get(), aTmp);
    QCOMPARE(b.countCommitted(), aCommitted);
}

void TestStorageRecordsLinks::selfSwap() {
    StorageRecordsLinks a;
    a.add(makeLink(1));
    a.commit();
    a.add(makeLink(2));

    const auto snapshotTmp = a.get();
    const auto snapshotCommitted = a.countCommitted();

    a.swap(a);

    QCOMPARE(a.get(), snapshotTmp);
    QCOMPARE(a.countCommitted(), snapshotCommitted);
}

// ---------- add / remove / get ----------
void TestStorageRecordsLinks::addInsert() {
    StorageRecordsLinks s;
    s.add(makeLink(1));
    s.add(makeLink(2));

    QCOMPARE(s.count(), qsizetype(2));
    QCOMPARE(s.countCommitted(), qsizetype(0));
    QVERIFY(s.get().contains(makeLink(1)));
    QVERIFY(s.get().contains(makeLink(2)));
}

void TestStorageRecordsLinks::addDuplicate() {
    StorageRecordsLinks s;
    s.add(makeLink(1));
    s.add(makeLink(1));
    QCOMPARE(s.count(), qsizetype(1));
}

void TestStorageRecordsLinks::removeExisting() {
    StorageRecordsLinks s;
    s.add(makeLink(1));
    s.add(makeLink(2));
    s.remove(makeLink(1));

    QCOMPARE(s.count(), qsizetype(1));
    QVERIFY(!s.get().contains(makeLink(1)));
    QVERIFY(s.get().contains(makeLink(2)));
}

void TestStorageRecordsLinks::removeNonExisting() {
    StorageRecordsLinks s;
    s.add(makeLink(1));
    s.remove(makeLink(42));
    QCOMPARE(s.count(), qsizetype(1));
}

void TestStorageRecordsLinks::getReturnsTmp() {
    StorageRecordsLinks s;
    s.add(makeLink(1));
    s.commit();
    s.add(makeLink(2));

    const auto result = s.get();
    QVERIFY(result.contains(makeLink(1)));
    QVERIFY(result.contains(makeLink(2)));
    QCOMPARE(result.size(), 2);
}

// ---------- reset / commit ----------
void TestStorageRecordsLinks::commitCopiesTmpToMain() {
    StorageRecordsLinks s;
    s.add(makeLink(1));
    s.commit();

    s.add(makeLink(2));
    QCOMPARE(s.countCommitted(), qsizetype(1));

    s.commit();
    QCOMPARE(s.countCommitted(), qsizetype(2));
    QCOMPARE(s.count(), qsizetype(2));
}

void TestStorageRecordsLinks::resetRestoresFromMain() {
    StorageRecordsLinks s;
    s.add(makeLink(1));
    s.commit();

    s.add(makeLink(2));
    s.add(makeLink(3));
    QCOMPARE(s.count(), qsizetype(3));

    s.reset();
    QCOMPARE(s.count(), qsizetype(1));
    QCOMPARE(s.countCommitted(), qsizetype(1));
    QVERIFY(s.get().contains(makeLink(1)));
    QVERIFY(!s.get().contains(makeLink(2)));
    QVERIFY(!s.get().contains(makeLink(3)));
}

void TestStorageRecordsLinks::commitSetsFlagTrue() {
    StorageRecordsLinks s;
    s.add(makeLink(1));
    s.commit();

    StorageRecordsLinks same;
    same.add(makeLink(1));
    same.commit();

    QVERIFY(s == same);
}

void TestStorageRecordsLinks::addRemoveSetsFlagFalse() {
    StorageRecordsLinks s;
    s.add(makeLink(1));
    s.commit();

    StorageRecordsLinks committed;
    committed.add(makeLink(1));
    committed.commit();

    QVERIFY(s == committed);

    s.add(makeLink(2));
    QVERIFY(s != committed);

    s.remove(makeLink(2));
    QVERIFY(s != committed);
}

// ---------- count ----------
void TestStorageRecordsLinks::countAndCountCommitted() {
    StorageRecordsLinks s;
    QCOMPARE(s.count(), qsizetype(0));
    QCOMPARE(s.countCommitted(), qsizetype(0));

    s.add(makeLink(1));
    s.add(makeLink(2));
    QCOMPARE(s.count(), qsizetype(2));
    QCOMPARE(s.countCommitted(), qsizetype(0));

    s.commit();
    QCOMPARE(s.count(), qsizetype(2));
    QCOMPARE(s.countCommitted(), qsizetype(2));

    s.add(makeLink(3));
    QCOMPARE(s.count(), qsizetype(3));
    QCOMPARE(s.countCommitted(), qsizetype(2));
}

// ---------- clear ----------
void TestStorageRecordsLinks::clearEmptiesTmpAndSetsFlagFalse() {
    StorageRecordsLinks s;
    s.add(makeLink(1));
    s.add(makeLink(2));
    s.commit();

    s.clear();
    QCOMPARE(s.count(), qsizetype(0));
    QCOMPARE(s.countCommitted(), qsizetype(2));
    QVERIFY(s.get().isEmpty());
}

// ---------- Serialization ----------
void TestStorageRecordsLinks::serializeDeserializeRoundTrip() {
    StorageRecordsLinks original;
    original.add(makeLink(1));
    original.add(makeLink(2));
    original.add(makeLink(3));
    original.commit();
    original.add(makeLink(4));

    QByteArray data;
    {
        QDataStream out(&data, QIODevice::WriteOnly);
        original.serialize(out);
        QCOMPARE(out.status(), QDataStream::Ok);
    }

    StorageRecordsLinks restored;
    {
        QDataStream in(&data, QIODevice::ReadOnly);
        restored.deserialize(in);
        QCOMPARE(in.status(), QDataStream::Ok);
    }

    QCOMPARE(restored.count(), original.count());
    QCOMPARE(restored.countCommitted(), original.countCommitted());
    QCOMPARE(restored.get(), original.get());
    QVERIFY(restored == original);
}

void TestStorageRecordsLinks::deserializeWrongVersionThrows() {
    QByteArray data;
    {
        QDataStream out(&data, QIODevice::WriteOnly);
        out << (out.version() + 1);
    }

    StorageRecordsLinks s;
    QDataStream in(&data, QIODevice::ReadOnly);

    QVERIFY_THROWS_EXCEPTION(RuntimeError, s.deserialize(in));
}

void TestStorageRecordsLinks::deserializeCorruptDataThrows() {
    QByteArray data;
    StorageRecordsLinks s;
    QDataStream in(&data, QIODevice::ReadOnly);

    QVERIFY_THROWS_EXCEPTION(RuntimeError, s.deserialize(in));
}

void TestStorageRecordsLinks::deserializeNegativeSizeThrows() {
    QByteArray data;
    {
        QDataStream out(&data, QIODevice::WriteOnly);
        out << out.version();
        out << static_cast<qint32>(-1);
    }

    StorageRecordsLinks s;
    QDataStream in(&data, QIODevice::ReadOnly);

    QVERIFY_THROWS_EXCEPTION(RuntimeError, s.deserialize(in));
}

void TestStorageRecordsLinks::serializeThrowsOnBadStream() {
    QByteArray data;
    QDataStream out(&data, QIODevice::ReadOnly);

    StorageRecordsLinks s;
    s.add(makeLink(1));

    QVERIFY_THROWS_EXCEPTION(RuntimeError, s.deserialize(out));
}

QTEST_APPLESS_MAIN(TestStorageRecordsLinks)
#include "test_StorageRecordsLinks.moc"
