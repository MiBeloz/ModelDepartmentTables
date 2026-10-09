#include <QAtomicInt>
#include <QByteArray>
#include <QDataStream>
#include <QFuture>
#include <QThread>
#include <QtConcurrent>
#include <QtTest>

#include "Drawing.h"
#include "Record.h"
#include "StorageManager.h"
#include "TestStats.h"

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

    // ---------- Multithreading ----------
    // ---------- TEST 1: 8 threads × 1000 addRecord operations, verification against the reference ----------
    void testMultithreadedAdd() {
        TEST_SCOPE("testMultithreadedAdd");

        const int threadCount = 8;
        const int recordsPerThread = 500;
        const int totalRecords = threadCount * recordsPerThread;

        StorageManager storageManager;
        QAtomicInt successCount;
        QAtomicInt failCount;

        QThreadPool::globalInstance()->setMaxThreadCount(16);

        // Заранее подготовим ожидаемые ключи (эталон)
        QSet<QString> expectedKeys;
        for (int t = 0; t < threadCount; ++t) {
            for (int i = 0; i < recordsPerThread; ++i) {
                expectedKeys.insert(makeKey(createRecord(t, i)));
            }
        }
        QCOMPARE(expectedKeys.size(), totalRecords);

        QElapsedTimer timer;
        timer.start();

        // Запускаем 8 потоков одновременно
        QList<QFuture<void>> futures;
        futures.reserve(threadCount);

        for (int t = 0; t < threadCount; ++t) {
            futures.append(QtConcurrent::run([&, t]() {
                for (int i = 0; i < recordsPerThread; ++i) {
                    Record record = createRecord(t, i);
                    if (storageManager.addRecord(record)) {
                        successCount.fetchAndAddOrdered(1);
                    } else {
                        failCount.fetchAndAddOrdered(1);
                    }
                }
            }));
        }

        for (auto& f : futures) {
            f.waitForFinished();
        }

        const qint64 elapsed = timer.elapsed();

        qDebug() << "Elapsed:" << elapsed << "ms";
        qDebug() << "Success:" << successCount.loadAcquire() << "Fail:" << failCount.loadAcquire();

        _scope.metric("threadCount", threadCount)
            .metric("recordsPerThread", recordsPerThread)
            .metric("totalRecords", totalRecords)
            .metric("addOk", successCount.loadAcquire())
            .metric("addFail", failCount.loadAcquire())
            .metric("elapsedMs", elapsed)
            .metric("finalRecords", storageManager.countRecords());

        // === Проверки ===

        // 1) Все добавления успешны
        QCOMPARE(failCount.loadAcquire(), 0);
        QCOMPARE(successCount.loadAcquire(), totalRecords);

        // 2) Количество записей в хранилище совпадает
        QCOMPARE(storageManager.countRecords(), static_cast<qsizetype>(totalRecords));

        // 3) Получаем все записи и сверяем с эталоном
        QList<Record> records = storageManager.getRecords();
        QCOMPARE(records.size(), totalRecords);

        QSet<QString> actualKeys;
        for (const Record& record : records) {
            QVERIFY(record.isValid());

            actualKeys.insert(makeKey(record));
        }

        // Все ключи уникальны
        QCOMPARE(actualKeys.size(), totalRecords);

        // Множества совпадают: ни одна запись не потеряна и не искажена
        QCOMPARE(actualKeys, expectedKeys);
    }

    //  ---------- TEST 2 : 8 addRecord threads, then 8 removeRecord threads ----------
    void testMultithreadedAddAndRemove() {
        TEST_SCOPE("testMultithreadedAddAndRemove");

        const int threadCount = 8;
        const int recordsPerThread = 500;
        const int totalRecords = threadCount * recordsPerThread;

        StorageManager sm;
        QAtomicInt addSuccess;
        QAtomicInt removeSuccess;

        QThreadPool::globalInstance()->setMaxThreadCount(16);

        QElapsedTimer timer;
        timer.start();

        QList<QFuture<void>> futures;
        // ---------- Фаза 1: добавляем ----------
        for (int t = 0; t < threadCount; ++t) {
            futures.append(QtConcurrent::run([&, t]() {
                for (int i = 0; i < recordsPerThread; ++i) {
                    if (sm.addRecord(createRecord(t, i))) {
                        addSuccess.fetchAndAddOrdered(1);
                    }
                }
            }));
        }
        for (auto& f : futures) {
            f.waitForFinished();
        }
        futures.clear();

        QCOMPARE(addSuccess.loadAcquire(), totalRecords);
        QCOMPARE(sm.countRecords(), static_cast<qsizetype>(totalRecords));

        // ---------- Фаза 2: удаляем ----------
        for (int t = 0; t < threadCount; ++t) {
            futures.append(QtConcurrent::run([&, t]() {
                for (int i = 0; i < recordsPerThread; ++i) {
                    if (sm.removeRecord(createRecord(t, i))) {
                        removeSuccess.fetchAndAddOrdered(1);
                    }
                }
            }));
        }
        for (auto& f : futures) {
            f.waitForFinished();
        }

        const qint64 elapsed = timer.elapsed();

        qDebug() << "Elapsed:" << elapsed << "ms";
        _scope.metric("threadCount", threadCount)
            .metric("recordsPerThread", recordsPerThread)
            .metric("totalRecords", totalRecords)
            .metric("addOk", addSuccess.loadAcquire())
            .metric("removeOk", removeSuccess.loadAcquire())
            .metric("elapsedMs", elapsed)
            .metric("finalRecords", sm.countRecords());

        QCOMPARE(removeSuccess.loadAcquire(), totalRecords);
        QCOMPARE(sm.countRecords(), static_cast<qsizetype>(0));
        QCOMPARE(sm.getRecords().size(), 0);
    }

    // ---------- TEST 3: read during write ----------
    void testConcurrentGetDuringAdd() {
        TEST_SCOPE("testConcurrentGetDuringAdd");

        const int threadCount = 8;
        const int recordsPerThread = 500;
        const int totalRecords = threadCount * recordsPerThread;

        StorageManager sm;
        QAtomicInt stopFlag;
        QAtomicInt addSuccess;
        QAtomicInt getterIterations;

        QThreadPool::globalInstance()->setMaxThreadCount(16);

        QElapsedTimer timer;
        timer.start();

        // Thread-reader
        QFuture<void> getter = QtConcurrent::run([&]() {
            while (!stopFlag.loadAcquire()) {
                const auto records = sm.getRecords();
                Q_UNUSED(records);
                getterIterations.fetchAndAddOrdered(1);
            }
        });

        // Threads-writes
        QList<QFuture<void>> futures;
        for (int t = 0; t < threadCount; ++t) {
            futures.append(QtConcurrent::run([&, t]() {
                for (int i = 0; i < recordsPerThread; ++i) {
                    if (sm.addRecord(createRecord(t, i))) {
                        addSuccess.fetchAndAddOrdered(1);
                    }
                }
            }));
        }
        for (auto& f : futures) {
            f.waitForFinished();
        }

        stopFlag.storeRelease(1);
        getter.waitForFinished();

        const qint64 elapsed = timer.elapsed();

        qDebug() << "Elapsed:" << elapsed << "ms";
        _scope.metric("threadCount", threadCount)
            .metric("recordsPerThread", recordsPerThread)
            .metric("totalRecords", totalRecords)
            .metric("addOk", addSuccess.loadAcquire())
            .metric("getterIter", getterIterations.loadAcquire())
            .metric("elapsedMs", elapsed)
            .metric("finalRecords", sm.countRecords());

        QCOMPARE(sm.countRecords(), static_cast<qsizetype>(totalRecords));
        QVERIFY(getterIterations.loadAcquire() > 0);
    }

    // ---------- TEST 4: 8 threads × 1000 Adders (without RecordLink) ----------
    void testMultithreadedAdder() {
        TEST_SCOPE("testMultithreadedAdder");

        const int threadCount = 8;
        const int addsPerThread = 500;
        const int total = threadCount * addsPerThread;

        StorageManager sm;
        QAtomicInt success;

        QThreadPool::globalInstance()->setMaxThreadCount(16);

        QElapsedTimer timer;
        timer.start();

        QList<QFuture<void>> futures;
        for (int t = 0; t < threadCount; ++t) {
            futures.append(QtConcurrent::run([&, t]() {
                for (int i = 0; i < addsPerThread; ++i) {
                    sm.add()
                        .date(makeDate(t, i))
                        .drawing(makeDrawing(t, i))
                        .amount(t * addsPerThread + i + 1)
                        .executor(QString("E_%1_%2").arg(t).arg(i))
                        .author(QString("A_%1_%2").arg(t).arg(i))
                        .castingMaterial(QString("C_%1_%2").arg(t).arg(i))
                        .modelMaterial(QString("M_%1_%2").arg(t).arg(i))
                        .machine(QString("Mc_%1_%2").arg(t).arg(i))
                        .note(QString("N_%1_%2").arg(t).arg(i));
                    success.fetchAndAddOrdered(1);
                }
            }));
        }
        for (auto& f : futures) {
            f.waitForFinished();
        }

        const qint64 elapsed = timer.elapsed();

        qDebug() << "Elapsed:" << elapsed << "ms";
        _scope.metric("threadCount", threadCount)
            .metric("total", total)
            .metric("adderOk", success.loadAcquire())
            .metric("elapsedMs", elapsed)
            .metric("finalRecords", sm.countRecords());

        QCOMPARE(sm.countRecords(), static_cast<qsizetype>(0));

        QCOMPARE(sm.count().dates(), total);
        QCOMPARE(sm.count().drawings(), total);
        QCOMPARE(sm.count().amounts(), total);
        QCOMPARE(sm.count().executors(), total);
        QCOMPARE(sm.count().authors(), total);
        QCOMPARE(sm.count().castingMaterials(), total);
        QCOMPARE(sm.count().modelMaterials(), total);
        QCOMPARE(sm.count().machines(), total);
        QCOMPARE(sm.count().notes(), total);

        QCOMPARE(success.loadAcquire(), total);
    }

    // ---------- TEST 5: 8 threads × 1000 Remover (with deleteBadLinks) ----------
    void testMultithreadedRemover() {
        TEST_SCOPE("testMultithreadedRemover");

        const int threadCount = 8;
        const int recordsPerThread = 300;
        const int totalRecords = threadCount * recordsPerThread;

        StorageManager sm;
        QAtomicInt addSuccess(0);

        QThreadPool::globalInstance()->setMaxThreadCount(16);

        QElapsedTimer timer;
        timer.start();

        // Фаза 1 — добавляем через addRecord (создаются RecordLink)
        for (int t = 0; t < threadCount; ++t) {
            for (int i = 0; i < recordsPerThread; ++i) {
                if (sm.addRecord(createRecord(t, i))) {
                    addSuccess.fetchAndAddOrdered(1);
                }
            }
        }

        QCOMPARE(sm.countRecords(), static_cast<qsizetype>(totalRecords));

        // Фаза 2 — удаляем через Remover в 8 потоков
        QAtomicInt removed(0);
        QList<QFuture<void>> futures;
        for (int t = 0; t < threadCount; ++t) {
            futures.append(QtConcurrent::run([&, t]() {
                for (int i = 0; i < recordsPerThread; ++i) {
                    const Record r = createRecord(t, i);
                    sm.remove().date(r.date).drawing(r.drawing).amount(r.amount);
                    removed.fetchAndAddOrdered(1);
                }
            }));
        }
        for (auto& f : futures) {
            f.waitForFinished();
        }

        const qint64 elapsed = timer.elapsed();

        qDebug() << "Elapsed:" << elapsed << "ms";
        _scope.metric("threadCount", threadCount)
            .metric("recordsPerThread", recordsPerThread)
            .metric("totalRecords", totalRecords)
            .metric("addOk", addSuccess.loadAcquire())
            .metric("removerOk", removed.loadAcquire())
            .metric("elapsedMs", elapsed)
            .metric("finalRecords", sm.countRecords());

        QCOMPARE(removed.loadAcquire(), totalRecords);

        QCOMPARE(sm.countRecords(), static_cast<qsizetype>(0));
    }

    // ---------- TEST 6: mixed load Adder/Remover/Getter/Counter ----------
    void testMixedWorkload() {
        TEST_SCOPE("testMixedWorkload");

        const int threadCount = 8;
        StorageManager sm;
        QAtomicInt stopFlag;
        QAtomicInt adderRemoverIters;
        QAtomicInt getterIters;
        QAtomicInt counterIters;

        QThreadPool::globalInstance()->setMaxThreadCount(16);

        QElapsedTimer timer;
        timer.start();

        // Thread-Getter
        QFuture<void> getter = QtConcurrent::run([&]() {
            while (!stopFlag.loadAcquire()) {
                auto d = sm.get().dates();
                auto dr = sm.get().drawings();
                auto a = sm.get().amounts();
                Q_UNUSED(d);
                Q_UNUSED(dr);
                Q_UNUSED(a);
                getterIters.fetchAndAddOrdered(1);
            }
        });

        // Thread-Counter
        QFuture<void> counter = QtConcurrent::run([&]() {
            while (!stopFlag.loadAcquire()) {
                auto n1 = sm.count().dates();
                auto n2 = sm.count().drawings();
                Q_UNUSED(n1);
                Q_UNUSED(n2);
                counterIters.fetchAndAddOrdered(1);
            }
        });

        // Threads-Adder/Remover
        QList<QFuture<void>> futures;
        for (int t = 0; t < threadCount; ++t) {
            futures.append(QtConcurrent::run([&, t]() {
                for (int i = 0; i < 1000; ++i) {
                    const QString key = QString("E_%1_%2").arg(t).arg(i);
                    if (i % 2 == 0) {
                        sm.add().executor(key);
                    } else {
                        sm.remove().executor(QString("E_%1_%2").arg(t).arg(i - 1));
                    }
                    adderRemoverIters.fetchAndAddOrdered(1);
                }
            }));
        }
        for (auto& f : futures) {
            f.waitForFinished();
        }

        stopFlag.storeRelease(1);
        getter.waitForFinished();
        counter.waitForFinished();

        const qint64 elapsed = timer.elapsed();

        qDebug() << "Elapsed:" << elapsed << "ms";
        _scope.metric("threadCount", threadCount)
            .metric("adderRemoverIters", adderRemoverIters.loadAcquire())
            .metric("getterIters", getterIters.loadAcquire())
            .metric("counterIters", counterIters.loadAcquire())
            .metric("elapsedMs", elapsed)
            .metric("finalRecords", sm.countRecords());

        QVERIFY(getterIters.loadAcquire() > 0);
        QVERIFY(counterIters.loadAcquire() > 0);
    }

    // ---------- TEST 7: reset / clear / commit ----------
    void testResetClearCommit() {
        TEST_SCOPE("testResetClearCommit");

        const int threadCount = 8;
        const int recordsPerThread = 300;
        const int totalRecords = threadCount * recordsPerThread;

        QThreadPool::globalInstance()->setMaxThreadCount(16);

        StorageManager sm;

        // Наполняем
        QList<QFuture<void>> futures;
        for (int t = 0; t < threadCount; ++t) {
            futures.append(QtConcurrent::run([&, t]() {
                for (int i = 0; i < recordsPerThread; ++i) {
                    sm.addRecord(createRecord(t, i));
                }
            }));
        }
        for (auto& f : futures) {
            f.waitForFinished();
        }

        QCOMPARE(sm.countRecords(), static_cast<qsizetype>(totalRecords));
        QCOMPARE(sm.getRecords().size(), static_cast<qsizetype>(totalRecords));

        QElapsedTimer timer;
        timer.start();

        sm.commit();
        QCOMPARE(sm.countRecords(), static_cast<qsizetype>(totalRecords));
        QCOMPARE(sm.getRecords().size(), static_cast<qsizetype>(totalRecords));

        futures.clear();
        for (int t = threadCount; t < threadCount * 2; ++t) {
            futures.append(QtConcurrent::run([&, t]() {
                for (int i = 0; i < recordsPerThread; ++i) {
                    sm.addRecord(createRecord(t, i));
                }
            }));
        }
        for (auto& f : futures) {
            f.waitForFinished();
        }

        const qint64 elapsed = timer.elapsed();

        qDebug() << "Elapsed:" << elapsed << "ms";
        _scope.metric("threadCount", threadCount)
            .metric("recordsPerThread", recordsPerThread)
            .metric("totalRecords", totalRecords)
            .metric("elapsedMs", elapsed)
            .metric("finalRecords", sm.countRecords());

        QCOMPARE(sm.countRecords(), static_cast<qsizetype>(totalRecords * 2));
        QCOMPARE(sm.getRecords().size(), static_cast<qsizetype>(totalRecords * 2));

        sm.reset();
        QCOMPARE(sm.countRecords(), static_cast<qsizetype>(totalRecords));
        QCOMPARE(sm.getRecords().size(), static_cast<qsizetype>(totalRecords));

        sm.clear();
        QCOMPARE(sm.countRecords(), static_cast<qsizetype>(0));
        QCOMPARE(sm.getRecords().size(), static_cast<qsizetype>(0));

        sm.reset();
        QCOMPARE(sm.countRecords(), static_cast<qsizetype>(totalRecords));
        QCOMPARE(sm.getRecords().size(), static_cast<qsizetype>(totalRecords));

        sm.clear();
        sm.commit();
        QCOMPARE(sm.countRecords(), static_cast<qsizetype>(0));
        QCOMPARE(sm.getRecords().size(), static_cast<qsizetype>(0));

        sm.reset();
        QCOMPARE(sm.countRecords(), static_cast<qsizetype>(0));
        QCOMPARE(sm.getRecords().size(), static_cast<qsizetype>(0));
    }

    // ---------- TEST 8: serialize / deserialize in a single thread (round-trip) ----------
    void testSerializeRoundTrip() {
        TEST_SCOPE("testSerializeRoundTrip");

        const int threadCount = 8;
        const int recordsPerThread = 1000;
        const int totalRecords = threadCount * recordsPerThread;

        QThreadPool::globalInstance()->setMaxThreadCount(16);

        StorageManager sm;
        for (int t = 0; t < threadCount; ++t) {
            for (int i = 0; i < recordsPerThread; ++i) {
                sm.addRecord(createRecord(t, i));
            }
        }

        QCOMPARE(sm.countRecords(), static_cast<qsizetype>(totalRecords));

        QElapsedTimer timerSerialize;
        timerSerialize.start();

        // serialize
        QByteArray data;
        {
            QDataStream out(&data, QIODevice::WriteOnly);
            out.setVersion(QDataStream::Version::Qt_6_11);
            sm.serialize(out);
        }

        const qint64 elapsedSerialize = timerSerialize.elapsed();

        QElapsedTimer timerDeserialize;
        timerDeserialize.start();

        // deserialize
        StorageManager sm2;
        {
            QDataStream in(&data, QIODevice::ReadOnly);
            in.setVersion(QDataStream::Version::Qt_6_11);
            sm2.deserialize(in);
        }

        const qint64 elapsedDeserialize = timerDeserialize.elapsed();

        qDebug() << "ElapsedSerialize:" << elapsedSerialize << "ms";
        qDebug() << "ElapsedDeserialize:" << elapsedDeserialize << "ms";
        _scope.metric("elapsedSerialize", elapsedSerialize)
            .metric("elapsedDeserialize", elapsedDeserialize)
            .metric("totalRecords", totalRecords)
            .metric("finalRecords", sm.countRecords());

        QCOMPARE(sm2.countRecords(), static_cast<qsizetype>(totalRecords));

        // Сверяем ключи
        QSet<QString> keys1, keys2;
        for (const Record& r : sm.getRecords()) {
            keys1.insert(makeKey(r));
        }
        for (const Record& r : sm2.getRecords()) {
            keys2.insert(makeKey(r));
        }
        QCOMPARE(keys1, keys2);
    }

    // ---------- TEST 9: serialization in parallel with writing (stress test) ----------
    void testSerializeDuringAdd() {
        TEST_SCOPE("testSerializeDuringAdd");

        const int threadCount = 4;
        const int recordsPerThread = 1000;
        const int totalRecords = threadCount * recordsPerThread;

        QThreadPool::globalInstance()->setMaxThreadCount(16);

        StorageManager sm;
        QAtomicInt stopFlag;
        QAtomicInt serializations;

        QElapsedTimer timer;
        timer.start();

        QFuture<void> serializer = QtConcurrent::run([&]() {
            while (!stopFlag.loadAcquire()) {
                QByteArray data;
                QDataStream out(&data, QIODevice::WriteOnly);
                out.setVersion(QDataStream::Version::Qt_6_11);
                sm.serialize(out);
                serializations.fetchAndAddOrdered(1);
            }
        });

        QList<QFuture<void>> futures;
        for (int t = 0; t < threadCount; ++t) {
            futures.append(QtConcurrent::run([&, t]() {
                for (int i = 0; i < recordsPerThread; ++i) {
                    sm.addRecord(createRecord(t, i));
                }
            }));
        }
        for (auto& f : futures) {
            f.waitForFinished();
        }

        stopFlag.storeRelease(1);
        serializer.waitForFinished();

        const qint64 elapsed = timer.elapsed();

        qDebug() << "Elapsed:" << elapsed << "ms";
        _scope.metric("threadCount", threadCount)
            .metric("recordsPerThread", recordsPerThread)
            .metric("totalRecords", totalRecords)
            .metric("elapsedMs", elapsed)
            .metric("finalRecords", sm.countRecords());

        QCOMPARE(sm.countRecords(), static_cast<qsizetype>(totalRecords));
        QVERIFY(serializations.loadAcquire() > 0);
    }

    // ---------- TEST 10: final stress test — all methods running in parallel, integrity check of each record ----------
    void testFullStressIntegrity() {
        {
            TEST_SCOPE("testFullStressIntegrity - Scenario A");
            // ------------------------------------------------------------
            //  Part 1. Scenario A: addRecord writers only,
            //           deterministic result, full reconciliation.
            // ------------------------------------------------------------
            const int writerCount = 8;
            const int recordsPerWriter = 500;
            const int totalRecords = writerCount * recordsPerWriter;

            StorageManager sm;

            QThreadPool::globalInstance()->setMaxThreadCount(16);

            QElapsedTimer timerA;
            timerA.start();

            // Эталон: уникальные ключи всех записей, которые должны быть добавлены
            QSet<QString> expectedKeys;
            for (int t = 0; t < writerCount; ++t) {
                for (int i = 0; i < recordsPerWriter; ++i) {
                    expectedKeys.insert(makeKey(createRecord(t, i)));
                }
            }
            QCOMPARE(expectedKeys.size(), totalRecords);

            // 8 потоков, каждый — addRecord со своим диапазоном
            QList<QFuture<void>> writers;
            QAtomicInt success(0);
            for (int t = 0; t < writerCount; ++t) {
                writers.append(QtConcurrent::run([&, t]() {
                    for (int i = 0; i < recordsPerWriter; ++i) {
                        if (sm.addRecord(createRecord(t, i))) {
                            success.fetchAndAddOrdered(1);
                        }
                    }
                }));
            }
            for (auto& f : writers) {
                f.waitForFinished();
            }

            const qint64 elapsedA = timerA.elapsed();

            qDebug() << "Elapsed:" << elapsedA << "ms";
            _scope.metric("writerCount", writerCount)
                .metric("recordsPerWriter", recordsPerWriter)
                .metric("totalRecords", totalRecords)
                .metric("elapsedMs", elapsedA)
                .metric("finalRecords", sm.countRecords());

            QCOMPARE(success.loadAcquire(), totalRecords);
            QCOMPARE(sm.countRecords(), static_cast<qsizetype>(totalRecords));

            // ---------- Проверка каждой Record на целостность ----------
            const QList<Record> records = sm.getRecords();
            QCOMPARE(records.size(), totalRecords);

            QSet<QString> actualKeys;
            for (const Record& r : records) {
                verifyRecordIntegrity(r, /*sm=*/sm);
                actualKeys.insert(makeKey(r));
            }
            QCOMPARE(actualKeys.size(), totalRecords);
            QCOMPARE(actualKeys, expectedKeys);

            // ---- Повторный вызов getRecords должен дать тот же результат ----
            const QList<Record> recordsAgain = sm.getRecords();
            QCOMPARE(recordsAgain.size(), records.size());
            for (int k = 0; k < records.size(); ++k) {
                QCOMPARE(makeKey(recordsAgain[k]), makeKey(records[k]));
            }

            // ---- serialize → deserialize даёт тот же набор ----
            QByteArray data;
            {
                QDataStream out(&data, QIODevice::WriteOnly);
                out.setVersion(QDataStream::Version::Qt_6_11);
                sm.serialize(out);
            }
            StorageManager sm2;
            {
                QDataStream in(&data, QIODevice::ReadOnly);
                in.setVersion(QDataStream::Version::Qt_6_11);
                sm2.deserialize(in);
            }
            QCOMPARE(sm2.countRecords(), static_cast<qsizetype>(totalRecords));

            QSet<QString> deserializedKeys;
            for (const Record& r : sm2.getRecords()) {
                deserializedKeys.insert(makeKey(r));
            }
            QCOMPARE(deserializedKeys, expectedKeys);
        }

        // ------------------------------------------------------------
        //  Part 2. Scenario B: all methods in parallel.
        //           The result is non-deterministic, but each Record
        //           must be consistent.
        // ------------------------------------------------------------
        {
            TEST_SCOPE("testFullStressIntegrity - Scenario B");

            const int runTimeMs = 30000; // сколько крутим стресс

            QThreadPool::globalInstance()->setMaxThreadCount(16);

            StorageManager sm;

            // Стартовое наполнение — чтобы у удалятелей было что удалять
            const int initialRecords = 300;
            for (int i = 0; i < initialRecords; ++i) {
                sm.addRecord(createRecord(i % 8, i));
            }
            QCOMPARE(sm.countRecords(), static_cast<qsizetype>(initialRecords));

            QAtomicInt stopFlag;
            QAtomicInt addOk, addFail;
            QAtomicInt removeOk, removeFail;
            QAtomicInt getterIters, counterIters, serializeIters, adderIters, removerIters;

            QElapsedTimer timerB;
            timerB.start();

            // ---------- Thread 1: addRecord ---
            QFuture<void> fAdd = QtConcurrent::run([&]() {
                int i = 0;
                while (!stopFlag.loadAcquire()) {
                    const Record r = createRecord(0, 100000 + i);
                    if (sm.addRecord(r)) {
                        addOk.fetchAndAddOrdered(1);
                    } else {
                        addFail.fetchAndAddOrdered(1);
                    }
                    ++i;

                    if (i > 999) {
                        break;
                    }
                }
            });

            // ---------- Thread 2: removeRecord ---
            QFuture<void> fRemove = QtConcurrent::run([&]() {
                int i = 0;
                while (!stopFlag.loadAcquire()) {
                    const Record r = createRecord(0, 100000 + i);
                    if (sm.removeRecord(r)) {
                        removeOk.fetchAndAddOrdered(1);
                    } else {
                        removeFail.fetchAndAddOrdered(1);
                    }
                    ++i;

                    if (i > 999) {
                        break;
                    }
                }
            });

            // ---------- Thread 3: Adder ---
            QFuture<void> fAdder = QtConcurrent::run([&]() {
                int i = 0;
                while (!stopFlag.loadAcquire()) {
                    sm.add()
                        .date(makeDate(2, i))
                        .drawing(makeDrawing(2, 200000 + i))
                        .amount(1000000 + i)
                        .executor(QString("E_adder_%1").arg(i))
                        .author(QString("A_adder_%1").arg(i))
                        .castingMaterial(QString("C_adder_%1").arg(i))
                        .modelMaterial(QString("M_adder_%1").arg(i))
                        .machine(QString("Mc_adder_%1").arg(i))
                        .note(QString("N_adder_%1").arg(i));
                    adderIters.fetchAndAddOrdered(1);
                    ++i;

                    if (i > 999) {
                        break;
                    }
                }
            });

            // ---------- Thread 4: Remover ---
            QFuture<void> fRemover = QtConcurrent::run([&]() {
                int i = 0;
                while (!stopFlag.loadAcquire()) {
                    sm.remove()
                        .executor(QString("E_adder_%1").arg(i))
                        .author(QString("A_adder_%1").arg(i))
                        .castingMaterial(QString("C_adder_%1").arg(i))
                        .modelMaterial(QString("M_adder_%1").arg(i))
                        .machine(QString("Mc_adder_%1").arg(i))
                        .note(QString("N_adder_%1").arg(i));
                    removerIters.fetchAndAddOrdered(1);
                    ++i;

                    if (i > 999) {
                        break;
                    }
                }
            });

            // ---------- Thread 5: getRecords ---
            QFuture<void> fGet = QtConcurrent::run([&]() {
                int i = 0;
                while (!stopFlag.loadAcquire()) {
                    const auto records = sm.getRecords();
                    for (const Record& r : records) {
                        QVERIFY(r.isValid());
                    }
                    getterIters.fetchAndAddOrdered(1);
                    ++i;

                    if (i > 999) {
                        break;
                    }
                }
            });

            // ---------- Thread 6: Getter ---
            QFuture<void> fGetLists = QtConcurrent::run([&]() {
                while (!stopFlag.loadAcquire()) {
                    Q_UNUSED(sm.get().dates());
                    Q_UNUSED(sm.get().drawings());
                    Q_UNUSED(sm.get().amounts());
                    Q_UNUSED(sm.get().executors());
                    Q_UNUSED(sm.get().authors());
                    Q_UNUSED(sm.get().castingMaterials());
                    Q_UNUSED(sm.get().modelMaterials());
                    Q_UNUSED(sm.get().machines());
                    Q_UNUSED(sm.get().notes());
                    counterIters.fetchAndAddOrdered(1);
                }
            });

            // ---------- Thread 7: Counter ---
            QFuture<void> fCount = QtConcurrent::run([&]() {
                while (!stopFlag.loadAcquire()) {
                    Q_UNUSED(sm.count().dates());
                    Q_UNUSED(sm.count().drawings());
                    Q_UNUSED(sm.count().amounts());
                    Q_UNUSED(sm.count().executors());
                    Q_UNUSED(sm.count().authors());
                    Q_UNUSED(sm.count().castingMaterials());
                    Q_UNUSED(sm.count().modelMaterials());
                    Q_UNUSED(sm.count().machines());
                    Q_UNUSED(sm.count().notes());
                    counterIters.fetchAndAddOrdered(1);
                }
            });

            // ---------- Thread 8: serialize ---
            QFuture<void> fSerialize = QtConcurrent::run([&]() {
                while (!stopFlag.loadAcquire()) {
                    QByteArray data;
                    QDataStream out(&data, QIODevice::WriteOnly);
                    out.setVersion(QDataStream::Version::Qt_6_11);
                    sm.serialize(out);
                    serializeIters.fetchAndAddOrdered(1);
                }
            });

            // Крутим runTimeMs миллисекунд
            QThread::msleep(runTimeMs);
            stopFlag.storeRelease(1);

            fAdd.waitForFinished();
            fRemove.waitForFinished();
            fAdder.waitForFinished();
            fRemover.waitForFinished();
            fGet.waitForFinished();
            fGetLists.waitForFinished();
            fCount.waitForFinished();
            fSerialize.waitForFinished();

            const qint64 elapsedB = timerB.elapsed();

            qDebug() << "stress elapsed:" << elapsedB << "ms";
            qDebug() << "addOk:" << addOk.loadAcquire() << "addFail:" << addFail.loadAcquire()
                     << "removeOk:" << removeOk.loadAcquire()
                     << "removeFail:" << removeFail.loadAcquire()
                     << "adderIters:" << adderIters.loadAcquire()
                     << "removerIters:" << removerIters.loadAcquire()
                     << "getterIters:" << getterIters.loadAcquire()
                     << "counterIters:" << counterIters.loadAcquire()
                     << "serializeIters:" << serializeIters.loadAcquire() << "runTimeMs"
                     << runTimeMs;
            _scope.metric("elapsed", elapsedB)
                .metric("addOk", addOk.loadAcquire())
                .metric("addFail", addFail.loadAcquire())
                .metric("removeOk", removeOk.loadAcquire())
                .metric("removeFail", removeFail.loadAcquire())
                .metric("adderIters", adderIters.loadAcquire())
                .metric("removerIters", removerIters.loadAcquire())
                .metric("getterIters", getterIters.loadAcquire())
                .metric("counterIters", counterIters.loadAcquire())
                .metric("serializeIters", serializeIters.loadAcquire())
                .metric("runTimeMs", runTimeMs);

            {
                // Все потоки стоят. Состояние sm фиксировано.
                const QList<Record> finalRecords1 = sm.getRecords();
                QByteArray data1;
                {
                    QDataStream out(&data1, QIODevice::WriteOnly);
                    out.setVersion(QDataStream::Version::Qt_6_11);
                    sm.serialize(out);
                }
                StorageManager sm2;
                {
                    QDataStream in(&data1, QIODevice::ReadOnly);
                    in.setVersion(QDataStream::Version::Qt_6_11);
                    sm2.deserialize(in);
                }
                const QList<Record> deserRecords = sm2.getRecords();
                const QList<Record> finalRecords2 = sm.getRecords();

                // 1) getRecords стабилен
                QCOMPARE(finalRecords1.size(), finalRecords2.size());
                for (int i = 0; i < finalRecords1.size(); ++i) {
                    QCOMPARE(makeKey(finalRecords1[i]), makeKey(finalRecords2[i]));
                }

                // 2) serialize/deserialize даёт то же
                QCOMPARE(deserRecords.size(), finalRecords1.size());
                QSet<QString> k1, k2;
                for (const auto& r : finalRecords1) {
                    k1.insert(makeKey(r));
                }
                for (const auto& r : deserRecords) {
                    k2.insert(makeKey(r));
                }
                QCOMPARE(k1, k2);
            }

            // ---- Финальная проверка целостности ----
            const QList<Record> finalRecords = sm.getRecords();
            qDebug() << "final records:" << finalRecords.size();

            QSet<QString> finalKeys;
            for (const Record& r : finalRecords) {
                verifyRecordIntegrity(r, sm);
                finalKeys.insert(makeKey(r));
            }
            // Все записи уникальны (нет дубликатов из-за гонок)
            QCOMPARE(finalKeys.size(), finalRecords.size());

            // ---- serialize финального состояния корректен ----
            QByteArray data;
            {
                QDataStream out(&data, QIODevice::WriteOnly);
                out.setVersion(QDataStream::Version::Qt_6_11);
                sm.serialize(out);
            }
            StorageManager sm2;
            {
                QDataStream in(&data, QIODevice::ReadOnly);
                in.setVersion(QDataStream::Version::Qt_6_11);
                sm2.deserialize(in);
            }
            QCOMPARE(sm2.countRecords(), sm.countRecords());

            QSet<QString> deserKeys;
            for (const Record& r : sm2.getRecords()) {
                verifyRecordIntegrity(r, sm2);
                deserKeys.insert(makeKey(r));
            }

            QCOMPARE(deserKeys, finalKeys);
        }
    }

    // несколько читателей getRecords() одновременно
    void testMultipleConcurrentGetters() {
        const int writerCount = 4;
        const int getterCount = 4;
        const int perThread = 1000;

        StorageManager sm;
        QAtomicInt stopFlag;

        QThreadPool::globalInstance()->setMaxThreadCount(16);

        QList<QFuture<void>> getters;
        for (int g = 0; g < getterCount; ++g) {
            getters.append(QtConcurrent::run([&]() {
                while (!stopFlag.loadAcquire()) {
                    auto records = sm.getRecords(); // ← mutable-кэш, гонка
                    Q_UNUSED(records);
                }
            }));
        }

        QList<QFuture<void>> writers;
        for (int w = 0; w < writerCount; ++w) {
            writers.append(QtConcurrent::run([&, w]() {
                for (int i = 0; i < perThread; ++i) {
                    sm.addRecord(createRecord(w, i));
                }
            }));
        }

        for (auto& f : writers) {
            f.waitForFinished();
        }
        stopFlag.storeRelease(1);
        for (auto& f : getters) {
            f.waitForFinished();
        }

        QCOMPARE(sm.countRecords(), static_cast<qsizetype>(writerCount * perThread));
    }

    // getRecords() во время clear()/reset()
    void testGetDuringClear() {
        QThreadPool::globalInstance()->setMaxThreadCount(16);

        StorageManager sm;
        for (int i = 0; i < 1000; ++i) {
            sm.addRecord(createRecord(0, i));
        }

        QAtomicInt stop(0);
        QFuture<void> getter = QtConcurrent::run([&]() {
            while (!stop.loadAcquire()) {
                auto r = sm.getRecords();
                // размер может быть любым: 0, 1000, промежуточным
                Q_UNUSED(r);
            }
        });

        for (int k = 0; k < 100; ++k) {
            sm.clear();
            for (int i = 0; i < 10; ++i) {
                sm.addRecord(createRecord(0, i));
            }
        }
        stop.storeRelease(1);
        getter.waitForFinished();
    }

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

    // ---------- Multithreading helpers ----------
    QString makeDate(int threadId, int index) const {
        QDate base(2000, 1, 1);
        QDate d = base.addDays(threadId * 1000 + index);
        return d.toString("dd.MM.yyyy");
    }

    Drawing makeDrawing(int threadId, int index) const {
        return Drawing(QString("DWG-%1-%2").arg(threadId).arg(index),
                       QString("Title-%1-%2").arg(threadId).arg(index));
    }

    Record createRecord(int threadId, int index) const {
        return Record(makeDate(threadId, index),
                      makeDrawing(threadId, index),
                      index + 1,
                      QStringList { QString("Exec_%1_%2").arg(threadId).arg(index) },
                      QStringList { QString("Auth_%1_%2").arg(threadId).arg(index) },
                      QStringList { QString("Cast_%1_%2").arg(threadId).arg(index) },
                      QStringList { QString("Model_%1_%2").arg(threadId).arg(index) },
                      QStringList { QString("Mach_%1_%2").arg(threadId).arg(index) },
                      QStringList { QString("Note_%1_%2").arg(threadId).arg(index) });
    }

    QString makeKey(const Record& r) const {
        return QStringList { r.date,
                             r.drawing.getNumber(),
                             r.drawing.getTitle(),
                             QString::number(r.amount),
                             r.executors.join(","),
                             r.authors.join(","),
                             r.castingMaterials.join(","),
                             r.modelMaterials.join(","),
                             r.machines.join(","),
                             r.notes.join(",") }
            .join("|");
    }

    void verifyRecordIntegrity(const Record& r, const StorageManager& sm) const {
        // ---------- 1. Базовые проверки ---
        QVERIFY2(!r.date.isEmpty(), "date пустой");
        QVERIFY2(DatesList::checkDate(r.date), "date не проходит checkDate");
        QVERIFY2(r.drawing.isValid(), "drawing невалиден");
        QVERIFY2(!r.drawing.getNumber().isEmpty(), "drawing.number пуст");
        QVERIFY2(!r.drawing.getTitle().isEmpty(), "drawing.title пуст");
        QVERIFY2(r.amount >= 1, "amount < 1");

        // ---------- 2. Консистентность со StorageLists ---
        //  Каждое значение из Record должно присутствовать в соответствующем
        //  списке StorageLists. Если оно оттуда пропало — значит, ссылка
        //  «полубитая»: RecordLink ещё ссылается на удалённый id.
        const auto allDates = sm.get().datesStr();
        QVERIFY2(allDates.contains(r.date),
                 qPrintable(QString("date '%1' отсутствует в StorageLists::dates").arg(r.date)));

        const auto allDrawings = sm.get().drawings();
        bool drawingFound = false;
        for (const auto& d : allDrawings) {
            if (d == r.drawing) {
                drawingFound = true;
                break;
            }
        }
        QVERIFY2(drawingFound,
                 qPrintable(QString("drawing '%1 / %2' отсутствует в StorageLists::drawings")
                                .arg(r.drawing.getNumber(), r.drawing.getTitle())));

        const auto allAmounts = sm.get().amounts();
        QVERIFY2(allAmounts.contains(r.amount),
                 qPrintable(
                     QString("amount '%1' отсутствует в StorageLists::amounts").arg(r.amount)));

        auto checkList =
            [](const QStringList& values, const auto& allValues, const char* fieldName) {
                for (const QString& v : values) {
                    if (!allValues.contains(v)) {
                        QFAIL(qPrintable(
                            QString("%1 '%2' отсутствует в StorageLists").arg(fieldName, v)));
                    }
                }
            };

        checkList(r.executors, sm.get().executors(), "executor");
        checkList(r.authors, sm.get().authors(), "author");
        checkList(r.castingMaterials, sm.get().castingMaterials(), "castingMaterial");
        checkList(r.modelMaterials, sm.get().modelMaterials(), "modelMaterial");
        checkList(r.machines, sm.get().machines(), "machine");
        checkList(r.notes, sm.get().notes(), "note");
    }
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

    QCOMPARE(sm.get().datesStr().first(), QStringLiteral("01.03.2024"));
    QCOMPARE(sm.get().amounts().first(), 10);
    QCOMPARE(sm.get().executors().first(), QStringLiteral("ExecA"));
}

