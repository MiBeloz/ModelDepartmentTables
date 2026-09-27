#include <QDebug>
#include <QObject>
#include <QTest>

#include "StorageRecord.h"

StorageRecord storage;

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

QDebug operator <<(QDebug out, const Record& record) {
    out << "\nRecord:" << Qt::endl;
    out << '\t' << record.date << Qt::endl;
    out << '\t' << record.drawing.getNumber() << '-' << record.drawing.getTitle() << Qt::endl;
    out << '\t' << record.amount << Qt::endl;
    out << '\t' << record.executors << Qt::endl;
    out << '\t' << record.authors << Qt::endl;
    out << '\t' << record.castingMaterials << Qt::endl;
    out << '\t' << record.modelMaterials << Qt::endl;
    out << '\t' << record.machines << Qt::endl;
    out << '\t' << record.notes << Qt::endl;
    return out;
}

class TestStorageRecord final : public QObject {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(TestStorageRecord)

public:
    TestStorageRecord() = default;

private:
    QList<Record> records;

private slots:
    void testAdd() {
        QVERIFY(storage.add(rec1));
        QVERIFY(!storage.add(rec2e));
        QVERIFY(!storage.add(rec3e));
        QVERIFY(storage.add(rec4));
        QVERIFY(storage.add(rec5));
        QVERIFY(storage.add(rec6));
        QVERIFY(storage.add(rec1));

        QCOMPARE(storage.count(), 4);

        QVERIFY(storage.commit());

        records = storage.get();

        qDebug() << "---------- testAdd ----------";
        QList<Record> records = storage.get();
        for (int i = 0; i < records.count(); ++i) {
            auto r = records.at(i);
            qDebug() << r;
        }
    }

    void testRemove() {
        QVERIFY(storage.remove(rec1));
        QCOMPARE(storage.count(), 3);

        QVERIFY(!storage.remove(rec1));
        QCOMPARE(storage.count(), 3);

        QVERIFY(!storage.remove(rec2e));
        QCOMPARE(storage.count(), 3);

        QVERIFY(storage.remove(rec6));
        QCOMPARE(storage.count(), 2);

        qDebug() << "---------- testRemove ----------";
        QList<Record> records = storage.get();
        for (int i = 0; i < records.count(); ++i) {
            auto r = records.at(i);
            qDebug() << r;
        }
    }

    void testReset() {
        QVERIFY(storage.reset());

        QCOMPARE(storage.count(), 4);

        QCOMPARE(records, storage.get());

        qDebug() << "---------- testReset ----------";
        QList<Record> records = storage.get();
        for (int i = 0; i < records.count(); ++i) {
            auto r = records.at(i);
            qDebug() << r;
        }
    }

    void testSaveAndLoad() {
        QVERIFY(storage.commit());
        QVERIFY(storage.save());

        storage.clear();
        QVERIFY(storage.commit());
        QCOMPARE(storage.count(), 0);

        QVERIFY(storage.load());
        QCOMPARE(storage.count(), 4);

        QCOMPARE(records, storage.get());

        qDebug() << "---------- testSaveAndLoad ----------";
        QList<Record> records = storage.get();
        for (int i = 0; i < records.count(); ++i) {
            auto r = records.at(i);
            qDebug() << r;
        }
    }
};

QTEST_APPLESS_MAIN(TestStorageRecord)
#include "test_StorageRecord.moc"
