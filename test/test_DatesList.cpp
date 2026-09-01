#include <QTest>
#include <QObject>

#include "ItemData.h"


DatesList dates_list;

class TestDatesList final : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(TestDatesList)

public:
    TestDatesList() = default;

private slots:


    void testCreate() {
        QCOMPARE(dates_list.size(), 0);
    }

    void testInsert() {
        QCOMPARE(dates_list.insert("25.08.2025"), 1);
        QCOMPARE(dates_list.size(), 1);

        QCOMPARE(dates_list.insert("15.07.2024"), 2);
        QCOMPARE(dates_list.insert("18.03.2020"), 3);
        QCOMPARE(dates_list.insert("05.11.2021"), 4);
        QCOMPARE(dates_list.size(), 4);

        QCOMPARE(dates_list.insert("18.03.2020"), 3);
        QCOMPARE(dates_list.size(), 4);

        QCOMPARE(dates_list.insert("28.08.2019"), 5);
        QCOMPARE(dates_list.size(), 5);

        QCOMPARE(dates_list.insert("55.08.2019"), -1);
    }

    void testRemove() {
        QCOMPARE(dates_list.remove("05.11.2021"), true);
        QCOMPARE(dates_list.size(), 4);

        QCOMPARE(dates_list.remove("25.09.2008"), false);
        QCOMPARE(dates_list.size(), 4);

        QCOMPARE(dates_list.remove("18.03.2020"), true);
        QCOMPARE(dates_list.size(), 3);

        QCOMPARE(dates_list.remove("55.08.2019"), false);
    }

    void testInsertWithQueue() {
        QCOMPARE(dates_list.insert("13.06.2015"), 4);
        QCOMPARE(dates_list.size(), 4);

        QCOMPARE(dates_list.insert("26.03.2018"), 3);
        QCOMPARE(dates_list.size(), 5);

        QCOMPARE(dates_list.insert("09.01.2019"), 6);
        QCOMPARE(dates_list.size(), 6);
    }

    void testGetId() {
        QCOMPARE(dates_list.getID("13.06.2015"), 4);
        QCOMPARE(dates_list.getID("15.07.2024"), 2);
        QCOMPARE(dates_list.getID("26.03.2018"), 3);
        QCOMPARE(dates_list.getID("28.08.2019"), 5);
        QCOMPARE(dates_list.getID("09.01.2019"), 6);
        QCOMPARE(dates_list.getID("25.08.2025"), 1);

        QCOMPARE(dates_list.getID("01.05.2017"), 0);
        QCOMPARE(dates_list.getID("55.05.2017"), -1);
    }

    void testGetDate() {
        QCOMPARE(dates_list.getDate(4), "13.06.2015");
        QCOMPARE(dates_list.getDate(2), "15.07.2024");
        QCOMPARE(dates_list.getDate(3), "26.03.2018");
        QCOMPARE(dates_list.getDate(5), "28.08.2019");
        QCOMPARE(dates_list.getDate(6), "09.01.2019");
        QCOMPARE(dates_list.getDate(1), "25.08.2025");

        QCOMPARE(dates_list.getDate(7), "");
    }
};

QTEST_APPLESS_MAIN(TestDatesList)
#include "test_DatesList.moc"
