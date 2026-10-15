#include "SaverFile.h"

#ifdef Q_OS_WIN
#include <QDir>

#include <string>
#include <windows.h>
#endif

SaverFile::SaverFile(QString fileName, QString tempFileName)
    : m_fileName(std::move(fileName))
    , m_tempFileName(std::move(tempFileName)) { }

SaverFile::~SaverFile() {
    QFile::remove(m_tempFileName);
}

bool SaverFile::prepare(const Storage &storage) {
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

bool SaverFile::read(Storage &storage) {
    const QReadLocker locker(&m_lock);

    QFile file(m_fileName);
    if (!file.open(QIODeviceBase::ReadOnly)) {
        return false;
    }

    QDataStream stream(&file);
    stream.setVersion(QDataStream::Qt_6_0);

    try {
        storage.deserialize(stream);
    } catch (...) {
        file.close();
        throw;
    }

    file.close();

    if (stream.status() != QDataStream::Ok) {
        return false;
    }

    return true;
}

bool SaverFile::write() {
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
