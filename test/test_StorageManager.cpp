#include <QByteArray>
#include <QDataStream>
#include <QtTest>

#include "Drawing.h"
#include "Record.h"
#include "StorageManager.h"

class TestStorageManager : public QObject {
    Q_OBJECT

private slots:
    void testEmptyOnCreate();
    void testAddRecord();
    void testAddInvalidRecord();
    void testAddDuplicateValuesInList();
    void testRemoveRecord();
    void testRemoveNonexistentRecord();

    void testAdderFluent();
    void testRemoverFluent();
    void testGetter();
    void testCounter();

    void testDeleteBadLinksOnRemoverDate();
    void testDeleteBadLinksOnRemoverExecutor();
    void testDeleteBadLinksKeepsRecordIfOnlySomeValuesRemoved();

    void testRecordsCacheInvalidation();

    void testSerializeDeserialize();

    void testReset();
    void testClear();

private:
    static Drawing makeValidDrawing(int n = 1);
    static Record makeValidRecord(const QString& date = QStringLiteral("15.01.2024"),
                                  qint32 amount = 1,
                                  const QStringList& executors = { },
                                  const QStringList& authors = { },
                                  const QStringList& castingMaterials = { },
                                  const QStringList& modelMaterials = { },
                                  const QStringList& machines = { },
                                  const QStringList& notes = { });
};

Drawing TestStorageManager::makeValidDrawing(int n) {
    return Drawing(QString::number(n), QStringLiteral("Title %1").arg(n));
}

Record TestStorageManager::makeValidRecord(const QString& date,
                                           qint32 amount,
                                           const QStringList& executors,
                                           const QStringList& authors,
                                           const QStringList& castingMaterials,
                                           const QStringList& modelMaterials,
                                           const QStringList& machines,
                                           const QStringList& notes) {
    return Record(date,
                  makeValidDrawing(1),
                  amount,
                  executors,
                  authors,
                  castingMaterials,
                  modelMaterials,
                  machines,
                  notes);
}

// ---------------------------------------------------------------------------
// Базовые операции
// ---------------------------------------------------------------------------

void TestStorageManager::testEmptyOnCreate() {
    StorageManager sm;
    QCOMPARE(sm.countRecords(), qsizetype(0));
    QVERIFY(sm.getRecords().isEmpty());
}

void TestStorageManager::testAddRecord() {
    StorageManager sm;
    const Record r = makeValidRecord(QStringLiteral("15.01.2024"),
                                     5,
                                     { QStringLiteral("Ivanov") },
                                     { QStringLiteral("Petrov") });

    QVERIFY(sm.addRecord(r));
    QCOMPARE(sm.countRecords(), qsizetype(1));

    const QList<Record> records = sm.getRecords();
    QCOMPARE(records.size(), 1);
    QCOMPARE(records.first(), r);
}

void TestStorageManager::testAddInvalidRecord() {
    StorageManager sm;

    // Пустая / невалидная дата
    const Record badDate = makeValidRecord(QString(), 1);
    QVERIFY(!sm.addRecord(badDate));

    // Невалидный drawing (Drawing::Null)
    const Record badDrawing(QStringLiteral("15.01.2024"), Drawing::Null, 1);
    QVERIFY(!sm.addRecord(badDrawing));

    // amount < 1
    const Record badAmount = makeValidRecord(QStringLiteral("15.01.2024"), 0);
    QVERIFY(!sm.addRecord(badAmount));

    QCOMPARE(sm.countRecords(), qsizetype(0));
}

void TestStorageManager::testAddDuplicateValuesInList() {
    // ServiceCustomList не хранит дубликаты, поэтому
    // добавление одного и того же значения несколько раз не увеличивает count.
    StorageManager sm;

    sm.add().executor(QStringLiteral("Ivanov"));
    sm.add().executor(QStringLiteral("Ivanov"));
    sm.add().executor(QStringLiteral("Ivanov"));

    QCOMPARE(sm.count().executors(), 1);
    QCOMPARE(sm.get().executors().size(), 1);
    QCOMPARE(sm.get().executors().first(), QStringLiteral("Ivanov"));
}

void TestStorageManager::testRemoveRecord() {
    StorageManager sm;
    const Record r = makeValidRecord(QStringLiteral("20.02.2024"), 3, { QStringLiteral("Sidorov") });

    QVERIFY(sm.addRecord(r));
    QCOMPARE(sm.countRecords(), qsizetype(1));

    QVERIFY(sm.removeRecord(r));
    QCOMPARE(sm.countRecords(), qsizetype(0));
    QVERIFY(sm.getRecords().isEmpty());
}

