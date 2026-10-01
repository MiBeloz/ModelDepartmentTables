#include <QDataStream>
#include <QFile>
#include <QString>
#include <QTemporaryDir>
#include <QtTest>

#include "SaverFile.h"
#include <stdexcept>

struct TestStorage {
    int number = 0;
    QString text;

    static inline bool throwOnSerialize = false;
    static inline bool throwOnDeserialize = false;

    void serialize(QDataStream &stream) const {
        if (throwOnSerialize) {
            throw std::runtime_error("serialize failed");
        }
        stream << number << text;
    }

    void deserialize(QDataStream &stream) {
        if (throwOnDeserialize) {
            throw std::runtime_error("deserialize failed");
        }
        stream >> number >> text;
    }

    friend bool operator ==(const TestStorage &a, const TestStorage &b) {
        return a.number == b.number && a.text == b.text;
    }
};

class SaverFileTest : public QObject {
    Q_OBJECT

private slots:
    void init();
    void cleanup();

    // write
    void write_createsTempFile();
    void write_truncatesExistingTemp();
    void write_returnsFalseOnBadPath();
    void write_returnsFalseOnSerializeError();
    void write_propagatesSerializeException();

    // save
    void save_returnsTrueWhenNothingToDo();
    void save_returnsFalseWithoutWrite();
    void save_movesTempToMain();
    void save_overwritesExistingMain();
    void save_resetsDirtyFlag();

    // read
    void read_returnsFalseWhenFileMissing();
    void read_returnsFalseOnCorruptFile();
    void read_doesNotModifyStorageOnFailure();
    void read_roundTrip();

    // взаимодействие
    void fullCycle_writeSaveRead();
    void read_ignoresUncommittedTemp();
    void save_isIdempotent();

private:
    QString mainPath() const {
        return m_dir->filePath("main.dat");
    }
    QString tempPath() const {
        return m_dir->filePath("main.dat.tmp");
    }

    std::unique_ptr<QTemporaryDir> m_dir;
    std::unique_ptr<SaverFile<TestStorage>> m_saver;
};

void SaverFileTest::init() {
    m_dir = std::make_unique<QTemporaryDir>();
    QVERIFY(m_dir->isValid());
    m_saver = std::make_unique<SaverFile<TestStorage>>(mainPath(), tempPath());
    TestStorage::throwOnSerialize = false;
    TestStorage::throwOnDeserialize = false;
}

void SaverFileTest::cleanup() {
    m_saver.reset();
    m_dir.reset();
}

void SaverFileTest::write_createsTempFile() {
    TestStorage s { 42, "hello" };
    QVERIFY(m_saver->write(s));

    QVERIFY(QFile::exists(tempPath()));
    QVERIFY(!QFile::exists(mainPath())); // основной файл не тронут
}

void SaverFileTest::write_truncatesExistingTemp() {
    QFile temp(tempPath());
    QVERIFY(temp.open(QIODeviceBase::WriteOnly));
    temp.write(QByteArray(1000, 'x'));
    temp.close();

    TestStorage s { 1, "a" };
    QVERIFY(m_saver->write(s));

    QVERIFY(QFile::size(tempPath()) < 1000);
}

void SaverFileTest::write_returnsFalseOnBadPath() {
    SaverFile<TestStorage> bad("/nonexistent_dir_xyz/main.dat", "/nonexistent_dir_xyz/main.dat.tmp");
    TestStorage s { 1, "a" };
    QVERIFY(!bad.write(s));
}

void SaverFileTest::write_returnsFalseOnSerializeError() {
    // ломаем поток: закроем файл до записи — не выйдет через публичный API,
    // поэтому проверяем через статус потока косвенно: пишем в путь,
    // который станет недоступен. Проще — проверить исключение.
    // Здесь оставим заготовку, реальную проверку статуса сложно вызвать.
    QSKIP("Сложно вызвать QDataStream::status != Ok через публичный API");
}

void SaverFileTest::write_propagatesSerializeException() {
    TestStorage::throwOnSerialize = true;
    TestStorage s { 1, "a" };

    QVERIFY_THROWS_EXCEPTION(std::runtime_error, m_saver->write(s));

    // temp должен быть удалён
    QVERIFY(!QFile::exists(tempPath()));
}

void SaverFileTest::save_returnsTrueWhenNothingToDo() {
    // ничего не писали — сохранять нечего, но это не ошибка
    QVERIFY(m_saver->save());
}

