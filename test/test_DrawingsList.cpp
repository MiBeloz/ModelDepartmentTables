#include <QTest>
#include <QObject>

#include "ItemData.h"


DrawingList ld;

class TestDrawingsList final : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(TestDrawingsList)

public:
    TestDrawingsList() = default;

private slots:


    void testCreate() {
        QCOMPARE(ld.size(), 0);
    }

    void testInsert() {
        QCOMPARE(ld.insert(Drawing("ИМН-111", "Колесо рабочее")), 1);
        QCOMPARE(ld.size(), 1);

        QCOMPARE(ld.insert(Drawing("111-11", "Шкиф")), 2);
        QCOMPARE(ld.insert(Drawing("1-234", "Башмак")), 3);
        QCOMPARE(ld.insert(Drawing("ИМЭ-987", "Поршень")), 4);
        QCOMPARE(ld.size(), 4);

        QCOMPARE(ld.insert(Drawing("1-234", "Башмак")), 3);
        QCOMPARE(ld.size(), 4);

        QCOMPARE(ld.insert(Drawing("987-789", "Корпус")), 5);
        QCOMPARE(ld.size(), 5);
    }

    // void testRemove() {
    //     QCOMPARE(ld.remove("05.11.2021"), true);
    //     QCOMPARE(ld.size(), 4);

    //     QCOMPARE(ld.remove("25.09.2008"), false);
    //     QCOMPARE(ld.size(), 4);

    //     QCOMPARE(ld.remove("18.03.2020"), true);
    //     QCOMPARE(ld.size(), 3);

    //     QCOMPARE(ld.remove("55.08.2019"), false);
    // }

    // void testInsertWithQueue() {
    //     QCOMPARE(ld.insert("13.06.2015"), 4);
    //     QCOMPARE(ld.size(), 4);

    //     QCOMPARE(ld.insert("26.03.2018"), 3);
    //     QCOMPARE(ld.size(), 5);

    //     QCOMPARE(ld.insert("09.01.2019"), 6);
    //     QCOMPARE(ld.size(), 6);
    // }

    // void testGetId() {
    //     QCOMPARE(ld.getID("13.06.2015"), 4);
    //     QCOMPARE(ld.getID("15.07.2024"), 2);
    //     QCOMPARE(ld.getID("26.03.2018"), 3);
    //     QCOMPARE(ld.getID("28.08.2019"), 5);
    //     QCOMPARE(ld.getID("09.01.2019"), 6);
    //     QCOMPARE(ld.getID("25.08.2025"), 1);

    //     QCOMPARE(ld.getID("01.05.2017"), 0);
    //     QCOMPARE(ld.getID("55.05.2017"), -1);
    // }

    // void testGetDate() {
    //     QCOMPARE(ld.getDate(4), "13.06.2015");
    //     QCOMPARE(ld.getDate(2), "15.07.2024");
    //     QCOMPARE(ld.getDate(3), "26.03.2018");
    //     QCOMPARE(ld.getDate(5), "28.08.2019");
    //     QCOMPARE(ld.getDate(6), "09.01.2019");
    //     QCOMPARE(ld.getDate(1), "25.08.2025");

    //     QCOMPARE(ld.getDate(7), "");
    // }
};

QTEST_APPLESS_MAIN(TestDrawingsList)
#include "test_DrawingsList.moc"
