#include <QObject>
#include <QTest>

#include "DrawingsList.h"

DrawingsList drawings_list;
auto d_IMN_111 = Drawing("ИМН-111", "Колесо рабочее");
auto d_111_11 = Drawing("111-11", "Шкиф");
auto d_1_234 = Drawing("1-234", "Башмак");
auto d_IME_987 = Drawing("ИМЭ-987", "Поршень");
auto d_987_789 = Drawing("987-789", "Корпус");
auto d_SZ_0101 = Drawing("СЗ-0101", "Тарелка");
auto d_IME_123 = Drawing("ИМЭ-123", "Блок");
auto d_1_9876 = Drawing("1-9876", "Фланец");

class TestDrawingsList final : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(TestDrawingsList)

public:
    TestDrawingsList() = default;

private slots:

    void testCreate() {
        QCOMPARE(drawings_list.size(), 0);
    }

    void testInsert() {
        QCOMPARE(drawings_list.insert(d_IMN_111), 1);
        QCOMPARE(drawings_list.size(), 1);

        QCOMPARE(drawings_list.insert(d_111_11), 2);
        QCOMPARE(drawings_list.insert(d_1_234), 3);
        QCOMPARE(drawings_list.insert(d_IME_987), 4);
        QCOMPARE(drawings_list.size(), 4);

        QCOMPARE(drawings_list.insert(d_1_234), 3);
        QCOMPARE(drawings_list.size(), 4);

        QCOMPARE(drawings_list.insert(d_987_789), 5);
        QCOMPARE(drawings_list.size(), 5);
    }

    void testRemove() {
        QCOMPARE(drawings_list.remove(d_IME_987), true);
        QCOMPARE(drawings_list.size(), 4);

        QCOMPARE(drawings_list.remove(d_IME_987), false);
        QCOMPARE(drawings_list.size(), 4);

        QCOMPARE(drawings_list.remove(d_1_234), true);
        QCOMPARE(drawings_list.size(), 3);
    }

    void testInsertWithQueue() {
        QCOMPARE(drawings_list.insert(d_SZ_0101), 4);
        QCOMPARE(drawings_list.size(), 4);

        QCOMPARE(drawings_list.insert(d_IME_123), 3);
        QCOMPARE(drawings_list.size(), 5);

        QCOMPARE(drawings_list.insert(d_1_9876), 6);
        QCOMPARE(drawings_list.size(), 6);
    }

    void testGetId() {
        QCOMPARE(drawings_list.getID(d_SZ_0101), 4);
        QCOMPARE(drawings_list.getID(d_111_11), 2);
        QCOMPARE(drawings_list.getID(d_IME_123), 3);
        QCOMPARE(drawings_list.getID(d_987_789), 5);
        QCOMPARE(drawings_list.getID(d_1_9876), 6);
        QCOMPARE(drawings_list.getID(d_IMN_111), 1);

        QCOMPARE(drawings_list.getID(d_IME_987), std::nullopt);
    }

    void testGetDrawing() {
        QCOMPARE(drawings_list.getValue(4), d_SZ_0101);
        QCOMPARE(drawings_list.getValue(2), d_111_11);
        QCOMPARE(drawings_list.getValue(3), d_IME_123);
        QCOMPARE(drawings_list.getValue(5), d_987_789);
        QCOMPARE(drawings_list.getValue(6), d_1_9876);
        QCOMPARE(drawings_list.getValue(1), d_IMN_111);

        QCOMPARE(drawings_list.getValue(7), std::nullopt);
    }
};

QTEST_APPLESS_MAIN(TestDrawingsList)
#include "test_DrawingsList.moc"
