#include <QByteArray>
#include <QDataStream>
#include <QtTest>

#include "CustomList.h"
#include "Drawing.h"

class TestCustomList : public QObject {
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    // ---------- Constructors / Assignment ----------
    void defaultConstructor();
    void copyConstructor();
    void moveConstructor();
    void copyAssignment();
    void moveAssignment();

    // ---------- Comparison operators ----------
    void equalityOperator();
    void inequalityOperator();

    // ---------- insert ----------
    void insertReturnsIncrementingIds();
    void insertReusesFreedIdsAfterCommit();
    void insertManyItemsAfterCommit();
    void insertExistingReturnsSameId();
    void insertExistingDoesNotDuplicateItem();
    void insertExistingAfterCommitReturnsSameId();
    void insertDuplicateInDraftReturnsSameId();
    void insertDuplicateManyTimesKeepsSingleEntry();
    void insertDuplicateOfRemovedItemCreatesNewId();

    // ---------- remove ----------
    void removeExistingItemAfterCommit();
    void removeNonExistingItemAfterCommit();
    void removeThenReinsertReusesIdAfterCommit();

    // ---------- getId / getValue / getAllValues ----------
    void getIdExistingAfterCommit();
    void getIdNonExistingAfterCommit();
    void getValueExistingAfterCommit();
    void getValueNonExistingAfterCommit();
    void getAllValuesEmptyAfterCommit();
    void getAllValuesMultipleAfterCommit();

    // ---------- Modes ----------
    void insertSwitchesToDraftMode();
    void removeSwitchesToDraftMode();
    void clearSwitchesToDraftMode();
    void readersIgnoreDraftMode();
    void readersSeeCommittedData();

    // ---------- size / sizeNotCommitted ----------
    void sizeReflectsMainOnly();
    void sizeNotCommittedReflectsDraftOnly();
    void sizeNotCommittedIsZeroInMainMode();

    // ---------- clear ----------
    void clearInDraftModeAffectsOnlyDraft();
    void clearAfterCommitEmptiesMain();

    // ---------- reset ----------
    void resetRollsBackDraft();
    void resetReturnsToMainMode();
    void resetOnCleanStateIsNoop();
    void resetThenInsertStartsNewDraft();

    // ---------- commit ----------
    void commitAppliesDraftInsert();
    void commitAppliesDraftRemove();
    void commitAppliesDraftClear();
    void commitOnCleanStateIsNoop();
    void commitTwiceIsSafe();
    void commitThenInsertStartsNewDraft();

    // ---------- Combined scenarios ----------
    void insertResetCommitKeepsMainIntact();
    void multipleDraftCycles();
    void draftAccumulatesMultipleOperations();

    // ---------- serialize / deserialize ----------
    void serializeDeserializeRoundTrip();
    void serializeDeserializeEmpty();
    void serializeDeserializePreservesFreeIds();
    void serializeIgnoresDraft();

    // ---------- Different types ----------
    void workWithInt();
    void workWithQString();
    void workWithDrawing();

private:
    QByteArray toBytes(const CustomList<int>& list);
    CustomList<int> fromBytes(const QByteArray& bytes);

    // Helper
    template<typename T>
    CustomList<T> makeCommitted(const QList<T>& values) {
        CustomList<T> list;
        for (const auto& v : values) {
            list.insert(v);
        }
        list.commit();
        return list;
    }
};

// =============== Implementation ===============

void TestCustomList::init() { }
void TestCustomList::cleanup() { }

// ---------- Constructors / Assignment ----------

void TestCustomList::defaultConstructor() {
    CustomList<int> list;
    QCOMPARE(list.size(), 0);
    QCOMPARE(list.sizeNotCommitted(), 0);
    QVERIFY(list.getAllValues().isEmpty());
}

void TestCustomList::copyConstructor() {
    auto a = makeCommitted<int>({ 10, 20, 30 });

    CustomList<int> b(a);
    QCOMPARE(b.size(), a.size());
    QCOMPARE(b, a);

    b.remove(20);
    b.commit();
    QVERIFY(b != a);
    QVERIFY(a.getId(20).has_value());
}

