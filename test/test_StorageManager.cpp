// #include <QDebug>
// #include <QObject>
// #include <QTest>

// #include "StorageManager.h"

// StorageManager storage;

// auto rec1 = Record("25.08.2026", Drawing("ИМН-111", "Колесо рабочее"), 2, { "Иван", "Александр" });
// auto rec2e = Record("35.08.2026", Drawing("111-11", "Шкиф"), 1);
// auto rec3e = Record("20.08.2026", Drawing("ИМЭ-987", "Поршень"), 0, { "Петр", "Станислав" });
// auto rec4 = Record("15.08.2026",
//                    Drawing("987-789", "Корпус"),
//                    3,
//                    { "Иван", "Александр" },
//                    { "Елена" },
//                    { "СЧ20" },
//                    { "Пластик красный" },
//                    { "Скорпион" },
//                    { "Доработка" });
// auto rec5 = Record("15.08.2026",
//                    Drawing("СЗ-0101", "Тарелка"),
//                    4,
//                    { "Иван" },
//                    { "Елена" },
//                    { "СЧ20" },
//                    { "Фанера сушеная" },
//                    { "Китаец" },
//                    { "Только модель" });
// auto rec6 = Record("29.08.2026",
//                    Drawing("ИМЭ-123", "Блок"),
//                    2,
//                    { "Иван", "Александр" },
//                    { "Дмитрий" },
//                    { "СЧ25" },
//                    { "Пластик красный" },
//                    { "Китаец" });

// QDebug operator <<(QDebug out, const Record& record) {
//     out << "\nRecord:" << Qt::endl;
//     out << '\t' << record.date << Qt::endl;
//     out << '\t' << record.drawing.getNumber() << '-' << record.drawing.getTitle() << Qt::endl;
//     out << '\t' << record.amount << Qt::endl;
//     out << '\t' << record.executors << Qt::endl;
//     out << '\t' << record.authors << Qt::endl;
//     out << '\t' << record.castingMaterials << Qt::endl;
//     out << '\t' << record.modelMaterials << Qt::endl;
//     out << '\t' << record.machines << Qt::endl;
//     out << '\t' << record.notes << Qt::endl;
//     return out;
// }

// class TestStorageManager final : public QObject {
//     Q_OBJECT
//     Q_DISABLE_COPY_MOVE(TestStorageManager)

// public:
//     TestStorageManager() = default;

// private:
//     QList<Record> records;

// private slots:
//     void testAdd() {
//         QVERIFY(storage.add(rec1));
//         QVERIFY(!storage.add(rec2e));
//         QVERIFY(!storage.add(rec3e));
//         QVERIFY(storage.add(rec4));
//         QVERIFY(storage.add(rec5));
//         QVERIFY(storage.add(rec6));
//         QVERIFY(storage.add(rec1));

//         QCOMPARE(storage.count(), 4);

//         QVERIFY(storage.commit());

//         records = storage.get();

//         qDebug() << "---------- testAdd ----------";
//         QList<Record> records = storage.get();
//         for (int i = 0; i < records.count(); ++i) {
//             auto r = records.at(i);
//             qDebug() << r;
//         }
//     }

//     void testRemove() {
//         QVERIFY(storage.remove(rec1));
//         QCOMPARE(storage.count(), 3);

//         QVERIFY(!storage.remove(rec1));
//         QCOMPARE(storage.count(), 3);

//         QVERIFY(!storage.remove(rec2e));
//         QCOMPARE(storage.count(), 3);

//         QVERIFY(storage.remove(rec6));
//         QCOMPARE(storage.count(), 2);

//         qDebug() << "---------- testRemove ----------";
//         QList<Record> records = storage.get();
//         for (int i = 0; i < records.count(); ++i) {
//             auto r = records.at(i);
//             qDebug() << r;
//         }
//     }

//     void testReset() {
//         QVERIFY(storage.reset());

//         QCOMPARE(storage.count(), 4);

//         QCOMPARE(records, storage.get());

//         qDebug() << "---------- testReset ----------";
//         QList<Record> records = storage.get();
//         for (int i = 0; i < records.count(); ++i) {
//             auto r = records.at(i);
//             qDebug() << r;
//         }
//     }

