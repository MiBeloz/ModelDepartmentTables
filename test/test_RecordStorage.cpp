#include <QObject>
#include <QTest>

#include "RecordStorage.h"

RecordStorage storage;

auto rec1 = Record("25.08.2026", Drawing("ИМН-111", "Колесо рабочее"), 2, { "Иван", "Александр" });
auto rec2e = Record("35.08.2026", Drawing("111-11", "Шкиф"), 1);
auto rec3e = Record("20.08.2026", Drawing("ИМЭ-987", "Поршень"), 0, { "Петр", "Станислав" });
auto rec4 = Record("15.08.2026",
                   Drawing("987-789", "Корпус"),
                   3,
                   { "Иван", "Александр" },
                   { "Елена" },
                   { "СЧ20" },
                   { "Пластик красный" },
                   { "Скорпион" },
                   { "Доработка" });
auto rec5 = Record("15.08.2026",
                   Drawing("СЗ-0101", "Тарелка"),
                   4,
                   { "Иван" },
                   { "Елена" },
                   { "СЧ20" },
                   { "Фанера сушеная" },
                   { "Китаец" },
                   { "Только модель" });
auto rec6 = Record("29.08.2026",
                   Drawing("ИМЭ-123", "Блок"),
                   2,
                   { "Иван", "Александр" },
                   { "Дмитрий" },
                   { "СЧ25" },
                   { "Пластик красный" },
                   { "Китаец" });

class TestRecordStorage final : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(TestRecordStorage)

public:
    TestRecordStorage() = default;

private slots:
    void testAdd() {
        QCOMPARE(storage.add(rec1), true);
        QCOMPARE(storage.add(rec2e), false);
        QCOMPARE(storage.add(rec3e), false);
        QCOMPARE(storage.add(rec4), true);
        QCOMPARE(storage.add(rec5), true);
        QCOMPARE(storage.add(rec6), true);
        QCOMPARE(storage.add(rec1), true);
        QCOMPARE(storage.count(), 4);

        qDebug() << "----------   ADDED  ----------";
        storage.print();
    }

    // void testRemove() {
    //     QCOMPARE(storage.dates().remove("05.11.2021"), true);
    //     QCOMPARE(storage.dates().count(), 4);

    //     QCOMPARE(storage.dates().remove("25.09.2008"), false);
    //     QCOMPARE(storage.dates().count(), 4);

    //     QCOMPARE(storage.dates().remove("18.03.2020"), true);
    //     QCOMPARE(storage.dates().count(), 3);

    //     QCOMPARE(storage.dates().remove("55.08.2019"), false);

    //     QCOMPARE(storage.drawings().remove(d_IME_987), true);
    //     QCOMPARE(storage.drawings().remove(d_987_789), true);
    //     QCOMPARE(storage.drawings().count(), 4);
    // }

    // void testaddWithQueue() {
    //     QCOMPARE(storage.dates().add("13.06.2015"), 4);
    //     QCOMPARE(storage.dates().count(), 4);

    //     QCOMPARE(storage.dates().add("26.03.2018"), 3);
    //     QCOMPARE(storage.dates().count(), 5);

    //     QCOMPARE(storage.dates().add("09.01.2019"), 6);
    //     QCOMPARE(storage.dates().count(), 6);
    // }

    // void testGetId() {
    //     QCOMPARE(storage.dates().findId("13.06.2015"), 4);
    //     QCOMPARE(storage.dates().findId("15.07.2024"), 2);
    //     QCOMPARE(storage.dates().findId("26.03.2018"), 3);
    //     QCOMPARE(storage.dates().findId("28.08.2019"), 5);
    //     QCOMPARE(storage.dates().findId("09.01.2019"), 6);
    //     QCOMPARE(storage.dates().findId("25.08.2025"), 1);

    //     QCOMPARE(storage.dates().findId("01.05.2017"), std::nullopt);
    //     QCOMPARE(storage.dates().findId("55.05.2017"), std::nullopt);
    // }

    // void testGetDate() {
    //     QCOMPARE(storage.dates().findStrValue(4), "13.06.2015");
    //     QCOMPARE(storage.dates().findStrValue(2), "15.07.2024");
    //     QCOMPARE(storage.dates().findStrValue(3), "26.03.2018");
    //     QCOMPARE(storage.dates().findStrValue(5), "28.08.2019");
    //     QCOMPARE(storage.dates().findStrValue(6), "09.01.2019");
    //     QCOMPARE(storage.dates().findStrValue(1), "25.08.2025");

    //     QCOMPARE(storage.dates().findStrValue(7), std::nullopt);

    //     qDebug() << "----------   SAVE   ----------";
    //     saver.save(storage);
    //     qDebug() << "Dates:";
    //     storage.dates().printState();
    //     qDebug() << "Drawings:";
    //     storage.drawings().printState();

    //     qDebug() << "----------   CLEAR   ---------";
    //     storage.dates().clear();
    //     storage.drawings().clear();
    //     qDebug() << "Dates:";
    //     storage.dates().printState();
    //     qDebug() << "Drawings:";
    //     storage.drawings().printState();

    //     qDebug() << "----------   LOAD   ----------";
    //     saver.load(storage);
    //     qDebug() << "Dates:";
    //     storage.dates().printState();
    //     qDebug() << "Drawings:";
    //     storage.drawings().printState();
    // }

    // void clear() {
    //     QCOMPARE(storage.dates().count(), 6);
    //     storage.dates().clear();
    //     QCOMPARE(storage.dates().count(), 0);
    // }
};

QTEST_APPLESS_MAIN(TestRecordStorage)
#include "test_RecordStorage.moc"