void TestCustomList::moveConstructor() {
    auto a = makeCommitted<int>({ 1, 2 });

    CustomList<int> b(std::move(a));
    QCOMPARE(b.size(), 2);
    QVERIFY(b.getId(1).has_value());
    QVERIFY(b.getId(2).has_value());
}

void TestCustomList::copyAssignment() {
    auto a = makeCommitted<int>({ 5, 6 });

    CustomList<int> b;
    b.insert(100);
    b.commit();

    b = a;
    QCOMPARE(b, a);
    QCOMPARE(b.size(), 2);
    QVERIFY(!b.getId(100).has_value());
}

void TestCustomList::moveAssignment() {
    auto a = makeCommitted<int>({ 7, 8 });

    CustomList<int> b;
    b.insert(999);
    b.commit();

    b = std::move(a);
    QCOMPARE(b.size(), 2);
    QVERIFY(b.getId(7).has_value());
    QVERIFY(b.getId(8).has_value());
}

// ---------- Comparison operators ----------

void TestCustomList::equalityOperator() {
    CustomList<int> a, b;
    QVERIFY(a == b);

    a.insert(1);
    QVERIFY(a == b);

    a.commit();
    QVERIFY(a != b);

    b.insert(1);
    b.commit();
    QVERIFY(a == b);

    a.insert(2);
    a.commit();
    b.insert(2);
    b.commit();
    QVERIFY(a == b);
}

void TestCustomList::inequalityOperator() {
    CustomList<int> a, b;
    a.insert(42);
    a.commit();
    QVERIFY(a != b);

    b.insert(43);
    b.commit();
    QVERIFY(a != b);

    b.remove(43);
    b.commit();
    b.insert(42);
    b.commit();
    QVERIFY(a == b);
}

// ---------- insert ----------

void TestCustomList::insertReturnsIncrementingIds() {
    CustomList<int> list;
    auto id1 = list.insert(100);
    auto id2 = list.insert(200);
    auto id3 = list.insert(300);

    QVERIFY(id1 && id2 && id3);
    QCOMPARE(*id1, 1);
    QCOMPARE(*id2, 2);
    QCOMPARE(*id3, 3);

    list.commit();
    QCOMPARE(list.size(), 3);
}

void TestCustomList::insertReusesFreedIdsAfterCommit() {
    auto list = makeCommitted<int>({ 10, 20, 30 });
    auto id20 = *list.getId(20);

    list.remove(20);
    list.commit();

    auto reused = list.insert(40);
    QVERIFY(reused.has_value());
    QCOMPARE(*reused, id20);
    list.commit();
}

void TestCustomList::insertManyItemsAfterCommit() {
    CustomList<int> list;
    const int N = 500;
    for (int i = 0; i < N; ++i) {
        QVERIFY(list.insert(i).has_value());
    }
    list.commit();

    QCOMPARE(list.size(), static_cast<qsizetype>(N));

    for (int i = 0; i < N; ++i) {
        auto id = list.getId(i);
        QVERIFY(id.has_value());
        QCOMPARE(list.getValue(*id).value(), i);
    }
}

void TestCustomList::insertExistingReturnsSameId() {
    CustomList<int> list;

    auto id1 = list.insert(42);
    QVERIFY(id1.has_value());

    auto id2 = list.insert(42);
    QVERIFY(id2.has_value());

    QCOMPARE(*id2, *id1);

    list.commit();
    QCOMPARE(list.size(), 1);
}

void TestCustomList::insertExistingDoesNotDuplicateItem() {
    CustomList<int> list;
    list.insert(7);
    list.insert(7);
    list.insert(7);
    list.commit();

    auto allValues = list.getAllValues();
    QCOMPARE(list.size(), 1);
    QCOMPARE(allValues.size(), 1);
    QCOMPARE(allValues.at(0), 7);
}

void TestCustomList::insertExistingAfterCommitReturnsSameId() {
    auto list = makeCommitted<int>({ 10, 20, 30 });
    const auto id20 = *list.getId(20);

    auto again = list.insert(20);
    QVERIFY(again.has_value());
    QCOMPARE(*again, id20);

    list.commit();
    QCOMPARE(list.size(), 3);
    QCOMPARE(*list.getId(20), id20);
}