void TestStorageManager::testRemoveNonexistentRecord() {
    StorageManager sm;
    const Record r = makeValidRecord(QStringLiteral("20.02.2024"), 3);

    // Ничего не добавляли — removeRecord должен вернуть false
    QVERIFY(!sm.removeRecord(r));
    QCOMPARE(sm.countRecords(), qsizetype(0));
}

// ---------------------------------------------------------------------------
// Adder / Remover / Getter / Counter
// ---------------------------------------------------------------------------

void TestStorageManager::testAdderFluent() {
    StorageManager sm;

    sm.add()
        .date(QStringLiteral("01.03.2024"))
        .drawing(makeValidDrawing(2))
        .amount(10)
        .executor(QStringLiteral("ExecA"))
        .author(QStringLiteral("AuthorA"))
        .castingMaterial(QStringLiteral("CastA"))
        .modelMaterial(QStringLiteral("ModelA"))
        .machine(QStringLiteral("MachineA"))
        .note(QStringLiteral("NoteA"));

    QCOMPARE(sm.count().dates(), 1);
    QCOMPARE(sm.count().amounts(), 1);
    QCOMPARE(sm.count().executors(), 1);
    QCOMPARE(sm.count().authors(), 1);
    QCOMPARE(sm.count().castingMaterials(), 1);
    QCOMPARE(sm.count().modelMaterials(), 1);
    QCOMPARE(sm.count().machines(), 1);
    QCOMPARE(sm.count().notes(), 1);

    // Проверяем, что значения реально попали в списки
    QCOMPARE(sm.get().datesStr().first(), QStringLiteral("01.03.2024"));
    QCOMPARE(sm.get().amounts().first(), 10);
    QCOMPARE(sm.get().executors().first(), QStringLiteral("ExecA"));
}

void TestStorageManager::testRemoverFluent() {
    StorageManager sm;
    sm.add().date(QStringLiteral("02.03.2024")).amount(1).executor(QStringLiteral("ExecB")).note(QStringLiteral("NoteB"));

    QCOMPARE(sm.count().executors(), 1);
    QCOMPARE(sm.count().notes(), 1);

    sm.remove().executor(QStringLiteral("ExecB")).note(QStringLiteral("NoteB"));

    QCOMPARE(sm.count().executors(), 0);
    QCOMPARE(sm.count().notes(), 0);
}

void TestStorageManager::testGetter() {
    StorageManager sm;
    sm.add().date(QStringLiteral("01.04.2024")).amount(7).executor(QStringLiteral("ExecC")).author(QStringLiteral("AuthorC"));

    const auto dates = sm.get().datesStr();
    QCOMPARE(dates.size(), 1);
    QCOMPARE(dates.first(), QStringLiteral("01.04.2024"));

    const auto amounts = sm.get().amounts();
    QCOMPARE(amounts.size(), 1);
    QCOMPARE(amounts.first(), 7);

    const auto execs = sm.get().executors();
    QCOMPARE(execs.size(), 1);
    QCOMPARE(execs.first(), QStringLiteral("ExecC"));

    const auto authors = sm.get().authors();
    QCOMPARE(authors.size(), 1);
    QCOMPARE(authors.first(), QStringLiteral("AuthorC"));
}

void TestStorageManager::testCounter() {
    StorageManager sm;

    // Дубликаты не хранятся — счётчик по уникальным значениям
    sm.add().date(QStringLiteral("01.05.2024"));
    sm.add().date(QStringLiteral("02.05.2024"));
    sm.add().date(QStringLiteral("01.05.2024")); // дубликат
    sm.add().date(QStringLiteral("01.05.2024")); // дубликат

    QCOMPARE(sm.count().dates(), 2);
}

// ---------------------------------------------------------------------------
// Каскадное удаление "плохих" ссылок
// ---------------------------------------------------------------------------

void TestStorageManager::testDeleteBadLinksOnRemoverDate() {
    StorageManager sm;
    const Record r = makeValidRecord(QStringLiteral("01.06.2024"), 4, { QStringLiteral("ExecD") });
    QVERIFY(sm.addRecord(r));
    QCOMPARE(sm.countRecords(), qsizetype(1));

    // Удаляем дату через Remover — запись станет "плохой" и должна исчезнуть
    sm.remove().date(QStringLiteral("01.06.2024"));
    QCOMPARE(sm.countRecords(), qsizetype(0));
    QVERIFY(sm.getRecords().isEmpty());
}

