#ifndef IFILESAVER_H
#define IFILESAVER_H

#include <QFile>

#include "ISaver.h"

template<typename T>
class IFileSaver : public ISaver<T> {
public:
    IFileSaver(const QString &fileName, const QString &tempFileName, const QString &backupFileName) {
        m_file.setFileName(fileName);
        m_tempFile.setFileName(tempFileName);
        m_backupFile.setFileName(backupFileName);
        m_saved = false;
    }

    ~IFileSaver() override = default;

    bool commit() override {
        if (!m_saved) {
            return true;
        }

        if (!m_tempFile.exists()) {
            return false;
        }

        QString nameOfFile = m_file.fileName();
        if (m_file.exists()) {
            if (m_backupFile.exists() && !m_backupFile.remove()) {
                return false;
            }
            if (!m_file.rename(m_backupFile.fileName())) {
                return false;
            }
        }

        if (!m_tempFile.rename(nameOfFile)) {
            if (m_backupFile.exists()) {
                m_backupFile.rename(nameOfFile);
            }
            return false;
        }

        m_saved = false;
        return true;
    }

protected:
    QFile m_file;
    QFile m_tempFile;
    QFile m_backupFile;
    bool m_saved;
};

#endif // IFILESAVER_H