void TestCustomList::insertDuplicateInDraftReturnsSameId() {
    CustomList<int> list;

    auto first = list.insert(99);
    QVERIFY(first.has_value());

    auto second = list.insert(99);
    auto third = list.insert(99);

    QVERIFY(second.has_value());
    QVERIFY(third.has_value());
    QCOMPARE(*second, *first);
    QCOMPARE(*third, *first);

    QCOMPARE(list.sizeNotCommitted(), 1);
    QCOMPARE(list.size(), 0);

    list.commit();
    QCOMPARE(list.size(), 1);
}

void TestCustomList::insertDuplicateManyTimesKeepsSingleEntry() {
    CustomList<int> list;
    std::optional<qint32> firstId;

    for (int i = 0; i < 100; ++i) {
        auto id = list.insert(12345);
        QVERIFY(id.has_value());
        if (!firstId.has_value()) {
            firstId = id;
        } else {
            QCOMPARE(*id, *firstId);
        }
    }

    QCOMPARE(list.sizeNotCommitted(), 1);

    list.commit();
    auto allValues = list.getAllValues();
    QCOMPARE(allValues.size(), 1);
    QCOMPARE(allValues.at(0), 12345);
}

void TestCustomList::insertDuplicateOfRemovedItemCreatesNewId() {
    auto list = makeCommitted<int>({ 5, 10, 15 });
    const auto oldId = *list.getId(10);

    list.remove(10);
    list.commit();

    auto newId = list.insert(10);
    QVERIFY(newId.has_value());
    QVERIFY(*newId == oldId);

    list.commit();
    QCOMPARE(list.size(), 3);
    QCOMPARE(*list.getId(10), *newId);
}

// ---------- remove ----------

void TestCustomList::removeExistingItemAfterCommit() {
    auto list = makeCommitted<int>({ 11, 22 });

    QVERIFY(list.remove(11));
    list.commit();

    QCOMPARE(list.size(), 1);
    QVERIFY(!list.getId(11).has_value());
    QVERIFY(list.getId(22).has_value());
}

void TestCustomList::removeNonExistingItemAfterCommit() {
    auto list = makeCommitted<int>({ 1 });

    QVERIFY(!list.remove(999));
    list.commit();
    QCOMPARE(list.size(), 1);
}

void TestCustomList::removeThenReinsertReusesIdAfterCommit() {
    auto list = makeCommitted<int>({ 5 });
    auto id = *list.getId(5);

    QVERIFY(list.remove(5));
    list.commit();

    auto newId = list.insert(6);
    QVERIFY(newId.has_value());
    QCOMPARE(*newId, id);
    list.commit();
}

// ---------- getId / getValue / getAllValues ----------

void TestCustomList::getIdExistingAfterCommit() {
    auto list = makeCommitted<int>({ 77 });

    auto found = list.getId(77);
    QVERIFY(found.has_value());
    QCOMPARE(*found, 1);
}

void TestCustomList::getIdNonExistingAfterCommit() {
    auto list = makeCommitted<int>({ 1 });
    QVERIFY(!list.getId(999).has_value());
}

void TestCustomList::getValueExistingAfterCommit() {
    auto list = makeCommitted<int>({ 1234 });
    auto id = *list.getId(1234);

    auto value = list.getValue(id);
    QVERIFY(value.has_value());
    QCOMPARE(*value, 1234);
}

void TestCustomList::getValueNonExistingAfterCommit() {
    auto list = makeCommitted<int>({ 1 });
    QVERIFY(!list.getValue(999).has_value());
    QVERIFY(!list.getValue(-1).has_value());
}

void TestCustomList::getAllValuesEmptyAfterCommit() {
    CustomList<int> list;
    QVERIFY(list.getAllValues().isEmpty());
}

void TestCustomList::getAllValuesMultipleAfterCommit() {
    auto list = makeCommitted<int>({ 1, 2, 3 });

    auto values = list.getAllValues();
    QCOMPARE(values.size(), 3);
    std::sort(values.begin(), values.end());
    QCOMPARE(values, QList<int>({ 1, 2, 3 }));
}

// ---------- Режимы ----------

