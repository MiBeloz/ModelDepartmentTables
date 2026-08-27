#include "DatabaseManager.h"
#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <QHostInfo>

DatabaseManager::DatabaseManager(QObject *parent) : QObject(parent) {
    m_activeConnections = 0;

    // Проверяем доступность драйвера
    if (!QSqlDatabase::isDriverAvailable("QPSQL")) {
        qCritical() << "PostgreSQL driver not available!";
    }
}

DatabaseManager::~DatabaseManager() {
    qDebug() << "DatabaseManager destructor started. Active connections:" << m_activeConnections;

    if (m_activeConnections > 0) {
        disconnectAllThreads();
    }

    if (m_mainConnection.isOpen()) {
        disconnect();
    }

    qDebug() << "DatabaseManager destructor finished";
}

void DatabaseManager::setConnectionParams(const QString& host, int port,
                                          const QString& dbName,
                                          const QString& user,
                                          const QString& password)
{
    QMutexLocker locker(&m_mutex);
    m_host = host;
    m_port = port;
    m_dbName = dbName;
    m_user = user;
    m_password = password;
}

bool DatabaseManager::connect() {
    QMutexLocker locker(&m_mutex);

    // Проверяем параметры подключения
    if (m_host.isEmpty() || m_dbName.isEmpty() || m_user.isEmpty()) {
        qCritical() << "Database connection parameters are incomplete!";
        return false;
    }

    // Удаляем предыдущее подключение если оно было
    if (m_mainConnection.isValid()) {
        QString connectionName = m_mainConnection.connectionName();
        m_mainConnection.close();
        QSqlDatabase::removeDatabase(connectionName);
    }

    // Создаем основное подключение
    m_mainConnection = QSqlDatabase::addDatabase("QPSQL", "main_connection");
    m_mainConnection.setHostName(m_host);
    m_mainConnection.setPort(m_port);
    m_mainConnection.setDatabaseName(m_dbName);
    m_mainConnection.setUserName(m_user);
    m_mainConnection.setPassword(m_password);

    // Дополнительные параметры для PostgreSQL
    m_mainConnection.setConnectOptions("connect_timeout=10"); // Таймаут подключения 10 секунд

    qDebug() << "Attempting to connect to PostgreSQL at" << m_host << ":" << m_port;
    qDebug() << "Database:" << m_dbName << "User:" << m_user;

    if (!m_mainConnection.open()) {
        QSqlError error = m_mainConnection.lastError();
        qCritical() << "Database connection error:" << m_mainConnection.lastError().text();

        return false;
    }

    qDebug() << "Successfully connected to PostgreSQL database";

    // Подключаем таймер к лямбда-функции
    m_monitorTimer = new QTimer(this);
    QObject::connect(m_monitorTimer, &QTimer::timeout, [this]() {
        // Получаем текущее время
        QString currentTime = QTime::currentTime().toString("hh:mm:ss");

        qDebug() << "\n[" << currentTime << "] - Active connections: " << QSqlDatabase::connectionNames();
        emit sig_printActiveConnections(QSqlDatabase::connectionNames());

    });

    // Запускаем таймер с интервалом 1000 мс (1 секунда)
    m_monitorTimer->start(5000);

    return true;
}

void DatabaseManager::disconnect() {
    qDebug() << "DatabaseManager::disconnect() started";

    QMutexLocker locker(&m_mutex);

    // Закрываем основное соединение
    if (m_mainConnection.isOpen()) {
        QString connectionName = m_mainConnection.connectionName();
        m_mainConnection.close();

        m_mainConnection = QSqlDatabase();
        QSqlDatabase::removeDatabase(connectionName);

        qDebug() << "Main connection closed and removed";
    }

    qDebug() << "DatabaseManager::disconnect() finished";
}

