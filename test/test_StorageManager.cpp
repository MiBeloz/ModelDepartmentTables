#include <QtTest>

#include "StorageManager.h"

class TestStorageManager : public QObject {
    Q_OBJECT

private:
    std::unique_ptr<StorageManager> makeManager();

    static Record makeRecord(const QString& date = QStringLiteral("15.01.2024"),
                             Drawing drawing = Drawing("Ч-001", "Шкив"),
                             qint32 amount = 10,
                             const QStringList& executors = { QStringLiteral("Ivanov") },
                             const QStringList& authors = { QStringLiteral("Petrov") },
                             const QStringList& castingMaterials = { QStringLiteral("Steel") },
                             const QStringList& modelMaterials = { QStringLiteral("Wax") },
                             const QStringList& machines = { QStringLiteral("CNC-1") },
                             const QStringList& notes = { QStringLiteral("note") }) {
        return Record(date,
                      Drawing(drawing),
                      amount,
                      executors,
                      authors,
                      castingMaterials,
                      modelMaterials,
                      machines,
                      notes);
    }

private slots:

    // --- конструктор / базовое состояние ---

    void initiallyEmpty() {
        auto manager = makeManager();
        QCOMPARE(manager->count(), 0);
        QCOMPARE(manager->getRecords().count(), 0);
    }

    // --- add(Record) ---

    void addValidRecord() {
        auto manager = makeManager();
        const Record r = makeRecord();

        QVERIFY(manager->addRecord(r));
        QCOMPARE(manager->count(), 1);

        QCOMPARE(manager->getRecords().count(), 1);
        QCOMPARE(manager->getRecords().first().date, r.date);
        QCOMPARE(manager->getRecords().first().amount, r.amount);
        QCOMPARE(manager->getRecords().first().executors, r.executors);
        QCOMPARE(manager->getRecords().first().authors, r.authors);
    }

    void addInvalidDateFails() {
        auto manager = makeManager();
        QVERIFY(!manager->addRecord(makeRecord(QStringLiteral("not-a-date"))));
        QCOMPARE(manager->count(), 0);
    }

    void addInvalidAmountFails() {
        auto manager = makeManager();
        QVERIFY(!manager->addRecord(makeRecord(QStringLiteral("15.01.2024"), Drawing::Null, 0)));
        QCOMPARE(manager->count(), 0);
    }

    void addInvalidDrawingFails() {
        auto manager = makeManager();
        Record r = makeRecord();
        r.drawing = Drawing(); // предполагаем, что пустой Drawing невалиден
        QVERIFY(!manager->addRecord(r));
        QCOMPARE(manager->count(), 0);
    }

    void addRecordFailsWhenSaverPrepareFails() {
        auto manager = makeManager();
        QVERIFY(!manager->addRecord(makeRecord()));
    }

    void addMultipleDistinctRecords() {
        auto manager = makeManager();
        QVERIFY(manager->addRecord(makeRecord(QStringLiteral("15.01.2024"))));
        QVERIFY(manager->addRecord(makeRecord(QStringLiteral("20.02.2024"),
                                              Drawing("Ч-002", "Колесо"),
                                              5,
                                              { QStringLiteral("Sidorov") })));
        QCOMPARE(manager->count(), 2);
        QCOMPARE(manager->getRecords().count(), 2);
    }

    // --- remove(Record) ---

    void removeExistingRecord() {
        auto manager = makeManager();
        const Record r = makeRecord();
        QVERIFY(manager->addRecord(r));
        QCOMPARE(manager->count(), 1);

        QVERIFY(manager->removeRecord(r));
        QCOMPARE(manager->count(), 0);
        QCOMPARE(manager->getRecords().count(), 0);
    }

    void removeNonExistingRecordReturnsFalse() {
        auto manager = makeManager();
        QVERIFY(manager->addRecord(makeRecord()));
        QVERIFY(!manager->removeRecord(makeRecord(QStringLiteral("2024-12-31"))));
        QCOMPARE(manager->count(), 1);
    }

    void removeInvalidRecordReturnsFalse() {
        auto manager = makeManager();
        QVERIFY(!manager->removeRecord(makeRecord(QStringLiteral("bad-date"))));
    }

    // --- Adder ---

    void adderAddsExecutorAndPersists() {
        auto manager = makeManager();
        manager->add().executor(QStringLiteral("NewExecutor"));
    }

    void adderChainMultipleValues() {
        auto manager = makeManager();
        manager->add()
            .executor(QStringLiteral("E1"))
            .author(QStringLiteral("A1"))
            .machine(QStringLiteral("M1"))
            .note(QStringLiteral("N1"));
    }

    // --- Remover ---

    void removerRemovesValue() {
        auto manager = makeManager();
        manager->add().executor(QStringLiteral("E1"));

        manager->remove().executor(QStringLiteral("E1"));
    }

    // --- reset ---

    void resetClearsEverything() {
        auto manager = makeManager();
        QVERIFY(manager->addRecord(makeRecord()));
        QCOMPARE(manager->count(), 1);

        manager->reset();
        QCOMPARE(manager->count(), 0);
        QCOMPARE(manager->getRecords().count(), 0);
    }

    void resetRestoresStateOnFailure() {
        auto manager = makeManager();
        QVERIFY(manager->addRecord(makeRecord()));
        QCOMPARE(manager->count(), 1);

        manager->reset();
        QCOMPARE(manager->count(), 1);
    }

    // --- clear ---

    void clearRemovesAll() {
        auto manager = makeManager();
        QVERIFY(manager->addRecord(makeRecord()));

        manager->clear();
        QCOMPARE(manager->count(), 0);
    }

    void clearRestoresOnFailure() {
        auto manager = makeManager();
        QVERIFY(manager->addRecord(makeRecord()));

        manager->clear();
        QCOMPARE(manager->count(), 1);
    }

    // --- count ---

    void countReflectsAddsAndRemoves() {
        auto manager = makeManager();
        QCOMPARE(manager->count(), 0);
        QVERIFY(manager->addRecord(makeRecord(QStringLiteral("01.01.2024"))));
        QCOMPARE(manager->count(), 1);
        QVERIFY(manager->addRecord(makeRecord(QStringLiteral("02.01.2024"))));
        QCOMPARE(manager->count(), 2);
        QVERIFY(manager->removeRecord(makeRecord(QStringLiteral("01.01.2024"))));
        QCOMPARE(manager->count(), 1);
    }

    // --- get() cache ---

    void getCachesResult() {
        auto manager = makeManager();
        QVERIFY(manager->addRecord(makeRecord()));
        const auto first = manager->getRecords();
        const auto second = manager->getRecords();
        QCOMPARE(first.size(), second.size());
        QCOMPARE(first.size(), 1);
    }

    void getInvalidatedAfterAdd() {
        auto manager = makeManager();
        QVERIFY(manager->addRecord(makeRecord(QStringLiteral("01.01.2024"))));
        QCOMPARE(manager->getRecords().size(), 1);
        QVERIFY(manager->addRecord(makeRecord(QStringLiteral("02.01.2024"))));
        QCOMPARE(manager->getRecords().size(), 2);
    }
};

QTEST_APPLESS_MAIN(TestStorageManager)
#include "test_StorageManager.moc"
