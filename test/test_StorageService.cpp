#include <QObject>
#include <QTest>

// #include "SaverFileStorage.h"
// #include "StorageService.h"

// StorageService storage;
// FileStorageSaver saver("data.txt", "data.txt.tmp", "data.txt.backup");

// auto d_IMN_111 = Drawing("ИМН-111", "Колесо рабочее");
// auto d_111_11 = Drawing("111-11", "Шкиф");
// auto d_IME_987 = Drawing("ИМЭ-987", "Поршень");
// auto d_987_789 = Drawing("987-789", "Корпус");
// auto d_SZ_0101 = Drawing("СЗ-0101", "Тарелка");
// auto d_IME_123 = Drawing("ИМЭ-123", "Блок");

class TestStorageService final : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(TestStorageService)

public:
    TestStorageService() = default;

private slots:
//     void testAdd() {
//         QCOMPARE(storage.dates().add("25.08.2025"), 1);
//         QCOMPARE(storage.dates().count(), 1);

//         QCOMPARE(storage.dates().add("15.07.2024"), 2);
//         QCOMPARE(storage.dates().add("18.03.2020"), 3);
//         QCOMPARE(storage.dates().add("05.11.2021"), 4);
//         QCOMPARE(storage.dates().count(), 4);

//         QCOMPARE(storage.dates().add("18.03.2020"), 3);
//         QCOMPARE(storage.dates().count(), 4);

//         QCOMPARE(storage.dates().add("28.08.2019"), 5);
//         QCOMPARE(storage.dates().count(), 5);

//         QCOMPARE(storage.dates().add("55.08.2019"), std::nullopt);

//         QCOMPARE(storage.drawings().add(d_IMN_111), 1);
//         QCOMPARE(storage.drawings().add(d_111_11), 2);
//         QCOMPARE(storage.drawings().add(d_IME_987), 3);
//         QCOMPARE(storage.drawings().add(d_987_789), 4);
//         QCOMPARE(storage.drawings().add(d_SZ_0101), 5);
//         QCOMPARE(storage.drawings().add(d_IME_123), 6);
//         QCOMPARE(storage.drawings().count(), 6);
//     }

//     void testRemove() {
//         QCOMPARE(storage.dates().remove("05.11.2021"), true);
//         QCOMPARE(storage.dates().count(), 4);

//         QCOMPARE(storage.dates().remove("25.09.2008"), false);
//         QCOMPARE(storage.dates().count(), 4);

//         QCOMPARE(storage.dates().remove("18.03.2020"), true);
//         QCOMPARE(storage.dates().count(), 3);

//         QCOMPARE(storage.dates().remove("55.08.2019"), false);

//         QCOMPARE(storage.drawings().remove(d_IME_987), true);
//         QCOMPARE(storage.drawings().remove(d_987_789), true);
//         QCOMPARE(storage.drawings().count(), 4);
//     }

//     void testaddWithQueue() {
//         QCOMPARE(storage.dates().add("13.06.2015"), 4);
//         QCOMPARE(storage.dates().count(), 4);

//         QCOMPARE(storage.dates().add("26.03.2018"), 3);
//         QCOMPARE(storage.dates().count(), 5);

//         QCOMPARE(storage.dates().add("09.01.2019"), 6);
//         QCOMPARE(storage.dates().count(), 6);
//     }

//     void testGetId() {
//         QCOMPARE(storage.dates().findId("13.06.2015"), 4);
//         QCOMPARE(storage.dates().findId("15.07.2024"), 2);
//         QCOMPARE(storage.dates().findId("26.03.2018"), 3);
//         QCOMPARE(storage.dates().findId("28.08.2019"), 5);
//         QCOMPARE(storage.dates().findId("09.01.2019"), 6);
//         QCOMPARE(storage.dates().findId("25.08.2025"), 1);

//         QCOMPARE(storage.dates().findId("01.05.2017"), std::nullopt);
//         QCOMPARE(storage.dates().findId("55.05.2017"), std::nullopt);
//     }

//     void testGetDate() {
//         QCOMPARE(storage.dates().findStrValue(4), "13.06.2015");
//         QCOMPARE(storage.dates().findStrValue(2), "15.07.2024");
//         QCOMPARE(storage.dates().findStrValue(3), "26.03.2018");
//         QCOMPARE(storage.dates().findStrValue(5), "28.08.2019");
//         QCOMPARE(storage.dates().findStrValue(6), "09.01.2019");
//         QCOMPARE(storage.dates().findStrValue(1), "25.08.2025");

//         QCOMPARE(storage.dates().findStrValue(7), std::nullopt);

//         qDebug() << "----------   SAVE   ----------";
//         saver.save(storage);
//         saver.commit();
//         qDebug() << "Dates:";
//         storage.dates().printState();
//         qDebug() << "Drawings:";
//         storage.drawings().printState();

//         qDebug() << "----------   CLEAR   ---------";
//         storage.dates().clear();
//         storage.drawings().clear();
//         qDebug() << "Dates:";
//         storage.dates().printState();
//         qDebug() << "Drawings:";
//         storage.drawings().printState();

//         qDebug() << "----------   LOAD   ----------";
//         saver.load(storage);
//         qDebug() << "Dates:";
//         storage.dates().printState();
//         qDebug() << "Drawings:";
//         storage.drawings().printState();
//     }

//     void clear() {
//         QCOMPARE(storage.dates().count(), 6);
//         storage.dates().clear();
//         QCOMPARE(storage.dates().count(), 0);
//     }
};

QTEST_APPLESS_MAIN(TestStorageService)
#include "test_StorageService.moc"
