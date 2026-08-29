#include <QTest>
#include <QObject>

#include "ItemData.h"


DrawingsList ld;
auto d_IMN_111 = std::make_shared<Drawing>("ИМН-111", "Колесо рабочее");
auto d_111_11 = std::make_shared<Drawing>("111-11", "Шкиф");
auto d_1_234 = std::make_shared<Drawing>("1-234", "Башмак");
auto d_IME_987 = std::make_shared<Drawing>("ИМЭ-987", "Поршень");
auto d_987_789 = std::make_shared<Drawing>("987-789", "Корпус");
auto d_SZ_0101 = std::make_shared<Drawing>("СЗ-0101", "Тарелка");
auto d_IME_123 = std::make_shared<Drawing>("ИМЭ-123", "Блок");
auto d_1_9876 = std::make_shared<Drawing>("1-9876", "Фланец");

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
        QCOMPARE(ld.insert(d_IMN_111), 1);
        QCOMPARE(ld.size(), 1);

        QCOMPARE(ld.insert(d_111_11->getNumber(), d_111_11->getTitle()), 2);
        QCOMPARE(ld.insert(d_1_234), 3);
        QCOMPARE(ld.insert(d_IME_987), 4);
        QCOMPARE(ld.size(), 4);

        QCOMPARE(ld.insert(d_1_234->getNumber(), d_1_234->getTitle()), 3);
        QCOMPARE(ld.size(), 4);

        QCOMPARE(ld.insert(d_987_789), 5);
        QCOMPARE(ld.size(), 5);
    }

    void testRemove() {
        QCOMPARE(ld.remove(d_IME_987), true);
        QCOMPARE(ld.size(), 4);

        QCOMPARE(ld.remove(d_IME_987), false);
        QCOMPARE(ld.size(), 4);

        QCOMPARE(ld.remove(d_1_234), true);
        QCOMPARE(ld.size(), 3);
    }

    void testInsertWithQueue() {
        QCOMPARE(ld.insert(d_SZ_0101), 4);
        QCOMPARE(ld.size(), 4);

        QCOMPARE(ld.insert(d_IME_123->getNumber(), d_IME_123->getTitle()), 3);
        QCOMPARE(ld.size(), 5);

        QCOMPARE(ld.insert(d_1_9876), 6);
        QCOMPARE(ld.size(), 6);
    }

    void testGetId() {
        QCOMPARE(ld.getID(d_SZ_0101), 4);
        QCOMPARE(ld.getID(d_111_11), 2);
        QCOMPARE(ld.getID(d_IME_123), 3);
        QCOMPARE(ld.getID(d_987_789), 5);
        QCOMPARE(ld.getID(d_1_9876), 6);
        QCOMPARE(ld.getID(d_IMN_111), 1);

        QCOMPARE(ld.getID(d_IME_987), 0);
    }

    void testGetDrawing() {
        QCOMPARE(*ld.getDrawing(4), *d_SZ_0101);
        QCOMPARE(*ld.getDrawing(2), *d_111_11);
        QCOMPARE(*ld.getDrawing(3), *d_IME_123);
        QCOMPARE(*ld.getDrawing(5), *d_987_789);
        QCOMPARE(*ld.getDrawing(6), *d_1_9876);
        QCOMPARE(*ld.getDrawing(1), *d_IMN_111);

        QCOMPARE(ld.getDrawing(7), nullptr);
    }
};

QTEST_APPLESS_MAIN(TestDrawingsList)
#include "test_DrawingsList.moc"
