#ifndef STORAGEDATESLIST_H
#define STORAGEDATESLIST_H

#include "StorageCustomList.h"
#include "DatesList.h"

class StorageDatesList final : public StorageCustomList<qint32, DatesList> {
public:
    StorageDatesList() : StorageCustomList() { }

    virtual ~StorageDatesList() = default;

    std::optional<qint32> add(const QString& date) {
        return m_list->insert(date);
    }

    bool remove(const QString& date) {
        return m_list->remove(date);
    }

    std::optional<qint32> findId(const QString& date) const {
        return m_list->getId(date);
    }

    std::optional<QString> findStrValue(qint32 id) const {
        return m_list->getStrValue(id);
    }

    DatesListError::ErrorType lastError() const {
        return m_list->lastError();
    }

    void setDateFormat(const QString& format) {
        m_list->setDateFormat(format);
    }

    QString getDateFormat() const {
        return m_list->getDateFormat();
    }
};

#endif // STORAGEDATESLIST_H
