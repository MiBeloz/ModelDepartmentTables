#include <QTest>
#include <QObject>

#include "ItemData.h"


CustomList<int> custom_list;

class TestCustomListInt final : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(TestCustomListInt)

public:
    TestCustomListInt() = default;

private slots:


    void testCreate() {
        QCOMPARE(custom_list.size(), 0);
    }

    void testInsert() {
        QCOMPARE(custom_list.insert(7), 1);
        QCOMPARE(custom_list.size(), 1);

        QCOMPARE(custom_list.insert(15), 2);
        QCOMPARE(custom_list.insert(1), 3);
        QCOMPARE(custom_list.insert(5), 4);
        QCOMPARE(custom_list.size(), 4);

        QCOMPARE(custom_list.insert(1), 3);
        QCOMPARE(custom_list.size(), 4);

        QCOMPARE(custom_list.insert(10), 5);
        QCOMPARE(custom_list.size(), 5);
    }

    void testRemove() {
        QCOMPARE(custom_list.remove(5), true);
        QCOMPARE(custom_list.size(), 4);

        QCOMPARE(custom_list.remove(55), false);
        QCOMPARE(custom_list.size(), 4);

        QCOMPARE(custom_list.remove(1), true);
        QCOMPARE(custom_list.size(), 3);
    }

    void testInsertWithQueue() {
        QCOMPARE(custom_list.insert(11), 4);
        QCOMPARE(custom_list.size(), 4);

        QCOMPARE(custom_list.insert(3), 3);
        QCOMPARE(custom_list.size(), 5);

        QCOMPARE(custom_list.insert(6), 6);
        QCOMPARE(custom_list.size(), 6);
    }

    void testGetId() {
        QCOMPARE(custom_list.getID(11), 4);
        QCOMPARE(custom_list.getID(15), 2);
        QCOMPARE(custom_list.getID(3), 3);
        QCOMPARE(custom_list.getID(10), 5);
        QCOMPARE(custom_list.getID(6), 6);
        QCOMPARE(custom_list.getID(7), 1);

        QCOMPARE(custom_list.getID(33), std::nullopt);
    }

    void testGetValue() {
        QCOMPARE(custom_list.getValue(4), 11);
        QCOMPARE(custom_list.getValue(2), 15);
        QCOMPARE(custom_list.getValue(3), 3);
        QCOMPARE(custom_list.getValue(5), 10);
        QCOMPARE(custom_list.getValue(6), 6);
        QCOMPARE(custom_list.getValue(1), 7);

        QCOMPARE(custom_list.getValue(7), std::nullopt);
    }
};

QTEST_APPLESS_MAIN(TestCustomListInt)
#include "test_CustomListInt.moc"