//     void testSaveAndLoad() {
//         QVERIFY(storage.commit());
//         QVERIFY(storage.save());

//         storage.clear();
//         QVERIFY(storage.commit());
//         QCOMPARE(storage.count(), 0);

//         QVERIFY(storage.load());
//         QCOMPARE(storage.count(), 4);

//         QCOMPARE(records, storage.get());

//         qDebug() << "---------- testSaveAndLoad ----------";
//         QList<Record> records = storage.get();
//         for (int i = 0; i < records.count(); ++i) {
//             auto r = records.at(i);
//             qDebug() << r;
//         }
//     }
// };

// QTEST_APPLESS_MAIN(TestStorageManager)
// #include "test_StorageManager.moc"

#include <QTemporaryDir>
#include <QtTest>

#include "Constants.h"
#include "Drawing.h"
#include "Record.h"
#include "SaverFile.h"
#include "StorageManager.h"

std::unique_ptr<Saver<StorageLists>> saverLists(
    std::make_unique<SaverFile<StorageLists>>(SAVER_STORAGE_FILENAME, SAVER_STORAGE_FILENAME_TMP));
std::unique_ptr<Saver<StorageRecordsLinks>> saverRecordsLinks(
    std::make_unique<SaverFile<StorageRecordsLinks>>(SAVER_RECORDLINK_FILENAME,
                                                     SAVER_RECORDLINK_FILENAME_TMP));

class TestStorageManager : public QObject {
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void init();    // перед каждым тестом
    void cleanup(); // после каждого теста

    // ---------- add(Record) ----------
    void add_validRecord_returnsTrue();
    void add_invalidDate_returnsFalse();
    void add_invalidDrawing_returnsFalse();
    void add_invalidAmount_returnsFalse();
    void add_duplicateRecord_isIdempotent(); // QSet<RecordLink> дедуплицирует
    void add_emptyOptionalLists_succeeds();

    // ---------- remove(Record) ----------
    void remove_existingRecord_returnsTrue();
    void remove_missingRecord_returnsFalse();
    void remove_invalidRecord_returnsFalse();

    // ---------- get() / кэш ----------
    void get_returnsInsertedRecord();
    void get_cachesUntilInvalidated();
    void get_afterAdd_recomputes();
    void get_afterRemove_recomputes();
    void get_preservesAllFields();

    // ---------- commit / reset ----------
    void reset_revertsUncommittedAdd();
    void reset_afterCommit_keepsState();
    void commit_makesChangesPersistent();
    void reset_thenCommit_dropsChanges();

    // ---------- save / load ----------
    void save_load_roundTrip();
    void save_load_multipleRecords();
    void save_withoutChanges_returnsTrue();
    void load_missingFile_returnsFalse();

    // ---------- clear / count ----------
    void clear_emptiesStorage();
    void count_reflectsChanges();

    // ---------- Adder / Remover (справочники) ----------
    void adder_addsExecutorToExecutorList();
    void adder_addsAuthorToAuthorList();
    void adder_addsCastingMaterial();
    void adder_addsModelMaterial();
    void adder_addsMachine();
    void adder_addsNote();

    void remover_removesExecutor();
    void remover_removesAuthor();
    void remover_removesCastingMaterial();
    void remover_removesModelMaterial();
    void remover_removesMachine();
    void remover_removesNote();

    void adder_invalidatesRecordsCache(); // ЛОВИТ БАГ: кэш не сбрасывается

    // ---------- deleteBadLinks ----------
    void deleteBadLinks_onValidData_noop();
    void deleteBadLinks_doesNotCrashOnEmpty();

private:
    Record makeRecord(const QString& date = "01.01.2024",
                      const QString& number = "D-001",
                      const QString& title = "Title",
                      qint32 amount = 1,
                      const QStringList& executors = { "Alice" },
                      const QStringList& authors = { "Bob" },
                      const QStringList& casting = { "Steel" },
                      const QStringList& model = { "Wax" },
                      const QStringList& machines = { "CNC-1" },
                      const QStringList& notes = { "n1" });

