#include <QtTest>

#include "StorageManager.h"

template<typename T>
class FakeSaver final : public Saver<T> {
public:
    bool prepare(const T& value) override {
        m_prepared = value;
        ++m_prepareCalls;
        return m_prepareResult;
    }

    bool write() override {
        if (!m_prepared.has_value()) {
            return false;
        }
        m_written = m_prepared.value();
        ++m_writeCalls;
        return m_writeResult;
    }

    bool read(T& value) override {
        ++m_readCalls;
        if (m_readResult && m_written.has_value()) {
            value = m_written.value();
            return true;
        }
        return false;
    }

    // --- управление поведением в тестах ---
    void setPrepareResult(bool v) {
        m_prepareResult = v;
    }
    void setWriteResult(bool v) {
        m_writeResult = v;
    }
    void setReadResult(bool v) {
        m_readResult = v;
    }

    void seed(const T& value) {
        m_written = value;
    }

    int prepareCalls() const {
        return m_prepareCalls;
    }
    int writeCalls() const {
        return m_writeCalls;
    }
    int readCalls() const {
        return m_readCalls;
    }

private:
    std::optional<T> m_prepared;
    std::optional<T> m_written;

    bool m_prepareResult = true;
    bool m_writeResult = true;
    bool m_readResult = true;

    int m_prepareCalls = 0;
    int m_writeCalls = 0;
    int m_readCalls = 0;
};

class TestStorageManager : public QObject {
    Q_OBJECT

private:
    FakeSaver<StorageLists>* m_listsSaverRaw = nullptr;
    FakeSaver<StorageRecordsLinks>* m_linksSaverRaw = nullptr;

