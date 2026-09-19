// #include <QObject>
// #include <QTest>

// #include "CustomList.h"

// CustomList<int> custom_list;

// class TestCustomListInt final : public QObject {
//     Q_OBJECT
//     Q_DISABLE_COPY_MOVE(TestCustomListInt)

// public:
//     TestCustomListInt() = default;

// private slots:

//     void testCreate() {
//         QCOMPARE(custom_list.size(), 0);
//     }

//     void testInsert() {
//         QCOMPARE(custom_list.insert(7), 1);
//         QCOMPARE(custom_list.size(), 1);

//         QCOMPARE(custom_list.insert(15), 2);
//         QCOMPARE(custom_list.insert(1), 3);
//         QCOMPARE(custom_list.insert(5), 4);
//         QCOMPARE(custom_list.size(), 4);

//         QCOMPARE(custom_list.insert(1), 3);
//         QCOMPARE(custom_list.size(), 4);

//         QCOMPARE(custom_list.insert(10), 5);
//         QCOMPARE(custom_list.size(), 5);
//     }

//     void testRemove() {
//         QCOMPARE(custom_list.remove(5), true);
//         QCOMPARE(custom_list.size(), 4);

//         QCOMPARE(custom_list.remove(55), false);
//         QCOMPARE(custom_list.size(), 4);

//         QCOMPARE(custom_list.remove(1), true);
//         QCOMPARE(custom_list.size(), 3);
//     }

//     void testInsertWithQueue() {
//         QCOMPARE(custom_list.insert(11), 4);
//         QCOMPARE(custom_list.size(), 4);

//         QCOMPARE(custom_list.insert(3), 3);
//         QCOMPARE(custom_list.size(), 5);

//         QCOMPARE(custom_list.insert(6), 6);
//         QCOMPARE(custom_list.size(), 6);
//     }

//     void testGetId() {
//         QCOMPARE(custom_list.getId(11), 4);
//         QCOMPARE(custom_list.getId(15), 2);
//         QCOMPARE(custom_list.getId(3), 3);
//         QCOMPARE(custom_list.getId(10), 5);
//         QCOMPARE(custom_list.getId(6), 6);
//         QCOMPARE(custom_list.getId(7), 1);

//         QCOMPARE(custom_list.getId(33), std::nullopt);
//     }

//     void testGetValue() {
//         QCOMPARE(custom_list.getValue(4), 11);
//         QCOMPARE(custom_list.getValue(2), 15);
//         QCOMPARE(custom_list.getValue(3), 3);
//         QCOMPARE(custom_list.getValue(5), 10);
//         QCOMPARE(custom_list.getValue(6), 6);
//         QCOMPARE(custom_list.getValue(1), 7);

//         QCOMPARE(custom_list.getValue(7), std::nullopt);
//     }
// };

// QTEST_APPLESS_MAIN(TestCustomListInt)
// #include "test_CustomListInt.moc"



// tst_customlist.h
//#pragma once

#include <QtTest>
#include <QSignalSpy>
#include <QDataStream>
#include <QByteArray>
#include <optional>

#include "CustomList.h"

class TestCustomList : public QObject
{
    Q_OBJECT

private slots:
    // ===== Инициализация/очистка =====
    void init();
    void cleanup();

    // ===== Конструкторы и присваивание =====
    void defaultConstructor();
    void copyConstructor();
    void moveConstructor();
    void copyAssignment();
    void moveAssignment();

    // ===== Операторы сравнения =====
    void equalityOperator();
    void inequalityOperator();

    // ===== insert =====
    void insertReturnsIncrementingIds();
    void insertReusesFreedIds();
    void insertManyItems();

    // ===== remove =====
    void removeExistingItem();
    void removeNonExistingItem();
    void removeThenReinsertReusesId();

    // ===== getId =====
    void getIdExisting();
    void getIdNonExisting();
    void getIdOnEmptyList();

    // ===== getValue =====
    void getValueExisting();
    void getValueNonExisting();

    // ===== getAllValues =====
    void getAllValuesEmpty();
    void getAllValuesMultiple();

    // ===== size / clear =====
    void sizeEmpty();
    void sizeAfterInsertRemove();
    void clearRemovesAll();

    // ===== serialize / deserialize =====
    void serializeDeserializeRoundTrip();
    void serializeDeserializeEmpty();
    void serializeDeserializePreservesFreeIds();

    // ===== Работа с разными типами =====
    void workWithInt();
    void workWithQString();

private:
    QByteArray serializeToList(const CustomList<int>& list);
    CustomList<int> deserializeFromBytes(const QByteArray& bytes);
};

// ---------------- Реализация ----------------

void TestCustomList::init() {}
void TestCustomList::cleanup() {}

// -------- Конструкторы --------