QSqlDatabase DatabaseManager::getConnection() {
    // Каждый поток получает свое собственное соединение
    if (!m_threadConnections.hasLocalData()) {
        QMutexLocker locker(&m_mutex);

        // Создаем уникальное имя для соединения в этом потоке
        QString connectionName = QString("thread_%1_%2")
                                     .arg((quintptr)QThread::currentThreadId())
                                     .arg(QRandomGenerator::global()->generate());

        qDebug() << "Creating new database connection for thread:"
                 << QThread::currentThreadId() << "Name:" << connectionName;

        QSqlDatabase db = QSqlDatabase::addDatabase("QPSQL", connectionName);
        db.setHostName(m_host);
        db.setPort(m_port);
        db.setDatabaseName(m_dbName);
        db.setUserName(m_user);
        db.setPassword(m_password);
        db.setConnectOptions("connect_timeout=5");

        if (!db.open()) {
            qCritical() << "Failed to open thread connection for thread"
                        << QThread::currentThreadId() << ":"
                        << db.lastError().text();
            return QSqlDatabase();
        }

        qDebug() << "Thread connection opened successfully for thread:"
                 << QThread::currentThreadId();

        m_threadConnections.setLocalData(db);
        m_activeConnections.ref(); // Увеличиваем счетчик активных соединений

        // Подключаем сигнал завершения потока для очистки
        QThread* currentThread = QThread::currentThread();
        QObject::connect(currentThread, &QThread::finished, this, [this, connectionName]() {
            qDebug() << "Thread" << QThread::currentThreadId() << "finished, cleaning up connection:" << connectionName;

            // Очищаем соединение для этого потока
            {
                QMutexLocker locker(&m_mutex);
                if (m_threadConnections.hasLocalData()) {
                    QSqlDatabase db = m_threadConnections.localData();
                    if (db.isOpen()) {
                        db.close();
                    }
                    m_threadConnections.setLocalData(QSqlDatabase());
                }
            }

            // Удаляем соединение из реестра
            QSqlDatabase::removeDatabase(connectionName);
            m_activeConnections.deref(); // Уменьшаем счетчик активных соединений

            qDebug() << "Connection" << connectionName << "cleaned up. Active connections:" << m_activeConnections;
        });
    }

    return m_threadConnections.localData();
}

void DatabaseManager::disconnectAllThreads() {
    qDebug() << "DatabaseManager::disconnectAllThreads() started. Active connections:" << m_activeConnections;

    // Ждем, пока все потоки освободят соединения
    int waitCount = 0;
    while (m_activeConnections > 0 && waitCount < 50) { // Максимум 5 секунд ожидания
        QThread::msleep(100);
        waitCount++;
        qDebug() << "Waiting for active connections to close..." << m_activeConnections;
    }

    qDebug() << "DatabaseManager::disconnectAllThreads() finished. Active connections:" << m_activeConnections;
}

bool DatabaseManager::createTable() {
    QSqlDatabase db = getConnection();
    if (!db.isOpen() || !db.isValid()) {
        qCritical() << "Invalid database connection in createTable";
        return false;
    }

    QSqlQuery query(db);

    // Используем более безопасный синтаксис с проверкой существования
    QString createTableSQL =
        "CREATE TABLE IF NOT EXISTS test_data ("
        "id SERIAL PRIMARY KEY,"
        "thread_id INTEGER NOT NULL,"
        "data TEXT UNIQUE NOT NULL,"
        "timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP"
        ")";

    bool success = query.exec(createTableSQL);

    if (!success) {
        QSqlError error = query.lastError();
        qCritical() << "Failed to create table:" << error.text();
        qDebug() << "Query:" << createTableSQL;
    } else {
        qDebug() << "Table created/verified successfully";
    }

    return success;
}

bool DatabaseManager::insertData(int threadId, const QString& data) {
    QSqlDatabase db = getConnection();
    if (!db.isOpen() || !db.isValid()) {
        qCritical() << "Invalid database connection in insertData for thread" << threadId;
        return false;
    }

    QSqlQuery query(db);

    // Используем именованные параметры для лучшей читаемости
    query.prepare("INSERT INTO test_data (thread_id, data) VALUES (:thread_id, :data)");
    query.bindValue(":thread_id", threadId);
    query.bindValue(":data", data);

    if (!query.exec()) {
        QSqlError error = query.lastError();
        qCritical() << "Failed to insert data for thread" << threadId << ":" << error.text();
        return false;
    }

    qDebug() << "Thread" << threadId << "inserted data successfully";
    return true;
}

QSqlQuery DatabaseManager::selectData(const QString& condition) {
    QSqlDatabase db = getConnection();
    QSqlQuery query(db);

    QString sql = "SELECT * FROM test_data";
    if (!condition.isEmpty()) {
        sql += " WHERE " + condition;
    }
    sql += " ORDER BY timestamp DESC";

    if (!query.exec(sql)) {
        QSqlError error = query.lastError();
        qCritical() << "Failed to select data:" << error.text();
    }

    return query;
}