void TestCustomList::insertSwitchesToDraftMode() {
    CustomList<int> list;
    QCOMPARE(list.sizeNotCommitted(), 0);

    list.insert(1);
    QCOMPARE(list.sizeNotCommitted(), 1);
    QCOMPARE(list.size(), 0);
}

void TestCustomList::removeSwitchesToDraftMode() {
    auto list = makeCommitted<int>({ 1, 2, 3 });

    QCOMPARE(list.sizeNotCommitted(), 3);

    list.remove(2);
    QCOMPARE(list.sizeNotCommitted(), 2);
    QCOMPARE(list.size(), 3);
}

void TestCustomList::clearSwitchesToDraftMode() {
    auto list = makeCommitted<int>({ 1, 2, 3 });

    list.clear();
    QCOMPARE(list.sizeNotCommitted(), 0);
    QCOMPARE(list.size(), 3);
}

void TestCustomList::readersIgnoreDraftMode() {
    auto list = makeCommitted<int>({ 10 });

    list.insert(20);
    QCOMPARE(list.sizeNotCommitted(), 2);

    QVERIFY(list.getId(10).has_value());
    QVERIFY(!list.getId(20).has_value());

    auto values = list.getAllValues();
    QCOMPARE(values, QList<int>({ 10 }));

    list.commit();
    QVERIFY(list.getId(20).has_value());
}

void TestCustomList::readersSeeCommittedData() {
    auto list = makeCommitted<int>({ 10 });

    QVERIFY(list.getId(10).has_value());
    QVERIFY(!list.getId(20).has_value());

    auto values = list.getAllValues();
    QCOMPARE(values, QList<int>({ 10 }));
}

// ---------- size / sizeNotCommitted ----------

void TestCustomList::sizeReflectsMainOnly() {
    CustomList<int> list;
    list.insert(1);
    list.insert(2);
    list.insert(3);

    QCOMPARE(list.size(), 0);
    QCOMPARE(list.sizeNotCommitted(), 3);

    list.commit();
    QCOMPARE(list.size(), 3);
    QCOMPARE(list.sizeNotCommitted(), 3);
}

void TestCustomList::sizeNotCommittedReflectsDraftOnly() {
    CustomList<int> list;
    list.insert(1);
    list.insert(2);

    QCOMPARE(list.sizeNotCommitted(), 2);
    QCOMPARE(list.size(), 0);
}

void TestCustomList::sizeNotCommittedIsZeroInMainMode() {
    CustomList<int> list;
    QCOMPARE(list.sizeNotCommitted(), 0);

    auto committed = makeCommitted<int>({ 1, 2, 3 });
    QCOMPARE(committed.sizeNotCommitted(), 3);
    QCOMPARE(committed.size(), 3);
}

// ---------- clear ----------

void TestCustomList::clearInDraftModeAffectsOnlyDraft() {
    auto list = makeCommitted<int>({ 1, 2, 3 });

    list.clear();
    QCOMPARE(list.sizeNotCommitted(), 0);
    QCOMPARE(list.size(), 3);

    QCOMPARE(list.getAllValues().size(), 3);
}

void TestCustomList::clearAfterCommitEmptiesMain() {
    auto list = makeCommitted<int>({ 1, 2, 3 });

    list.clear();
    list.commit();

    QCOMPARE(list.size(), 0);
    QVERIFY(list.getAllValues().isEmpty());
    QVERIFY(!list.getId(1).has_value());
}

// ---------- reset ----------

void TestCustomList::resetRollsBackDraft() {
    auto list = makeCommitted<int>({ 1, 2 });

    list.insert(3);
    list.insert(4);
    QCOMPARE(list.sizeNotCommitted(), 4);

    list.reset();

    QCOMPARE(list.sizeNotCommitted(), list.size());
}

void TestCustomList::resetReturnsToMainMode() {
    auto list = makeCommitted<int>({ 1 });

    list.insert(2);
    QCOMPARE(list.sizeNotCommitted(), 2);

    list.reset();
    QCOMPARE(list.sizeNotCommitted(), 1);

    QVERIFY(list.getId(1).has_value());
    QVERIFY(!list.getId(2).has_value());
}