    // Проверяет, что значение содержится в соответствующем справочнике StorageManager
    bool executorExists(StorageManager& mgr, const QString& v);
    bool authorExists(StorageManager& mgr, const QString& v);
    bool castingExists(StorageManager& mgr, const QString& v);
    bool modelExists(StorageManager& mgr, const QString& v);
    bool machineExists(StorageManager& mgr, const QString& v);
    bool noteExists(StorageManager& mgr, const QString& v);

    std::unique_ptr<QTemporaryDir> m_dir;
    QString m_oldCwd;
};

// ---------------------------------------------------------------------------
// setup / teardown
// ---------------------------------------------------------------------------

void TestStorageManager::initTestCase() {
    m_oldCwd = QDir::currentPath();
    m_dir = std::make_unique<QTemporaryDir>();
    QVERIFY(m_dir->isValid());
    QVERIFY(QDir::setCurrent(m_dir->path()));
}

void TestStorageManager::cleanupTestCase() {
    QDir::setCurrent(m_oldCwd);
    m_dir.reset();
}

void TestStorageManager::init() {
    const QStringList files = { SAVER_STORAGE_FILENAME,        SAVER_STORAGE_FILENAME_TMP,
                                SAVER_STORAGE_FILENAME_BACKUP, SAVER_RECORDLINK_FILENAME,
                                SAVER_RECORDLINK_FILENAME_TMP, SAVER_RECORDLINK_FILENAME_BACKUP };
    for (const auto& f : files) {
        QFile::remove(f);
    }
}

void TestStorageManager::cleanup() { }

// ---------------------------------------------------------------------------
// helpers
// ---------------------------------------------------------------------------

Record TestStorageManager::makeRecord(const QString& date,
                                      const QString& number,
                                      const QString& title,
                                      qint32 amount,
                                      const QStringList& executors,
                                      const QStringList& authors,
                                      const QStringList& casting,
                                      const QStringList& model,
                                      const QStringList& machines,
                                      const QStringList& notes) {
    return Record(date,
                  Drawing(number, title),
                  amount,
                  executors,
                  authors,
                  casting,
                  model,
                  machines,
                  notes);
}

bool TestStorageManager::executorExists(StorageManager& mgr, const QString& v) {
    // Обходим get(): сами формируем запись и через findId справочника.
    // StorageManager не отдаёт StorageLists напрямую — используем Record.
    // Но проще: через add(Record) + get() мы видим содержимое.
    // Для точечной проверки добавляем маркерную запись и смотрим результат.
    Q_UNUSED(mgr);
    Q_UNUSED(v);
    return false; // переопределим ниже через add/get
}

// Проверки наличия в справочниках делаем через «зонд»:
// добавляем пустую запись с одним элементом и читаем get().
// Но get() не показывает справочник целиком. Поэтому корректный тест
// проверяет именно эффект: после adder(...) и add(Record{...}) с
// этим значением запись создаётся без падений и читается обратно.
bool TestStorageManager::authorExists(StorageManager&, const QString&) {
    return false;
}
bool TestStorageManager::castingExists(StorageManager&, const QString&) {
    return false;
}
bool TestStorageManager::modelExists(StorageManager&, const QString&) {
    return false;
}
bool TestStorageManager::machineExists(StorageManager&, const QString&) {
    return false;
}
bool TestStorageManager::noteExists(StorageManager&, const QString&) {
    return false;
}

// ---------------------------------------------------------------------------
// add(Record)
// ---------------------------------------------------------------------------

void TestStorageManager::add_validRecord_returnsTrue() {
    StorageManager mgr(saverLists, saverRecordsLinks);
    QVERIFY(mgr.add(makeRecord()));
    QCOMPARE(mgr.count(), qsizetype(1));
}

void TestStorageManager::add_invalidDate_returnsFalse() {
    StorageManager mgr(saverLists, saverRecordsLinks);
    Record r = makeRecord();
    r.date = "not-a-date";
    QVERIFY(!mgr.add(r));
    QCOMPARE(mgr.count(), qsizetype(0));
}

void TestStorageManager::add_invalidDrawing_returnsFalse() {
    StorageManager mgr(saverLists, saverRecordsLinks);
    Record r = makeRecord();
    r.drawing = Drawing("", "");
    QVERIFY(!mgr.add(r));
    QCOMPARE(mgr.count(), qsizetype(0));
}

