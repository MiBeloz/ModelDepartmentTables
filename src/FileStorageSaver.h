#ifndef FILESTORAGESAVER_H
#define FILESTORAGESAVER_H

#include <QFile>
#include <QFileInfo>

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

    virtual ~FileStorageSaver() = default;

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

    // bool commit() override {
    //     if (m_saved) {
    //         if (m_file.exists()) {
    //             if (m_backupFile.exists()) {
    //                 if (m_backupFile.remove()) {
    //                     if (!m_file.rename(m_backupFile.fileName())) {
    //                         return false;
    //                     }
    //                 }
    //             } else {
    //                 if (!m_file.rename(m_backupFile.fileName())) {
    //                     return false;
    //                 }
    //             }
    //             if (m_tempFile.rename(m_file.fileName())) {
    //                 m_saved = false;
    //                 return true;
    //             }
    //         } else {
    //             if (m_tempFile.rename(m_file.fileName())) {
    //                 m_saved = false;
    //                 return true;
    //             }
    //         }
    //         return false;
    //     }
    //     return true;
    // }

    bool commit() override {
        if (!m_saved)
            return true;

        // 1. Если рабочий файл существует — уводим его в бэкап
        QString nameOfFile = m_file.fileName();
        if (m_file.exists()) {
            if (m_backupFile.exists() && !m_backupFile.remove())
                return false;

            if (!m_file.rename(m_backupFile.fileName()))
                return false;
        }

        // 2. Пытаемся поставить temp на место рабочего файла
        if (!m_tempFile.rename(nameOfFile)) {
            // откат: вернуть бэкап обратно
            if (m_backupFile.exists())
                m_backupFile.rename(nameOfFile);
        return false;
        }

        m_saved = false;
        return true;
    }

private:
    QFile m_file;
    QFile m_tempFile;
    QFile m_backupFile;
    bool m_saved;
};

#endif // FILESTORAGESAVER_H
