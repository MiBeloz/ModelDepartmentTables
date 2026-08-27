#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QObject>
#include <QtSql>
#include <QMutex>
#include <QWaitCondition>
#include <QThread>
#include <QAtomicInt>

#include "CacheManager.hpp"


class DatabaseManager : public QObject {
    Q_OBJECT

public:
    explicit DatabaseManager(QObject *parent = nullptr);
    ~DatabaseManager();

    void setConnectionParams(const QString& host, int port,
                             const QString& dbName,
                             const QString& user,
                             const QString& password);

    bool connect();
    void disconnect();

    // Потокобезопасные методы для работы с БД
    QSqlDatabase getConnection();
    void disconnectAllThreads();

    // Примеры операций с БД
    bool createTable();
    bool insertData(int threadId, const QString& data);
    QSqlQuery selectData(const QString& condition = QString());

    // Счетчик активных соединений
    int activeConnectionCount() const { return m_activeConnections; }

signals:
    void sig_printActiveConnections(const QStringList& connectionName);

private:
    QString m_host;
    int m_port;
    QString m_dbName;
    QString m_user;
    QString m_password;

    QMutex m_mutex;
    QSqlDatabase m_mainConnection;
    QThreadStorage<QSqlDatabase> m_threadConnections;
    QAtomicInt m_activeConnections;  // Счетчик активных соединений

    CacheManager m_cacheManager;

    QTimer* m_monitorTimer;
};

#endif // DATABASEMANAGER_H