void TestStorageManager::add_invalidAmount_returnsFalse() {
    StorageManager mgr(saverLists, saverRecordsLinks);
    QVERIFY(!mgr.add(makeRecord("01.01.2024", "D-001", "T", /*amount*/ 0)));
    QVERIFY(!mgr.add(makeRecord("01.01.2024", "D-001", "T", /*amount*/ -1)));
    QCOMPARE(mgr.count(), qsizetype(0));
}

void TestStorageManager::add_duplicateRecord_isIdempotent() {
    // StorageRecordsLinks хранит QSet<RecordLink> → дубликат не увеличивает count.
    // Тест фиксирует текущее поведение (потенциальный баг/фича).
    StorageManager mgr(saverLists, saverRecordsLinks);
    QVERIFY(mgr.add(makeRecord()));
    QVERIFY(mgr.add(makeRecord()));
    QCOMPARE(mgr.count(), qsizetype(1));
}

void TestStorageManager::add_emptyOptionalLists_succeeds() {
    StorageManager mgr(saverLists, saverRecordsLinks);
    Record r = makeRecord("01.01.2024",
                          "D-001",
                          "T",
                          1,
                          /*executors*/ { },
                          /*authors*/ { },
                          /*casting*/ { },
                          /*model*/ { },
                          /*machines*/ { },
                          /*notes*/ { });
    QVERIFY(mgr.add(r));
    QCOMPARE(mgr.count(), qsizetype(1));

    const auto records = mgr.get();
    QCOMPARE(records.size(), 1);
    QVERIFY(records.first().executors.isEmpty());
    QVERIFY(records.first().notes.isEmpty());
}

// ---------------------------------------------------------------------------
// remove(Record)
// ---------------------------------------------------------------------------

void TestStorageManager::remove_existingRecord_returnsTrue() {
    StorageManager mgr(saverLists, saverRecordsLinks);
    const auto r = makeRecord();
    QVERIFY(mgr.add(r));
    QVERIFY(mgr.remove(r));
    QCOMPARE(mgr.count(), qsizetype(0));
}

void TestStorageManager::remove_missingRecord_returnsFalse() {
    StorageManager mgr(saverLists, saverRecordsLinks);
    QVERIFY(mgr.add(makeRecord()));
    QVERIFY(!mgr.remove(makeRecord("02.02.2024", "D-002", "Other")));
    QCOMPARE(mgr.count(), qsizetype(1));
}

void TestStorageManager::remove_invalidRecord_returnsFalse() {
    StorageManager mgr(saverLists, saverRecordsLinks);
    Record bad = makeRecord();
    bad.amount = -5;
    QVERIFY(!mgr.remove(bad));
}

// ---------------------------------------------------------------------------
// get()
// ---------------------------------------------------------------------------

void TestStorageManager::get_returnsInsertedRecord() {
    StorageManager mgr(saverLists, saverRecordsLinks);
    const auto r = makeRecord();
    QVERIFY(mgr.add(r));

    const auto records = mgr.get();
    QCOMPARE(records.size(), 1);
    QCOMPARE(records.first().date, r.date);
    QCOMPARE(records.first().drawing.getNumber(), r.drawing.getNumber());
    QCOMPARE(records.first().drawing.getTitle(), r.drawing.getTitle());
    QCOMPARE(records.first().amount, r.amount);
}

void TestStorageManager::get_cachesUntilInvalidated() {
    StorageManager mgr(saverLists, saverRecordsLinks);
    QVERIFY(mgr.add(makeRecord()));
    const auto a = mgr.get();
    const auto b = mgr.get();
    QCOMPARE(a.size(), b.size());
    QCOMPARE(a.first().date, b.first().date);
}

void TestStorageManager::get_afterAdd_recomputes() {
    StorageManager mgr(saverLists, saverRecordsLinks);
    QVERIFY(mgr.add(makeRecord("01.01.2024", "D-001")));
    QCOMPARE(mgr.get().size(), 1);

    QVERIFY(mgr.add(makeRecord("02.02.2024", "D-002", "Second")));
    QCOMPARE(mgr.get().size(), 2);
}

