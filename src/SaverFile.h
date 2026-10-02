#ifndef SAVERFILE_H
#define SAVERFILE_H

#include <QDataStream>
#include <QFile>
#include <QReadWriteLock>
#include <QString>

#ifdef Q_OS_WIN
#include <QDir>

#include <string>
#include <windows.h>
#endif

#include "Saver.h"

template<typename T>
class SaverFile final : public Saver<T> {
public:
    SaverFile(QString fileName, QString tempFileName)
        : m_fileName(std::move(fileName))
        , m_tempFileName(std::move(tempFileName)) { }

    ~SaverFile() override {
        QFile::remove(m_tempFileName);
    }

    [[nodiscard]] bool prepare(const T &storage) override {
        const QWriteLocker locker(&m_lock);

        QFile temp(m_tempFileName);
        if (!temp.open(QIODeviceBase::WriteOnly | QIODeviceBase::Truncate)) {
            return false;
        }

        QDataStream stream(&temp);
        stream.setVersion(QDataStream::Qt_6_0);

        try {
            storage.serialize(stream);
        } catch (...) {
            temp.close();
            temp.remove();
            m_dirty = false;
            throw;
        }

        const bool flushed = temp.flush();
        temp.close();

        if (!flushed || stream.status() != QDataStream::Ok) {
            temp.remove();
            m_dirty = false;
            return false;
        }

        m_dirty = true;
        return true;
    }

    [[nodiscard]] bool read(T &storage) override {
        const QReadLocker locker(&m_lock);

        QFile file(m_fileName);
        if (!file.open(QIODeviceBase::ReadOnly)) {
            return false;
        }

        QDataStream stream(&file);
        stream.setVersion(QDataStream::Qt_6_0);

        T tmp { };
        try {
            tmp.deserialize(stream);
        } catch (...) {
            file.close();
            throw;
        }

        file.close();

        if (stream.status() != QDataStream::Ok) {
            return false;
        }

        storage = std::move(tmp);
        return true;
    }

    [[nodiscard]] bool write() override {
        const QWriteLocker locker(&m_lock);

        if (!m_dirty) {
            return true;
        }

        QFile temp(m_tempFileName);
        if (!temp.exists()) {
            return false;
        }

        QFile file(m_fileName);
        if (file.exists() && !file.remove()) {
            return false;
        }

#ifdef Q_OS_WIN
        const std::wstring from = QDir::toNativeSeparators(m_tempFileName).toStdWString();
        const std::wstring to = QDir::toNativeSeparators(m_fileName).toStdWString();

        if (!::MoveFileExW(from.c_str(), to.c_str(), MOVEFILE_REPLACE_EXISTING)) {
            return false;
        }
#else
        if (!temp.rename(m_fileName)) {
            return false;
        }
#endif

        m_dirty = false;
        return true;
    }

private:
    const QString m_fileName;
    const QString m_tempFileName;
    bool m_dirty = false;
    QReadWriteLock m_lock;
};

#endif // SAVERFILE_H
