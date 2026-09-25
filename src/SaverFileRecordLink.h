#ifndef SAVERFILERECORDLINK_H
#define SAVERFILERECORDLINK_H

#include <QFile>

#include "SaverFile.h"
#include "StorageRecordLink.h"

class SaverFileRecordLink final : public SaverFile<StorageRecordLink> {
public:
    SaverFileRecordLink(const QString &fileName,
                        const QString &tempFileName,
                        const QString &backupFileName)
        : SaverFile(fileName, tempFileName, backupFileName) { }

    bool save(const StorageRecordLink &storage) override {
        if (m_tempFile.open(QIODeviceBase::WriteOnly)) {
            QDataStream stream(&m_tempFile);
            stream.setVersion(QDataStream::Qt_6_11);

            storage.serialize(stream);

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

    bool load(StorageRecordLink &storage) override {
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

#endif // SAVERFILERECORDLINK_H