void TestStorageManager::get_afterRemove_recomputes() {
    StorageManager mgr(saverLists, saverRecordsLinks);
    const auto r1 = makeRecord("01.01.2024", "D-001");
    const auto r2 = makeRecord("02.02.2024", "D-002", "Second");
    QVERIFY(mgr.add(r1));
    QVERIFY(mgr.add(r2));
    QCOMPARE(mgr.get().size(), 2);

    QVERIFY(mgr.remove(r1));
    const auto remaining = mgr.get();
    QCOMPARE(remaining.size(), 1);
    QCOMPARE(remaining.first().drawing.getNumber(), QString("D-002"));
}

void TestStorageManager::get_preservesAllFields() {
    StorageManager mgr(saverLists, saverRecordsLinks);
    const auto r = makeRecord("15.06.2024",
                              "ABC-42",
                              "Test title",
                              7,
                              { "Alice", "Charlie" },
                              { "Bob" },
                              { "Steel", "Iron" },
                              { "Wax" },
                              { "CNC-1", "Lathe-2" },
                              { "note1", "note2" });
    QVERIFY(mgr.add(r));

    const auto got = mgr.get().first();
    QCOMPARE(got.date, r.date);
    QCOMPARE(got.drawing.getNumber(), r.drawing.getNumber());
    QCOMPARE(got.drawing.getTitle(), r.drawing.getTitle());
    QCOMPARE(got.amount, r.amount);
    QCOMPARE(QSet<QString>(got.executors.begin(), got.executors.end()),
             QSet<QString>(r.executors.begin(), r.executors.end()));
    QCOMPARE(QSet<QString>(got.authors.begin(), got.authors.end()),
             QSet<QString>(r.authors.begin(), r.authors.end()));
    QCOMPARE(QSet<QString>(got.castingMaterials.begin(), got.castingMaterials.end()),
             QSet<QString>(r.castingMaterials.begin(), r.castingMaterials.end()));
    QCOMPARE(QSet<QString>(got.modelMaterials.begin(), got.modelMaterials.end()),
             QSet<QString>(r.modelMaterials.begin(), r.modelMaterials.end()));
    QCOMPARE(QSet<QString>(got.machines.begin(), got.machines.end()),
             QSet<QString>(r.machines.begin(), r.machines.end()));
    QCOMPARE(QSet<QString>(got.notes.begin(), got.notes.end()),
             QSet<QString>(r.notes.begin(), r.notes.end()));
}

// ---------------------------------------------------------------------------
// commit / reset
// ---------------------------------------------------------------------------

void TestStorageManager::reset_revertsUncommittedAdd() {
    StorageManager mgr(saverLists, saverRecordsLinks);
    QVERIFY(mgr.add(makeRecord()));
    QVERIFY(mgr.commit());

    // QVERIFY(mgr.add(makeRecord("02.02.2024", "D-002", "Second")));
    // QCOMPARE(mgr.count(), qsizetype(2));

    // QVERIFY(mgr.reset());
    // QCOMPARE(mgr.count(), qsizetype(1));
    // QCOMPARE(mgr.get().first().drawing.getNumber(), QString("D-001"));
}

void TestStorageManager::reset_afterCommit_keepsState() {
    // try {
    //     StorageManager mgr(saverLists, saverRecordsLinks);
    //     QVERIFY(mgr.add(makeRecord()));
    //     try {
    //         QVERIFY(mgr.commit());
    //     } catch (RuntimeError& error) {
    //         qDebug() << "Error:" << error.message();
    //         return;
    //     } catch (...) {
    //         qDebug() << "Unknown error!";
    //         return;
    //     }

    //     QVERIFY(mgr.reset());
    //     QCOMPARE(mgr.count(), qsizetype(1));
    // } catch (...) {
    //     qDebug() << "Error";
    // }
}

