#ifndef SAVERFILESTORAGERECORDSLINKS_H
#define SAVERFILESTORAGERECORDSLINKS_H

#include <QFile>

#include "SaverFile.h"
#include "StorageRecordsLinks.h"

class SaverFileStorageRecordsLinks final : public SaverFile<StorageRecordsLinks> {
public:
    SaverFileStorageRecordsLinks(const QString &fileName,
                                 const QString &tempFileName,
                                 const QString &backupFileName)
        : SaverFile(fileName, tempFileName, backupFileName) { }

    bool write(const StorageRecordsLinks &storage) override {
        if (m_tempFile.open(QIODeviceBase::WriteOnly)) {
            QDataStream stream(&m_tempFile);
            stream.setVersion(QDataStream::Qt_6_11);

            storage.serialize(stream);

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

    bool read(StorageRecordsLinks &storage) override {
        if (m_file.open(QIODeviceBase::ReadOnly)) {
            QDataStream stream(&m_file);
            stream.setVersion(QDataStream::Qt_6_11);

            storage.deserialize(stream);

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

#endif // SAVERFILESTORAGERECORDSLINKS_H
