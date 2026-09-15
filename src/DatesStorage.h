#ifndef DATESSTORAGE_H
#define DATESSTORAGE_H

#include "CustomStorage.h"
#include "DatesList.h"

class DatesStorage final : public CustomStorage<qint64, DatesList> {
public:
    DatesStorage() : CustomStorage() { }

    virtual ~DatesStorage() = default;

    std::optional<qint64> add(const QString& date) {
        return m_list->insert(date);
    }

    bool remove(const QString& date) {
        return m_list->remove(date);
    }

    std::optional<qint64> findId(const QString& date) const {
        return m_list->getId(date);
    }

    std::optional<QString> findStrValue(qint64 id) const {
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

#endif // DATESSTORAGE_H