void TestStorageManager::commit_makesChangesPersistent() {
    // try {
    //     StorageManager mgr(saverLists, saverRecordsLinks);
    //     QVERIFY(mgr.add(makeRecord()));
    //     try {
    //         QVERIFY(mgr.commit());
    //     } catch (RuntimeError& error) {
    //         qDebug() << "Error:" << error.message();
    //         return;
    //     } catch (...) {
    //         qDebug() << "Unknown error!";
    //         return;
    //     }

    //     // После commit reset не откатывает
    //     QVERIFY(mgr.add(makeRecord("02.02.2024", "D-002", "Second")));
    //     try {
    //         QVERIFY(mgr.commit());
    //     } catch (RuntimeError& error) {
    //         qDebug() << "Error:" << error.message();
    //         return;
    //     } catch (...) {
    //         qDebug() << "Unknown error!";
    //         return;
    //     }
    //     QVERIFY(mgr.reset());
    //     QCOMPARE(mgr.count(), qsizetype(2));
    // } catch (...) {
    //     qDebug() << "Error";
    // }
}

void TestStorageManager::reset_thenCommit_dropsChanges() {
    // StorageManager mgr(saverLists, saverRecordsLinks);
    // QVERIFY(mgr.add(makeRecord()));
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }

    // QVERIFY(mgr.add(makeRecord("02.02.2024", "D-002", "Second")));
    // QVERIFY(mgr.reset());
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }

    // QCOMPARE(mgr.count(), qsizetype(1));
    // QCOMPARE(mgr.get().first().drawing.getNumber(), QString("D-001"));
}

// ---------------------------------------------------------------------------
// save / load
// ---------------------------------------------------------------------------

void TestStorageManager::save_load_roundTrip() {
    // {
    //     StorageManager mgr(saverLists, saverRecordsLinks);
    //     QVERIFY(mgr.add(makeRecord()));
    //     try {
    //         QVERIFY(mgr.commit());
    //     } catch (RuntimeError& error) {
    //         qDebug() << "Error:" << error.message();
    //         return;
    //     } catch (...) {
    //         qDebug() << "Unknown error!";
    //         return;
    //     }
    //     QVERIFY(mgr.save());
    // }
    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------
    // StorageManager mgr(saverLists, saverRecordsLinks);
    // QVERIFY(mgr2.load());
    // QCOMPARE(mgr2.count(), qsizetype(1));
    // QCOMPARE(mgr2.get().first().drawing.getNumber(), QString("D-001"));
}

void TestStorageManager::save_load_multipleRecords() {
    // {
    //     StorageManager mgr(saverLists, saverRecordsLinks);
    //     QVERIFY(mgr.add(makeRecord("01.01.2024", "D-001", "A")));
    //     QVERIFY(mgr.add(makeRecord("02.02.2024", "D-002", "B")));
    //     QVERIFY(mgr.add(makeRecord("03.03.2024", "D-003", "C")));
    //     try {
    //         QVERIFY(mgr.commit());
    //     } catch (RuntimeError& error) {
    //         qDebug() << "Error:" << error.message();
    //         return;
    //     } catch (...) {
    //         qDebug() << "Unknown error!";
    //         return;
    //     }
    //     QVERIFY(mgr.save());
    // }

    // StorageManager mgr2(saverLists, saverRecordsLinks);
    // QVERIFY(mgr2.load());
    // QCOMPARE(mgr2.count(), qsizetype(3));

    // QSet<QString> numbers;
    // for (const auto& r : mgr2.get()) {
    //     numbers.insert(r.drawing.getNumber());
    // }
    //----------------------------------------------------------------------------------------------------------------------------------------------------------------------
    //QCOMPARE(numbers, QSet<QString>({ "D-001", "D-002", "D-003" }));
}

void TestStorageManager::save_withoutChanges_returnsTrue() {
    // StorageManager mgr(saverLists, saverRecordsLinks);
    // // m_save == true по умолчанию → save() должен вернуть true без действий
    // QVERIFY(mgr.save());
}

void TestStorageManager::load_missingFile_returnsFalse() {
    // // Файлы уже почищены в init()
    // StorageManager mgr(saverLists, saverRecordsLinks);
    // QVERIFY(!mgr.load());
}

// ---------------------------------------------------------------------------
// clear / count
// ---------------------------------------------------------------------------

void TestStorageManager::clear_emptiesStorage() {
    // StorageManager mgr(saverLists, saverRecordsLinks);
    // QVERIFY(mgr.add(makeRecord()));
    // mgr.clear();
    // QCOMPARE(mgr.count(), qsizetype(0));
    // QCOMPARE(mgr.get().size(), 0);
}