void TestCustomList::resetOnCleanStateIsNoop() {
    auto list = makeCommitted<int>({ 1, 2 });

    list.insert(2);
    QCOMPARE(list.sizeNotCommitted(), 2);

    list.reset();

    QCOMPARE(list.size(), 2);
    QCOMPARE(list.sizeNotCommitted(), 2);
    QVERIFY(list.getId(1).has_value());
    QVERIFY(list.getId(2).has_value());
}

void TestCustomList::resetThenInsertStartsNewDraft() {
    auto list = makeCommitted<int>({ 1 });

    list.insert(2);
    list.reset();

    list.insert(3);
    QCOMPARE(list.sizeNotCommitted(), 2);
    QVERIFY(!list.getId(3).has_value());

    list.commit();
    QCOMPARE(list.size(), 2);
    QVERIFY(list.getId(1).has_value());
    QVERIFY(list.getId(3).has_value());
    QVERIFY(!list.getId(2).has_value());
}

// ---------- commit ----------

void TestCustomList::commitAppliesDraftInsert() {
    CustomList<int> list;
    list.insert(1);
    list.insert(2);

    QCOMPARE(list.size(), 0);
    QCOMPARE(list.sizeNotCommitted(), 2);

    list.commit();

    QCOMPARE(list.size(), list.sizeNotCommitted());
    QVERIFY(list.getId(1).has_value());
    QVERIFY(list.getId(2).has_value());
}

void TestCustomList::commitAppliesDraftRemove() {
    auto list = makeCommitted<int>({ 10, 20, 30 });

    list.remove(20);
    QCOMPARE(list.sizeNotCommitted(), 2);

    list.commit();

    QCOMPARE(list.size(), 2);
    QVERIFY(list.getId(10).has_value());
    QVERIFY(!list.getId(20).has_value());
    QVERIFY(list.getId(30).has_value());
}

void TestCustomList::commitAppliesDraftClear() {
    auto list = makeCommitted<int>({ 1, 2, 3 });

    list.clear();
    list.commit();

    QCOMPARE(list.size(), 0);
    QVERIFY(list.getAllValues().isEmpty());
}

void TestCustomList::commitOnCleanStateIsNoop() {
    auto list = makeCommitted<int>({ 1, 2 });

    auto before = list.getAllValues();
    const qsizetype s = list.size();

    list.commit();

    QCOMPARE(list.size(), s);
    QCOMPARE(list.getAllValues(), before);
}

void TestCustomList::commitTwiceIsSafe() {
    CustomList<int> list;
    list.insert(42);
    list.commit();
    const qsizetype s1 = list.size();

    list.commit();
    QCOMPARE(list.size(), s1);
    QVERIFY(list.getId(42).has_value());
}

void TestCustomList::commitThenInsertStartsNewDraft() {
    CustomList<int> list;
    list.insert(1);
    list.commit();

    list.insert(2);
    QCOMPARE(list.sizeNotCommitted(), 2);
    QCOMPARE(list.size(), 1);

    list.commit();
    QCOMPARE(list.size(), 2);
}

// ---------- Combined scenarios ----------

void TestCustomList::insertResetCommitKeepsMainIntact() {
    auto list = makeCommitted<int>({ 1, 2 });

    list.insert(3);
    list.insert(4);
    list.reset();
    list.commit();

    QCOMPARE(list.size(), 2);
    QVERIFY(list.getId(1).has_value());
    QVERIFY(list.getId(2).has_value());
    QVERIFY(!list.getId(3).has_value());
    QVERIFY(!list.getId(4).has_value());
}

void TestCustomList::multipleDraftCycles() {
    CustomList<int> list;
    list.insert(1);
    list.commit();

    list.insert(2);
    list.reset();

    list.insert(3);
    list.insert(4);
    list.commit();

    QCOMPARE(list.size(), 3);
    QVERIFY(list.getId(1).has_value());
    QVERIFY(!list.getId(2).has_value());
    QVERIFY(list.getId(3).has_value());
    QVERIFY(list.getId(4).has_value());
}

