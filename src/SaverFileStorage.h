#ifndef SAVERFILESTORAGE_H
#define SAVERFILESTORAGE_H

#include <QFile>

#include "SaverFile.h"
#include "StorageService.h"

class SaverFileStorage final : public SaverFile<StorageService> {
public:
    SaverFileStorage(const QString &fileName,
                     const QString &tempFileName,
                     const QString &backupFileName)
        : SaverFile(fileName, tempFileName, backupFileName) { }

    ~SaverFileStorage() override = default;

    bool save(const StorageService &storage) override {
        if (m_tempFile.open(QIODeviceBase::WriteOnly)) {
            QDataStream stream(&m_tempFile);
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

            m_tempFile.close();

            if (stream.status() != QDataStream::Ok) {
                m_tempFile.remove();
                return false;
            }

            m_saved = true;
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

            if (stream.status() != QDataStream::Ok) {
                return false;
            }
            return true;
        } else {
            return false;
        }
    }
};

#endif // SAVERFILESTORAGE_H