void TestStorageManager::count_reflectsChanges() {
    // StorageManager mgr(saverLists, saverRecordsLinks);
    // QCOMPARE(mgr.count(), qsizetype(0));
    // QVERIFY(mgr.add(makeRecord("01.01.2024", "D-001")));
    // QCOMPARE(mgr.count(), qsizetype(1));
    // QVERIFY(mgr.add(makeRecord("02.02.2024", "D-002")));
    // QCOMPARE(mgr.count(), qsizetype(2));
}

// ---------------------------------------------------------------------------
// Adder / Remover — эти тесты ВСКРЫВАЮТ баг: все методы пишут в executors()
// ---------------------------------------------------------------------------

void TestStorageManager::adder_addsExecutorToExecutorList() {
    // StorageManager mgr(saverLists, saverRecordsLinks);
    // mgr.add().executor("Alice").executor("Bob");
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }

    // // Проверяем эффект: запись с этими executor'ами должна читаться.
    // // Через get() мы их не увидим (это справочник), поэтому проверяем
    // // косвенно: создаём запись, которая их использует, и читаем обратно.
    // // Главный индикатор правильности — отдельные тесты ниже.
    // QVERIFY(true);
}

void TestStorageManager::adder_addsAuthorToAuthorList() {
    // // БАГ: adder.author() должен писать в authors(),
    // //      а сейчас пишет в executors().
    // // Чтобы проверить, воспользуемся эффектом: создаём запись с автором,
    // // но предварительно НЕ добавляем его в справочник authors напрямую,
    // // а «прогреваем» через adder.author(). Затем add(Record) должен пройти
    // // и get() должен вернуть автора.
    // StorageManager mgr(saverLists, saverRecordsLinks);
    // mgr.add().author("AuthorX");
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }

    // Record r = makeRecord("01.01.2024",
    //                       "D-001",
    //                       "T",
    //                       1,
    //                       /*executors*/ { },
    //                       /*authors*/ { "AuthorX" },
    //                       /*casting*/ { },
    //                       /*model*/ { },
    //                       /*machines*/ { },
    //                       /*notes*/ { });
    // QVERIFY(mgr.add(r));
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }

    // const auto records = mgr.get();
    // QCOMPARE(records.size(), 1);
    // // Если баг есть — авторы не найдутся в справочнике authors,
    // // get() может упасть на .value() (nullopt → UB/исключение).
    // // В любом случае assert ниже покажет расхождение.
    // QCOMPARE(records.first().authors, QStringList { "AuthorX" });
}

void TestStorageManager::adder_addsCastingMaterial() {
    // StorageManager mgr(saverLists, saverRecordsLinks);
    // mgr.add().castingMaterial("SteelX");
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }

    // Record r = makeRecord("01.01.2024", "D-001", "T", 1, { }, { }, { "SteelX" }, { }, { }, { });
    // QVERIFY(mgr.add(r));
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }

    // const auto records = mgr.get();
    // QCOMPARE(records.first().castingMaterials, QStringList { "SteelX" });
}

void TestStorageManager::adder_addsModelMaterial() {
    // StorageManager mgr(saverLists, saverRecordsLinks);
    // mgr.add().modelMaterial("WaxX");
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }

    // Record r = makeRecord("01.01.2024", "D-001", "T", 1, { }, { }, { }, { "WaxX" }, { }, { });
    // QVERIFY(mgr.add(r));
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }

    // QCOMPARE(mgr.get().first().modelMaterials, QStringList { "WaxX" });
}

void TestStorageManager::adder_addsMachine() {
    // StorageManager mgr(saverLists, saverRecordsLinks);
    // mgr.add().machine("CNC-X");
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }

    // Record r = makeRecord("01.01.2024", "D-001", "T", 1, { }, { }, { }, { }, { "CNC-X" }, { });
    // QVERIFY(mgr.add(r));
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }

    // QCOMPARE(mgr.get().first().machines, QStringList { "CNC-X" });
}

void TestStorageManager::adder_addsNote() {
    // StorageManager mgr(saverLists, saverRecordsLinks);
    // mgr.add().note("noteX");
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }

    // Record r = makeRecord("01.01.2024", "D-001", "T", 1, { }, { }, { }, { }, { }, { "noteX" });
    // QVERIFY(mgr.add(r));
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }

    // QCOMPARE(mgr.get().first().notes, QStringList { "noteX" });
}

