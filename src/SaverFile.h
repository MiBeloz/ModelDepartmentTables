#ifndef SAVERFILE_H
#define SAVERFILE_H

#include <QFile>

#include "Saver.h"

template<typename T>
class SaverFile : public Saver<T> {
public:
    SaverFile(const QString &fileName, const QString &tempFileName, const QString &backupFileName) {
        m_file.setFileName(fileName);
        m_tempFile.setFileName(tempFileName);
        m_backupFile.setFileName(backupFileName);
    }

    ~SaverFile() override = default;

    bool save() override {
        if (m_save) {
            return true;
        }

        if (m_tempFile.isOpen()) {
            m_tempFile.flush();
            m_tempFile.close();
        }

        if (!m_tempFile.exists()) {
            return false;
        }

        const QString mainName = m_file.fileName();
        const QString tempName = m_tempFile.fileName();

        if (m_file.exists()) {
            if (m_backupFile.exists() && !m_backupFile.remove()) {
                return false;
            }
            if (!m_file.rename(m_backupFile.fileName())) {
                return false;
            }
        }

        if (!m_tempFile.rename(mainName)) {
            if (m_backupFile.exists()) {
                m_backupFile.rename(mainName);
            }
            return false;
        }

        m_file.setFileName(mainName);
        m_tempFile.setFileName(tempName);

        m_save = true;
        return true;
    }

protected:
    QFile m_file;
    QFile m_tempFile;
    QFile m_backupFile;
    bool m_save = true;
};

#endif // SAVERFILE_H