void SaverFileTest::save_returnsFalseWithoutWrite() {
    // save без write: dirty == false, значит save вернёт true (нечего делать).
    // Чтобы получить false, нужно, чтобы dirty был true, но temp исчез.
    TestStorage s { 1, "a" };
    QVERIFY(m_saver->write(s));

    QVERIFY(QFile::remove(tempPath())); // кто-то удалил temp

    QVERIFY(!m_saver->save());          // коммитить нечего
}

void SaverFileTest::save_movesTempToMain() {
    TestStorage s { 7, "world" };
    QVERIFY(m_saver->write(s));
    QVERIFY(m_saver->save());

    QVERIFY(QFile::exists(mainPath()));
    QVERIFY(!QFile::exists(tempPath()));
}

void SaverFileTest::save_overwritesExistingMain() {
    // создаём «старый» основной файл
    {
        QFile f(mainPath());
        QVERIFY(f.open(QIODeviceBase::WriteOnly));
        f.write("OLD");
    }

    TestStorage s { 99, "new" };
    QVERIFY(m_saver->write(s));
    QVERIFY(m_saver->save());

    // читаем — должен быть новый
    TestStorage out;
    QVERIFY(m_saver->read(out));
    QCOMPARE(out, s);
}

void SaverFileTest::save_resetsDirtyFlag() {
    TestStorage s { 1, "a" };
    QVERIFY(m_saver->write(s));
    QVERIFY(m_saver->save());

    // после save повторный save — no-op, но true
    QVERIFY(m_saver->save());

    // а write снова делает dirty
    QVERIFY(m_saver->write(s));
    QVERIFY(QFile::exists(tempPath())); // temp появился снова
}

void SaverFileTest::read_returnsFalseWhenFileMissing() {
    TestStorage s;
    QVERIFY(!m_saver->read(s));
}

void SaverFileTest::read_returnsFalseOnCorruptFile() {
    QFile f(mainPath());
    QVERIFY(f.open(QIODeviceBase::WriteOnly));
    f.write("not a valid stream");
    f.close();

    TestStorage s;
    QVERIFY(!m_saver->read(s));
}

void SaverFileTest::read_doesNotModifyStorageOnFailure() {
    QFile f(mainPath());
    QVERIFY(f.open(QIODeviceBase::WriteOnly));
    f.write("\x00\x00"); // мусор
    f.close();

    TestStorage s { 123, "unchanged" };
    QVERIFY(!m_saver->read(s));
    QCOMPARE(s.number, 123);
    QCOMPARE(s.text, QString("unchanged"));
}

void SaverFileTest::read_roundTrip() {
    TestStorage in { 42, "hello" };
    QVERIFY(m_saver->write(in));
    QVERIFY(m_saver->save());

    TestStorage out;
    QVERIFY(m_saver->read(out));
    QCOMPARE(out, in);
}

void SaverFileTest::fullCycle_writeSaveRead() {
    TestStorage a { 1, "first" };
    QVERIFY(m_saver->write(a));
    QVERIFY(m_saver->save());

    TestStorage b { 2, "second" };
    QVERIFY(m_saver->write(b));
    QVERIFY(m_saver->save());

    TestStorage out;
    QVERIFY(m_saver->read(out));
    QCOMPARE(out, b);
}

void SaverFileTest::read_ignoresUncommittedTemp() {
    // основной файл содержит одно, temp — другое, save не вызван
    TestStorage committed { 10, "committed" };
    QVERIFY(m_saver->write(committed));
    QVERIFY(m_saver->save());

    TestStorage pending { 20, "pending" };
    QVERIFY(m_saver->write(pending));
    // save НЕ вызываем

    TestStorage out;
    QVERIFY(m_saver->read(out));
    QCOMPARE(out, committed); // read видит основной файл, не temp
}

void SaverFileTest::save_isIdempotent() {
    TestStorage s { 5, "x" };
    QVERIFY(m_saver->write(s));
    QVERIFY(m_saver->save());
    QVERIFY(m_saver->save()); // второй раз — no-op, но true
    QVERIFY(m_saver->save());

    TestStorage out;
    QVERIFY(m_saver->read(out));
    QCOMPARE(out, s);
}

QTEST_MAIN(SaverFileTest)
#include "test_SaverFile.moc"
