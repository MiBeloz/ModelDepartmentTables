#ifndef FILEREPOSITORYSAVER_H
#define FILEREPOSITORYSAVER_H

#include <QFile>

#include "IRepositorySaver.h"

class FileRepositorySaver final : public IRepositorySaver {
public:
    FileRepositorySaver(const QString &fileName) {
        m_file.setFileName(fileName);
    }

    ~FileRepositorySaver() = default;

    void save(const RepositoryService &repository) override {
        if (m_file.open(QIODeviceBase::WriteOnly)) {
            QDataStream stream(&m_file);
            stream.setVersion(QDataStream::Qt_6_11);
            // repository.dates()
            // repository.serialize(stream);
        }
        m_file.close();
    }

private:
    QFile m_file;
};

#endif // FILEREPOSITORYSAVER_H