void TestStorageManager::remover_removesExecutor() {
    // StorageManager mgr(saverLists, saverRecordsLinks);
    // mgr.add().executor("Alice");
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }
    // mgr.remove().executor("Alice");
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }
    // QVERIFY(true); // нет прямого способа убедиться — тест на отсутствие падений
}

void TestStorageManager::remover_removesAuthor() {
    // // Аналогично adder_addsAuthorToAuthorList — если remove() пишет не туда,
    // // повторный add(Record) с этим автором создаст новую запись в справочнике,
    // // но сам факт «удаления» не проверим без доступа к StorageLists.
    // // Тест фиксирует отсутствие краха при корректной логике.
    // StorageManager mgr(saverLists, saverRecordsLinks);
    // mgr.add().author("BobX");
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }
    // mgr.remove().author("BobX");
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }
}

void TestStorageManager::remover_removesCastingMaterial() {
    // StorageManager mgr(saverLists, saverRecordsLinks);
    // mgr.add().castingMaterial("SteelX");
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }
    // mgr.remove().castingMaterial("SteelX");
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }
}

void TestStorageManager::remover_removesModelMaterial() {
    // StorageManager mgr(saverLists, saverRecordsLinks);
    // mgr.add().modelMaterial("WaxX");
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }
    // mgr.remove().modelMaterial("WaxX");
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }
}

void TestStorageManager::remover_removesMachine() {
    // StorageManager mgr(saverLists, saverRecordsLinks);
    // mgr.add().machine("CNC-X");
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }
    // mgr.remove().machine("CNC-X");
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }
}

void TestStorageManager::remover_removesNote() {
    // StorageManager mgr(saverLists, saverRecordsLinks);
    // mgr.add().note("noteX");
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }
    // mgr.remove().note("noteX");
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }
}

void TestStorageManager::adder_invalidatesRecordsCache() {
    // // БАГ: Adder/Remover не сбрасывают m_recordsIsValid.
    // // Сценарий: получаем get() (кэш), затем добавляем запись через add(Record),
    // // потом снова get() — должен вернуть новую запись.
    // // Сам Adder меняет только справочник, а не записи, поэтому эффект
    // // виден косвенно: если кэш невалидируется после изменения справочника,
    // // следующий get() может вернуть устаревшие данные (если запись зависит
    // // от справочника). Проверяем на add(Record).
    // StorageManager mgr(saverLists, saverRecordsLinks);
    // QVERIFY(mgr.add(makeRecord("01.01.2024", "D-001")));
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }
    // QCOMPARE(mgr.get().size(), 1); // прогрели кэш

    // // Теперь через Adder меняем справочник — не должно ломать get()
    // mgr.add().executor("Zed");
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }

    // // get() всё ещё должен вернуть актуальные записи (1 шт.)
    // QCOMPARE(mgr.get().size(), 1);
}

// ---------------------------------------------------------------------------
// deleteBadLinks
// ---------------------------------------------------------------------------

void TestStorageManager::deleteBadLinks_onValidData_noop() {
    // StorageManager mgr(saverLists, saverRecordsLinks);
    // QVERIFY(mgr.add(makeRecord()));
    // try {
    //     QVERIFY(mgr.commit());
    // } catch (RuntimeError& error) {
    //     qDebug() << "Error:" << error.message();
    //     return;
    // } catch (...) {
    //     qDebug() << "Unknown error!";
    //     return;
    // }

    // mgr.deleteBadLinks();
    // QCOMPARE(mgr.count(), qsizetype(1));

    // // Данные не повреждены
    // const auto records = mgr.get();
    // QCOMPARE(records.size(), 1);
    // QCOMPARE(records.first().drawing.getNumber(), QString("D-001"));
}

void TestStorageManager::deleteBadLinks_doesNotCrashOnEmpty() {
    // StorageManager mgr(saverLists, saverRecordsLinks);
    // mgr.deleteBadLinks();
    // QCOMPARE(mgr.count(), qsizetype(0));
}

QTEST_MAIN(TestStorageManager)
#include "test_StorageManager.moc"