void TestStorageManager::testRemoverFluent() {
    StorageManager sm;
    sm.add()
        .date(QStringLiteral("02.03.2024"))
        .amount(1)
        .executor(QStringLiteral("ExecB"))
        .note(QStringLiteral("NoteB"));

    QCOMPARE(sm.count().executors(), 1);
    QCOMPARE(sm.count().notes(), 1);

    sm.remove().executor(QStringLiteral("ExecB")).note(QStringLiteral("NoteB"));

    QCOMPARE(sm.count().executors(), 0);
    QCOMPARE(sm.count().notes(), 0);
}

void TestStorageManager::testGetter() {
    StorageManager sm;
    sm.add()
        .date(QStringLiteral("01.04.2024"))
        .amount(7)
        .executor(QStringLiteral("ExecC"))
        .author(QStringLiteral("AuthorC"));

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

    sm.add().date(QStringLiteral("01.05.2024"));
    sm.add().date(QStringLiteral("02.05.2024"));
    sm.add().date(QStringLiteral("01.05.2024"));
    sm.add().date(QStringLiteral("01.05.2024"));

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

    sm.remove()
        .executor(QStringLiteral("ExecX"))
        .castingMaterial(QStringLiteral("CastX"))
        .machine(QStringLiteral("MachineX"));

    QCOMPARE(sm.countRecords(), qsizetype(1));
    const Record got = sm.getRecords().first();
    QVERIFY(got.executors.isEmpty());
    QVERIFY(got.castingMaterials.isEmpty());
    QVERIFY(got.machines.isEmpty());

    QCOMPARE(got.authors, QStringList { QStringLiteral("AuthorX") });
    QCOMPARE(got.modelMaterials, QStringList { QStringLiteral("ModelX") });
    QCOMPARE(got.notes, QStringList { QStringLiteral("NoteX") });
}

// ---------------------------------------------------------------------------
// Кэш записей
// ---------------------------------------------------------------------------

void TestStorageManager::testRecordsCacheInvalidation() {
    StorageManager sm;
    QVERIFY(sm.getRecords().isEmpty());

    const Record r = makeValidRecord(QStringLiteral("01.08.2024"), 1);
    QVERIFY(sm.addRecord(r));

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

QTEST_MAIN(TestStorageManager)
#include "test_StorageManager.moc"
