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

    void testRemove() {
        QCOMPARE(ld.remove(Drawing("ИМЭ-987", "Поршень")), true);
        QCOMPARE(ld.size(), 4);

        QCOMPARE(ld.remove(Drawing("ИМЭ-987", "Поршень")), false);
        QCOMPARE(ld.size(), 4);

        QCOMPARE(ld.remove(Drawing("1-234", "Башмак")), true);
        QCOMPARE(ld.size(), 3);
    }

    void testInsertWithQueue() {
        QCOMPARE(ld.insert(Drawing("СЗ-0101", "Тарелка")), 4);
        QCOMPARE(ld.size(), 4);

        QCOMPARE(ld.insert(Drawing("ИМЭ-123", "Блок")), 3);
        QCOMPARE(ld.size(), 5);

        QCOMPARE(ld.insert(Drawing("1-9876", "Фланец")), 6);
        QCOMPARE(ld.size(), 6);
    }

    void testGetId() {
        QCOMPARE(ld.getID(Drawing("СЗ-0101", "Тарелка")), 4);
        QCOMPARE(ld.getID(Drawing("111-11", "Шкиф")), 2);
        QCOMPARE(ld.getID(Drawing("ИМЭ-123", "Блок")), 3);
        QCOMPARE(ld.getID(Drawing("987-789", "Корпус")), 5);
        QCOMPARE(ld.getID(Drawing("1-9876", "Фланец")), 6);
        QCOMPARE(ld.getID(Drawing("ИМН-111", "Колесо рабочее")), 1);

        QCOMPARE(ld.getID(Drawing("111-222", "Звездочка")), 0);
    }

    void testGetDate() {
        QCOMPARE(ld.getDrawing(4), Drawing("СЗ-0101", "Тарелка"));
        QCOMPARE(ld.getDrawing(2), Drawing("111-11", "Шкиф"));
        QCOMPARE(ld.getDrawing(3), Drawing("ИМЭ-123", "Блок"));
        QCOMPARE(ld.getDrawing(5), Drawing("987-789", "Корпус"));
        QCOMPARE(ld.getDrawing(6), Drawing("1-9876", "Фланец"));
        QCOMPARE(ld.getDrawing(1), Drawing("ИМН-111", "Колесо рабочее"));

        QCOMPARE(ld.getDrawing(7), Drawing());
    }
};

QTEST_APPLESS_MAIN(TestDrawingsList)
#include "test_DrawingsList.moc"
