#ifndef SAVERFILE_H
#define SAVERFILE_H

#include <QDataStream>
#include <QFile>
#include <QReadWriteLock>
#include <QString>

#include "Saver.h"

class SaverFile final : public Saver {
public:
    SaverFile(QString fileName, QString tempFileName);

    ~SaverFile() override;

    [[nodiscard]] bool prepare(const Storage &storage) override;

    [[nodiscard]] bool read(Storage &storage) override;

    [[nodiscard]] bool write() override;

private:
    const QString m_fileName;
    const QString m_tempFileName;
    bool m_dirty = false;
    QReadWriteLock m_lock;
};

#endif // SAVERFILE_H
