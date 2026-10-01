#ifndef SAVERFILE_H
#define SAVERFILE_H

#include <QDataStream>
#include <QFile>
#include <QString>

#include "Saver.h"

template<typename T>
class SaverFile final : public Saver<T> {
public:
    SaverFile(QString fileName, QString tempFileName)
        : m_fileName(std::move(fileName))
        , m_tempFileName(std::move(tempFileName)) { }

    [[nodiscard]] bool write(const T &storage) override {
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
        m_dirty = false;
        return true;
    }

    [[nodiscard]] bool save() override {
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

        if (!temp.rename(m_fileName)) {
            return false;
        }

        m_dirty = false;
        return true;
    }

private:
    QString m_fileName;
    QString m_tempFileName;
    bool m_dirty = false;
};

#endif // SAVERFILE_H