void TestCustomList::defaultConstructor()
{
    CustomList<int> list;
    QCOMPARE(list.size(), 0);
    QVERIFY(list.getAllValues().isEmpty());
}

void TestCustomList::copyConstructor()
{
    CustomList<int> a;
    a.insert(10);
    a.insert(20);
    a.insert(30);

    CustomList<int> b(a);

    QCOMPARE(b.size(), a.size());
    QCOMPARE(b, a);

    // Изменение копии не должно влиять на оригинал
    b.remove(20);
    QVERIFY(b != a);
    QVERIFY(a.getId(20).has_value());
}

void TestCustomList::moveConstructor()
{
    CustomList<int> a;
    a.insert(1);
    a.insert(2);

    CustomList<int> b(std::move(a));

    QCOMPARE(b.size(), 2);
    QVERIFY(b.getId(1).has_value());
    QVERIFY(b.getId(2).has_value());
}

void TestCustomList::copyAssignment()
{
    CustomList<int> a;
    a.insert(5);
    a.insert(6);

    CustomList<int> b;
    b.insert(100);

    b = a;
    QCOMPARE(b, a);
    QCOMPARE(b.size(), 2);
    QVERIFY(!b.getId(100).has_value());
}

void TestCustomList::moveAssignment()
{
    CustomList<int> a;
    a.insert(7);
    a.insert(8);

    CustomList<int> b;
    b.insert(999);
    b = std::move(a);

    QCOMPARE(b.size(), 2);
    QVERIFY(b.getId(7).has_value());
    QVERIFY(b.getId(8).has_value());
}

// -------- Операторы сравнения --------

void TestCustomList::equalityOperator()
{
    CustomList<int> a, b;
    QVERIFY(a == b);

    a.insert(1);
    QVERIFY(a != b);

    b.insert(1);
    QVERIFY(a == b);

    a.insert(2);
    b.insert(2);
    QVERIFY(a == b);
}

void TestCustomList::inequalityOperator()
{
    CustomList<int> a, b;
    a.insert(42);
    QVERIFY(a != b);

    b.insert(43);
    QVERIFY(a != b);

    b.remove(43);
    b.insert(42);
    QVERIFY(a == b);
}

// -------- insert --------

void TestCustomList::insertReturnsIncrementingIds()
{
    CustomList<int> list;

    auto id1 = list.insert(100);
    auto id2 = list.insert(200);
    auto id3 = list.insert(300);

    QVERIFY(id1.has_value());
    QVERIFY(id2.has_value());
    QVERIFY(id3.has_value());

    QCOMPARE(*id1, 0);
    QCOMPARE(*id2, 1);
    QCOMPARE(*id3, 2);
}

void TestCustomList::insertReusesFreedIds()
{
    CustomList<int> list;
    auto id0 = *list.insert(10);
    auto id1 = *list.insert(20);
    auto id2 = *list.insert(30);

    list.remove(20); // освобождаем id1

    auto reused = list.insert(40);
    QVERIFY(reused.has_value());
    QCOMPARE(*reused, id1);

    Q_UNUSED(id0);
    Q_UNUSED(id2);
}

void TestCustomList::insertManyItems()
{
    CustomList<int> list;
    const int N = 1000;

    for (int i = 0; i < N; ++i)
        QVERIFY(list.insert(i).has_value());

    QCOMPARE(list.size(), static_cast<qsizetype>(N));

    for (int i = 0; i < N; ++i) {
        auto id = list.getId(i);
        QVERIFY(id.has_value());
        QCOMPARE(list.getValue(*id).value(), i);
    }
}

// -------- remove --------

void TestCustomList::removeExistingItem()
{
    CustomList<int> list;
    list.insert(11);
    list.insert(22);

    QVERIFY(list.remove(11));
    QCOMPARE(list.size(), 1);
    QVERIFY(!list.getId(11).has_value());
    QVERIFY(list.getId(22).has_value());
}

void TestCustomList::removeNonExistingItem()
{
    CustomList<int> list;
    list.insert(1);

    QVERIFY(!list.remove(999));
    QCOMPARE(list.size(), 1);
}

void TestCustomList::removeThenReinsertReusesId()
{
    CustomList<int> list;
    auto id = *list.insert(5);

    QVERIFY(list.remove(5));

    auto newId = list.insert(6);
    QVERIFY(newId.has_value());
    QCOMPARE(*newId, id);
}

// -------- getId --------

void TestCustomList::getIdExisting()
{
    CustomList<int> list;
    auto id = *list.insert(77);

    auto found = list.getId(77);
    QVERIFY(found.has_value());
    QCOMPARE(*found, id);
}

void TestCustomList::getIdNonExisting()
{
    CustomList<int> list;
    list.insert(1);

    QVERIFY(!list.getId(999).has_value());
}

void TestCustomList::getIdOnEmptyList()
{
    CustomList<int> list;
    QVERIFY(!list.getId(0).has_value());
}

