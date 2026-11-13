#ifndef SAVERFILE_H
#define SAVERFILE_H

#include <QDataStream>
#include <QFile>
#include <QReadWriteLock>
#include <QString>

#include "Saver.h"

namespace mdt {
    class SaverFile final : public Saver {
    public:
        SaverFile(Storage *storage, const QString &fileName);
        ~SaverFile() override;

        [[nodiscard]] bool prepare() override;
        [[nodiscard]] bool read() override;
        [[nodiscard]] bool write() override;

    private:
        const QString m_fileName;
        const QString m_tempFileName;
        bool m_dirty = false;
        QReadWriteLock m_lock;
    };
} // namespace mdt

#endif // SAVERFILE_H
