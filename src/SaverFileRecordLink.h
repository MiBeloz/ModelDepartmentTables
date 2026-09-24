#ifndef SAVERFILERECORDLINK_H
#define SAVERFILERECORDLINK_H

#include <QFile>

#include "RecordLink.h"
#include "SaverFile.h"

class SaverFileRecordLink final : public SaverFile<RecordLink> {
public:
    SaverFileRecordLink(const QString &fileName,
                        const QString &tempFileName,
                        const QString &backupFileName)
        : SaverFile(fileName, tempFileName, backupFileName) { }

    bool save(const RecordLink &recordLink) override {
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

    bool load(RecordLink &storage) override {
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

#endif // SAVERFILERECORDLINK_H