// -------- getValue --------

void TestCustomList::getValueExisting()
{
    CustomList<int> list;
    auto id = *list.insert(1234);

    auto value = list.getValue(id);
    QVERIFY(value.has_value());
    QCOMPARE(*value, 1234);
}

void TestCustomList::getValueNonExisting()
{
    CustomList<int> list;
    list.insert(1);

    QVERIFY(!list.getValue(999).has_value());
    QVERIFY(!list.getValue(-1).has_value());
}

// -------- getAllValues --------

void TestCustomList::getAllValuesEmpty()
{
    CustomList<int> list;
    QVERIFY(list.getAllValues().isEmpty());
}

void TestCustomList::getAllValuesMultiple()
{
    CustomList<int> list;
    list.insert(1);
    list.insert(2);
    list.insert(3);

    auto values = list.getAllValues();
    QCOMPARE(values.size(), 3);

    // Порядок не гарантирован, проверяем состав
    std::sort(values.begin(), values.end());
    QCOMPARE(values, QList<int>({1, 2, 3}));
}

// -------- size / clear --------

void TestCustomList::sizeEmpty()
{
    CustomList<int> list;
    QCOMPARE(list.size(), 0);
}

void TestCustomList::sizeAfterInsertRemove()
{
    CustomList<int> list;
    list.insert(1);
    list.insert(2);
    list.insert(3);
    QCOMPARE(list.size(), 3);

    list.remove(2);
    QCOMPARE(list.size(), 2);

    list.remove(1);
    list.remove(3);
    QCOMPARE(list.size(), 0);
}

void TestCustomList::clearRemovesAll()
{
    CustomList<int> list;
    list.insert(1);
    list.insert(2);
    list.insert(3);

    list.clear();
    QCOMPARE(list.size(), 0);
    QVERIFY(!list.getId(1).has_value());
    QVERIFY(list.getAllValues().isEmpty());
}

// -------- serialize/deserialize --------

QByteArray TestCustomList::serializeToList(const CustomList<int>& list)
{
    QByteArray data;
    QDataStream out(&data, QIODevice::WriteOnly);
    out.setVersion(QDataStream::Qt_6_0);
    list.serialize(out);
    return data;
}

CustomList<int> TestCustomList::deserializeFromBytes(const QByteArray& bytes)
{
    CustomList<int> list;
    QDataStream in(bytes);
    in.setVersion(QDataStream::Qt_6_0);
    list.deserialize(in);
    return list;
}

void TestCustomList::serializeDeserializeRoundTrip()
{
    CustomList<int> original;
    original.insert(10);
    original.insert(20);
    original.insert(30);
    original.remove(20);

    auto bytes = serializeToList(original);
    auto restored = deserializeFromBytes(bytes);

    QCOMPARE(restored, original);
    QCOMPARE(restored.size(), original.size());

    // Свободный id должен переиспользоваться
    auto reused = restored.insert(99);
    QVERIFY(reused.has_value());
    QCOMPARE(*reused, 1); // id=1 был освобождён
}

void TestCustomList::serializeDeserializeEmpty()
{
    CustomList<int> empty;
    auto bytes = serializeToList(empty);
    auto restored = deserializeFromBytes(bytes);

    QCOMPARE(restored.size(), 0);
    QCOMPARE(restored, empty);
}

void TestCustomList::serializeDeserializePreservesFreeIds()
{
    CustomList<int> original;
    original.insert(1); // id 0
    original.insert(2); // id 1
    original.insert(3); // id 2
    original.remove(1); // освобождаем 0
    original.remove(3); // освобождаем 2

    auto bytes = serializeToList(original);
    auto restored = deserializeFromBytes(bytes);

    // После восстановления новый insert должен взять последний свободный id
    auto id = restored.insert(100);
    QVERIFY(id.has_value());
    QVERIFY(*id == 0 || *id == 2);
}

// -------- Разные типы --------

void TestCustomList::workWithInt()
{
    CustomList<int> list;
    QVERIFY(list.insert(-5).has_value());
    QVERIFY(list.insert(0).has_value());
    QVERIFY(list.insert(5).has_value());

    QCOMPARE(list.size(), 3);
    QVERIFY(list.remove(0));
    QCOMPARE(list.size(), 2);
}

void TestCustomList::workWithQString()
{
    CustomList<QString> list;
    auto id1 = list.insert("hello");
    auto id2 = list.insert("world");

    QVERIFY(id1.has_value());
    QVERIFY(id2.has_value());

    auto v = list.getValue(*id1);
    QVERIFY(v.has_value());
    QCOMPARE(*v, QString("hello"));

    QVERIFY(list.remove("world"));
    QCOMPARE(list.size(), 1);
}

QTEST_APPLESS_MAIN(TestCustomList)
#include "test_CustomListInt.moc"