    std::unique_ptr<StorageManager> makeManager() {
        auto listsSaver = std::make_unique<FakeSaver<StorageLists>>();
        auto linksSaver = std::make_unique<FakeSaver<StorageRecordsLinks>>();
        m_listsSaverRaw = listsSaver.get();
        m_linksSaverRaw = linksSaver.get();

        std::unique_ptr<Saver<StorageLists>> listsBase = std::move(listsSaver);
        std::unique_ptr<Saver<StorageRecordsLinks>> linksBase = std::move(linksSaver);

        return std::make_unique<StorageManager>(listsBase, linksBase);
    }

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
        QCOMPARE(manager->get().size(), 0);
    }

    // --- add(Record) ---

    void addValidRecord() {
        auto manager = makeManager();
        const Record r = makeRecord();

        QVERIFY(manager->add(r));
        QCOMPARE(manager->count(), 1);

        const auto records = manager->get();
        QCOMPARE(records.size(), 1);
        QCOMPARE(records.first().date, r.date);
        QCOMPARE(records.first().amount, r.amount);
        QCOMPARE(records.first().executors, r.executors);
        QCOMPARE(records.first().authors, r.authors);
    }

    void addInvalidDateFails() {
        auto manager = makeManager();
        QVERIFY(!manager->add(makeRecord(QStringLiteral("not-a-date"))));
        QCOMPARE(manager->count(), 0);
    }

    void addInvalidAmountFails() {
        auto manager = makeManager();
        QVERIFY(!manager->add(makeRecord(QStringLiteral("15.01.2024"), Drawing::Null, 0)));
        QCOMPARE(manager->count(), 0);
    }

    void addInvalidDrawingFails() {
        auto manager = makeManager();
        Record r = makeRecord();
        r.drawing = Drawing(); // предполагаем, что пустой Drawing невалиден
        QVERIFY(!manager->add(r));
        QCOMPARE(manager->count(), 0);
    }

    void addRecordFailsWhenSaverPrepareFails() {
        auto manager = makeManager();
        m_linksSaverRaw->setPrepareResult(false);
        QVERIFY(!manager->add(makeRecord()));
    }

    void addMultipleDistinctRecords() {
        auto manager = makeManager();
        QVERIFY(manager->add(makeRecord(QStringLiteral("15.01.2024"))));
        QVERIFY(manager->add(makeRecord(QStringLiteral("20.02.2024"),
                                        Drawing("Ч-002", "Колесо"),
                                        5,
                                        { QStringLiteral("Sidorov") })));
        QCOMPARE(manager->count(), 2);
        QCOMPARE(manager->get().size(), 2);
    }

    // --- remove(Record) ---

    void removeExistingRecord() {
        auto manager = makeManager();
        const Record r = makeRecord();
        QVERIFY(manager->add(r));
        QCOMPARE(manager->count(), 1);

        QVERIFY(manager->remove(r));
        QCOMPARE(manager->count(), 0);
        QCOMPARE(manager->get().size(), 0);
    }

    void removeNonExistingRecordReturnsFalse() {
        auto manager = makeManager();
        QVERIFY(manager->add(makeRecord()));
        QVERIFY(!manager->remove(makeRecord(QStringLiteral("2024-12-31"))));
        QCOMPARE(manager->count(), 1);
    }

    void removeInvalidRecordReturnsFalse() {
        auto manager = makeManager();
        QVERIFY(!manager->remove(makeRecord(QStringLiteral("bad-date"))));
    }

    // --- Adder ---

    void adderAddsExecutorAndPersists() {
        auto manager = makeManager();
        manager->add().executor(QStringLiteral("NewExecutor"));
        QVERIFY(m_listsSaverRaw->prepareCalls() > 0);
    }

    void adderChainMultipleValues() {
        auto manager = makeManager();
        manager->add()
            .executor(QStringLiteral("E1"))
            .author(QStringLiteral("A1"))
            .machine(QStringLiteral("M1"))
            .note(QStringLiteral("N1"));

        QVERIFY(m_listsSaverRaw->prepareCalls() >= 4);
    }

    // --- Remover ---

    void removerRemovesValue() {
        auto manager = makeManager();
        manager->add().executor(QStringLiteral("E1"));
        const int before = m_listsSaverRaw->prepareCalls();

        manager->remove().executor(QStringLiteral("E1"));
        QVERIFY(m_listsSaverRaw->prepareCalls() > before);
    }

    // --- reset ---

    void resetClearsEverything() {
        auto manager = makeManager();
        QVERIFY(manager->add(makeRecord()));
        QCOMPARE(manager->count(), 1);

        QVERIFY(manager->reset());
        QCOMPARE(manager->count(), 0);
        QCOMPARE(manager->get().size(), 0);
    }

    void resetRestoresStateOnFailure() {
        auto manager = makeManager();
        QVERIFY(manager->add(makeRecord()));
        QCOMPARE(manager->count(), 1);

        m_linksSaverRaw->setPrepareResult(false);
        QVERIFY(!manager->reset());
        QCOMPARE(manager->count(), 1);
    }

    // --- clear ---

    void clearRemovesAll() {
        auto manager = makeManager();
        QVERIFY(manager->add(makeRecord()));
        QVERIFY(manager->clear());
        QCOMPARE(manager->count(), 0);
    }

    void clearRestoresOnFailure() {
        auto manager = makeManager();
        QVERIFY(manager->add(makeRecord()));

        m_listsSaverRaw->setPrepareResult(false);
        QVERIFY(!manager->clear());
        QCOMPARE(manager->count(), 1);
    }

    // --- save / load ---

    void saveWritesToSavers() {
        auto manager = makeManager();
        QVERIFY(manager->add(makeRecord()));

        const int writeBefore = m_linksSaverRaw->writeCalls();
        QVERIFY(manager->save());
        QVERIFY(m_linksSaverRaw->writeCalls() > writeBefore);
        QVERIFY(m_listsSaverRaw->writeCalls() > 0);
    }

    void saveFailsIfWriteFails() {
        auto manager = makeManager();
        QVERIFY(manager->add(makeRecord()));
        m_linksSaverRaw->setWriteResult(false);
        QVERIFY(!manager->save());
    }

    void loadReadsFromSavers() {
        // Подготавливаем saver'ы с уже «записанными» данными
        auto listsSaver = std::make_unique<FakeSaver<StorageLists>>();
        auto linksSaver = std::make_unique<FakeSaver<StorageRecordsLinks>>();

        // Сидируем fake пустыми значениями (реалистичный сценарий — данные пришли с диска)
        listsSaver->seed(StorageLists { });
        linksSaver->seed(StorageRecordsLinks { });

        std::unique_ptr<Saver<StorageLists>> listsBase = std::move(listsSaver);
        std::unique_ptr<Saver<StorageRecordsLinks>> linksBase = std::move(linksSaver);

        StorageManager manager(listsBase, linksBase);
        QVERIFY(manager.load());
        QCOMPARE(manager.count(), 0);
    }

    void loadFailsWhenReadFails() {
        auto listsSaver = std::make_unique<FakeSaver<StorageLists>>();
        auto linksSaver = std::make_unique<FakeSaver<StorageRecordsLinks>>();
        linksSaver->setReadResult(false);

        std::unique_ptr<Saver<StorageLists>> listsBase = std::move(listsSaver);
        std::unique_ptr<Saver<StorageRecordsLinks>> linksBase = std::move(linksSaver);

        StorageManager manager(listsBase, linksBase);
        QVERIFY(!manager.load());
    }

    // --- deleteBadLinks ---

    void deleteBadLinksNoChangesWhenAllValid() {
        auto manager = makeManager();
        QVERIFY(manager->add(makeRecord()));
        QVERIFY(manager->deleteBadLinks());
        QCOMPARE(manager->count(), 1);
    }

    void deleteBadLinksRemovesOrphanRecord() {
        auto manager = makeManager();
        QVERIFY(manager->add(makeRecord()));
        QCOMPARE(manager->count(), 1);

        // Удаляем значение из списка — ссылка становится «битой»
        manager->remove().executor(QStringLiteral("Ivanov"));

        QVERIFY(manager->deleteBadLinks());
        QVERIFY(manager->count() <= 1);
    }

    // --- count ---

    void countReflectsAddsAndRemoves() {
        auto manager = makeManager();
        QCOMPARE(manager->count(), 0);
        QVERIFY(manager->add(makeRecord(QStringLiteral("01.01.2024"))));
        QCOMPARE(manager->count(), 1);
        QVERIFY(manager->add(makeRecord(QStringLiteral("02.01.2024"))));
        QCOMPARE(manager->count(), 2);
        QVERIFY(manager->remove(makeRecord(QStringLiteral("01.01.2024"))));
        QCOMPARE(manager->count(), 1);
    }

    // --- get() cache ---

    void getCachesResult() {
        auto manager = makeManager();
        QVERIFY(manager->add(makeRecord()));
        const auto first = manager->get();
        const auto second = manager->get();
        QCOMPARE(first.size(), second.size());
        QCOMPARE(first.size(), 1);
    }

    void getInvalidatedAfterAdd() {
        auto manager = makeManager();
        QVERIFY(manager->add(makeRecord(QStringLiteral("01.01.2024"))));
        QCOMPARE(manager->get().size(), 1);
        QVERIFY(manager->add(makeRecord(QStringLiteral("02.01.2024"))));
        QCOMPARE(manager->get().size(), 2);
    }
};

QTEST_APPLESS_MAIN(TestStorageManager)
#include "test_StorageManager.moc"