void TestStorageManager::testDeleteBadLinksOnRemoverExecutor() {
    StorageManager sm;
    const Record r = makeValidRecord(QStringLiteral("01.07.2024"),
                                     2,
                                     { QStringLiteral("ExecE1"), QStringLiteral("ExecE2") });
    QVERIFY(sm.addRecord(r));

    sm.remove().executor(QStringLiteral("ExecE1"));

    // Запись должна остаться, но без ExecE1
    QCOMPARE(sm.countRecords(), qsizetype(1));
    const QList<Record> records = sm.getRecords();
    QCOMPARE(records.size(), 1);
    QCOMPARE(records.first().executors, QStringList { QStringLiteral("ExecE2") });
}

void TestStorageManager::testDeleteBadLinksKeepsRecordIfOnlySomeValuesRemoved() {
    StorageManager sm;
    const Record r = makeValidRecord(QStringLiteral("15.07.2024"),
                                     1,
                                     { QStringLiteral("ExecX") },
                                     { QStringLiteral("AuthorX") },
                                     { QStringLiteral("CastX") },
                                     { QStringLiteral("ModelX") },
                                     { QStringLiteral("MachineX") },
                                     { QStringLiteral("NoteX") });
    QVERIFY(sm.addRecord(r));

    // Удаляем несколько значений из разных списков
    sm.remove().executor(QStringLiteral("ExecX")).castingMaterial(QStringLiteral("CastX")).machine(QStringLiteral("MachineX"));

    // Запись осталась, но соответствующие списки опустели
    QCOMPARE(sm.countRecords(), qsizetype(1));
    const Record got = sm.getRecords().first();
    QVERIFY(got.executors.isEmpty());
    QVERIFY(got.castingMaterials.isEmpty());
    QVERIFY(got.machines.isEmpty());

    // Остальные значения сохранились
    QCOMPARE(got.authors, QStringList { QStringLiteral("AuthorX") });
    QCOMPARE(got.modelMaterials, QStringList { QStringLiteral("ModelX") });
    QCOMPARE(got.notes, QStringList { QStringLiteral("NoteX") });
}

// ---------------------------------------------------------------------------
// Кэш записей
// ---------------------------------------------------------------------------

void TestStorageManager::testRecordsCacheInvalidation() {
    StorageManager sm;
    QVERIFY(sm.getRecords().isEmpty()); // прогреваем кэш

    const Record r = makeValidRecord(QStringLiteral("01.08.2024"), 1);
    QVERIFY(sm.addRecord(r));

    // После addRecord кэш сброшен — getRecords вернёт новую запись
    QCOMPARE(sm.getRecords().size(), 1);
    QCOMPARE(sm.getRecords().first(), r);

    QVERIFY(sm.removeRecord(r));
    QCOMPARE(sm.getRecords().size(), 0);
}

// ---------------------------------------------------------------------------
// Сериализация
// ---------------------------------------------------------------------------

void TestStorageManager::testSerializeDeserialize() {
    StorageManager src;
    QVERIFY(src.addRecord(makeValidRecord(QStringLiteral("01.09.2024"),
                                          11,
                                          { QStringLiteral("ExecF") },
                                          { QStringLiteral("AuthorF") })));
    QVERIFY(src.addRecord(
        makeValidRecord(QStringLiteral("02.09.2024"), 22, { QStringLiteral("ExecG") })));

    QByteArray data;
    {
        QDataStream out(&data, QIODevice::WriteOnly);
        src.serialize(out);
    }

    StorageManager dst;
    {
        QDataStream in(&data, QIODevice::ReadOnly);
        dst.deserialize(in);
    }

    QCOMPARE(dst.countRecords(), src.countRecords());
    QCOMPARE(dst.getRecords().size(), src.getRecords().size());
    QCOMPARE(dst.getRecords(), src.getRecords());
}

// ---------------------------------------------------------------------------
// reset / clear
// ---------------------------------------------------------------------------

void TestStorageManager::testReset() {
    StorageManager sm;
    QVERIFY(sm.addRecord(makeValidRecord(QStringLiteral("01.10.2024"), 1)));
    QCOMPARE(sm.countRecords(), qsizetype(1));

    sm.reset();

    QCOMPARE(sm.countRecords(), qsizetype(0));
    QVERIFY(sm.getRecords().isEmpty());
    QCOMPARE(sm.count().dates(), 0);
    QCOMPARE(sm.count().amounts(), 0);
}

void TestStorageManager::testClear() {
    StorageManager sm;
    QVERIFY(sm.addRecord(makeValidRecord(QStringLiteral("01.11.2024"), 1)));
    QCOMPARE(sm.countRecords(), qsizetype(1));

    sm.clear();

    QCOMPARE(sm.countRecords(), qsizetype(0));
    QVERIFY(sm.getRecords().isEmpty());
    QCOMPARE(sm.count().dates(), 0);
}

QTEST_APPLESS_MAIN(TestStorageManager)
#include "test_StorageManager.moc"
