#ifndef TESTSTATS_H
#define TESTSTATS_H

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QElapsedTimer>
#include <QFile>
#include <QMap>
#include <QMutex>
#include <QMutexLocker>
#include <QString>
#include <QStringList>
#include <QTextStream>

class TestStats {
public:
    struct Entry {
        QString testName;
        qint64 durationMs = 0;
        QMap<QString, QString> metrics;
        bool passed = true;
        QString failureMessage;
    };

    static TestStats& instance() {
        static TestStats s;
        return s;
    }

    void addEntry(const Entry& e) {
        QMutexLocker lock(&m_mutex);
        m_entries.append(e);
        appendToFile(e);
    }

    QString filePath() const {
        return m_filePath;
    }

private:
    TestStats() {
        const QString dir = QCoreApplication::applicationDirPath();
        m_filePath = dir + '/' +
                     QDateTime::currentDateTime().toString(QString("dd-MM-yyyy_hh-mm-ss")) +
                     "_testStats.txt";

        QFile f(m_filePath);
        if (f.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            QTextStream out(&f);
            out << "==================================================\n";
            out << "Test run: "
                << QDateTime::currentDateTime().toString(QString("dd-MM-yyyy - hh-mm-ss")) << "\n";
            out << "==================================================\n";
        }
    }

    void appendToFile(const Entry& e) {
        QFile f(m_filePath);
        if (!f.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            qWarning() << "TestStats: cannot open" << m_filePath;
            return;
        }
        QTextStream out(&f);

        out << "Test: " << e.testName << "\n";
        out << "  Duration: " << e.durationMs << " ms\n";
        out << "  Status: " << (e.passed ? "PASSED" : "FAILED") << "\n";
        if (!e.passed && !e.failureMessage.isEmpty()) {
            out << "  Failure: " << e.failureMessage << "\n";
        }
        for (auto it = e.metrics.begin(); it != e.metrics.end(); ++it) {
            out << "  " << it.key() << ": " << it.value() << "\n";
        }
        out << "\n";
        out.flush();
    }

    mutable QMutex m_mutex;
    QList<Entry> m_entries;
    QString m_filePath;
};

class TestScope {
public:
    explicit TestScope(const QString& testName) : m_name(testName) {
        m_timer.start();
    }

    TestScope& metric(const QString& key, const QString& value) {
        m_metrics.insert(key, value);
        return *this;
    }
    TestScope& metric(const QString& key, qint64 value) {
        m_metrics.insert(key, QString::number(value));
        return *this;
    }
    TestScope& metric(const QString& key, int value) {
        m_metrics.insert(key, QString::number(value));
        return *this;
    }

    TestScope& fail(const QString& msg) {
        m_passed = false;
        m_failureMessage = msg;
        return *this;
    }

    ~TestScope() {
        TestStats::Entry e;
        e.testName = m_name;
        e.durationMs = m_timer.elapsed();
        e.metrics = m_metrics;
        e.passed = m_passed;
        e.failureMessage = m_failureMessage;
        TestStats::instance().addEntry(e);
    }

private:
    QString m_name;
    QElapsedTimer m_timer;
    QMap<QString, QString> m_metrics;
    bool m_passed = true;
    QString m_failureMessage;
};

// TEST_SCOPE("test_name");
#define TEST_SCOPE(name)                    \
    TestScope _scope(QStringLiteral(name)); \
    Q_UNUSED(_scope)

#endif // TESTSTATS_H