void TestCustomList::draftAccumulatesMultipleOperations() {
    auto list = makeCommitted<int>({ 1, 2, 3 });

    list.insert(4);
    list.remove(2);
    list.insert(5);

    QCOMPARE(list.sizeNotCommitted(), 4);
    QCOMPARE(list.size(), 3);

    list.commit();
    QCOMPARE(list.size(), 4);
    QVERIFY(list.getId(1).has_value());
    QVERIFY(!list.getId(2).has_value());
    QVERIFY(list.getId(3).has_value());
    QVERIFY(list.getId(4).has_value());
    QVERIFY(list.getId(5).has_value());
}

// ---------- serialize / deserialize ----------

QByteArray TestCustomList::toBytes(const CustomList<int>& list) {
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    list.serialize(out);
    return data;
}

CustomList<int> TestCustomList::fromBytes(const QByteArray& bytes) {
    CustomList<int> list;
    QDataStream in(bytes);
    in.setVersion(QDataStream::Qt_6_0);
    list.deserialize(in);
    return list;
}

void TestCustomList::serializeDeserializeRoundTrip() {
    auto original = makeCommitted<int>({ 10, 20, 30 });
    original.remove(20);
    original.commit();

    auto bytes = toBytes(original);
    auto restored = fromBytes(bytes);

    QCOMPARE(restored, original);
    QCOMPARE(restored.size(), 2);

    auto reused = restored.insert(99);
    QVERIFY(reused.has_value());
    QCOMPARE(*reused, 2);
}

void TestCustomList::serializeDeserializeEmpty() {
    CustomList<int> empty;
    auto bytes = toBytes(empty);
    auto restored = fromBytes(bytes);

    QCOMPARE(restored.size(), 0);
    QCOMPARE(restored, empty);
}

void TestCustomList::serializeDeserializePreservesFreeIds() {
    CustomList<int> original;
    original.insert(1);
    original.insert(2);
    original.insert(3);
    original.commit();

    original.remove(1);
    original.remove(3);
    original.commit();

    auto bytes = toBytes(original);
    auto restored = fromBytes(bytes);

    auto id = restored.insert(100);
    QVERIFY(id.has_value());
    QVERIFY(*id == 1 || *id == 3);
}

void TestCustomList::serializeIgnoresDraft() {
    auto original = makeCommitted<int>({ 1, 2 });

    original.insert(3);
    original.insert(4);

    auto bytes = toBytes(original);
    auto restored = fromBytes(bytes);

    QCOMPARE(restored.size(), 2);
    QVERIFY(restored.getId(1).has_value());
    QVERIFY(restored.getId(2).has_value());
    QVERIFY(!restored.getId(3).has_value());
    QVERIFY(!restored.getId(4).has_value());
}

// ---------- Different types ----------

void TestCustomList::workWithInt() {
    CustomList<int> list;
    QVERIFY(list.insert(-5).has_value());
    QVERIFY(list.insert(0).has_value());
    QVERIFY(list.insert(5).has_value());
    list.commit();

    QCOMPARE(list.size(), 3);
    QVERIFY(list.remove(0));
    list.commit();
    QCOMPARE(list.size(), 2);
}

void TestCustomList::workWithQString() {
    CustomList<QString> list;
    auto id1 = list.insert("hello");
    auto id2 = list.insert("world");
    list.commit();

    QVERIFY(id1 && id2);

    auto v = list.getValue(*id1);
    QVERIFY(v.has_value());
    QCOMPARE(*v, QString("hello"));

    QVERIFY(list.remove("world"));
    list.commit();
    QCOMPARE(list.size(), 1);
}

void TestCustomList::workWithDrawing() {
    CustomList<Drawing> list;
    auto id1 = list.insert(Drawing("ИМН-001", "Корпус"));
    auto id2 = list.insert(Drawing("105-02", "Крышка"));
    list.commit();

    QVERIFY(id1 && id2);

    auto v = list.getValue(*id1);
    QVERIFY(v.has_value());
    QCOMPARE(*v, Drawing("ИМН-001", "Корпус"));

    QVERIFY(list.remove(Drawing("105-02", "Крышка")));
    list.commit();
    QCOMPARE(list.size(), 1);
}

QTEST_MAIN(TestCustomList)
#include "test_CustomList.moc"
