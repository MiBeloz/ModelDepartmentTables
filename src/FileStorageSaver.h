#ifndef FILESTORAGESAVER_H
#define FILESTORAGESAVER_H

#include <QFile>

#include "IStorageSaver.h"

class FileStorageSaver final : public IStorageSaver {
public:
    FileStorageSaver(const QString &fileName) {
        m_file.setFileName(fileName);
    }

    ~FileStorageSaver() = default;

    bool save(const StorageService &storage) override {
        if (m_file.open(QIODeviceBase::WriteOnly)) {
            QDataStream stream(&m_file);
            stream.setVersion(QDataStream::Qt_6_11);

            storage.dates().serialize(stream);
            storage.drawings().serialize(stream);
            storage.executors().serialize(stream);
            storage.authors().serialize(stream);
            storage.amounts().serialize(stream);
            storage.castingMaterials().serialize(stream);
            storage.modelMaterials().serialize(stream);
            storage.machines().serialize(stream);
            storage.notes().serialize(stream);

            m_file.close();
            return true;
        } else {
            return false;
        }
    }

    bool load(StorageService &storage) override {
        if (m_file.open(QIODeviceBase::ReadOnly)) {
            QDataStream stream(&m_file);
            stream.setVersion(QDataStream::Qt_6_11);

            storage.dates().deserialize(stream);
            storage.drawings().deserialize(stream);
            storage.executors().deserialize(stream);
            storage.authors().deserialize(stream);
            storage.amounts().deserialize(stream);
            storage.castingMaterials().deserialize(stream);
            storage.modelMaterials().deserialize(stream);
            storage.machines().deserialize(stream);
            storage.notes().deserialize(stream);

            m_file.close();
            return true;
        } else {
            return false;
        }
    }

private:
    QFile m_file;
};

#endif // FILESTORAGESAVER_H
