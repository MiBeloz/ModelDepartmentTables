#ifndef SAVERFILESTORAGELISTS_H
#define SAVERFILESTORAGELISTS_H

#include <QFile>

#include "SaverFile.h"
#include "StorageLists.h"

class SaverFileStorageLists final : public SaverFile<StorageLists> {
public:
    SaverFileStorageLists(const QString &fileName,
                          const QString &tempFileName,
                          const QString &backupFileName)
        : SaverFile(fileName, tempFileName, backupFileName) { }

    ~SaverFileStorageLists() override = default;

    bool write(const StorageLists &storage) override {
        if (m_tempFile.open(QIODeviceBase::WriteOnly)) {
            QDataStream stream(&m_tempFile);
            stream.setVersion(QDataStream::Qt_6_11);

            try {
                storage.dates().serialize(stream);
                storage.drawings().serialize(stream);
                storage.executors().serialize(stream);
                storage.authors().serialize(stream);
                storage.amounts().serialize(stream);
                storage.castingMaterials().serialize(stream);
                storage.modelMaterials().serialize(stream);
                storage.machines().serialize(stream);
                storage.notes().serialize(stream);
            } catch (RuntimeError &error) {
                m_file.close();
                throw error;
            } catch (...) {
                m_file.close();
                return false;
            }

            m_tempFile.close();

            if (stream.status() != QDataStream::Ok) {
                m_tempFile.remove();
                return false;
            }

            m_save = false;
            return true;
        } else {
            return false;
        }
    }

    bool read(StorageLists &storage) override {
        if (m_file.open(QIODeviceBase::ReadOnly)) {
            QDataStream stream(&m_file);
            stream.setVersion(QDataStream::Qt_6_11);

            try {
                storage.dates().deserialize(stream);
                storage.drawings().deserialize(stream);
                storage.executors().deserialize(stream);
                storage.authors().deserialize(stream);
                storage.amounts().deserialize(stream);
                storage.castingMaterials().deserialize(stream);
                storage.modelMaterials().deserialize(stream);
                storage.machines().deserialize(stream);
                storage.notes().deserialize(stream);
            } catch (RuntimeError &error) {
                m_file.close();
                throw error;
            } catch (...) {
                m_file.close();
                return false;
            }

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

#endif //  SAVERFILESTORAGELISTS
