#ifndef FILESTORAGESAVER_H
#define FILESTORAGESAVER_H

#include <QFile>

#include "IStorageSaver.h"

class FileStorageSaver final : public IStorageSaver {
public:
    FileStorageSaver(const QString &fileName,
                     const QString &tempFileName,
                     const QString &backupFileName) {
        m_file.setFileName(fileName);
        m_tempFile.setFileName(tempFileName);
        m_backupFile.setFileName(backupFileName);
        m_saved = false;
    }

    virtual ~FileStorageSaver() {
        if (m_saved) {
            if (m_file.exists()) {
                if (m_backupFile.exists()) {
                    if (m_backupFile.remove()) {
                        if (!m_file.copy(m_backupFile.fileName())) {
                            // false TODO
                        }
                    }
                } else {
                    if (!m_file.copy(m_backupFile.fileName())) {
                        // false TODO
                    }
                }
                if (m_file.remove()) {
                    if (m_tempFile.rename(m_file.fileName())) {
                        // true TODO
                    }
                }
            } else {
                if (m_tempFile.rename(m_file.fileName())) {
                    // true TODO
                }
            }
            // false TODO
        }
    }

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

private:
    QFile m_file;
    QFile m_tempFile;
    QFile m_backupFile;
    bool m_saved;
};

#endif // FILESTORAGESAVER_H
