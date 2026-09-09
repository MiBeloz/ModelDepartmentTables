#ifndef DATESSTORAGE_H
#define DATESSTORAGE_H

#include "CustomStorage.h"
#include "DatesList.h"

class DatesStorage final : public CustomStorage<qsizetype, DatesList> {
public:
    DatesStorage() : CustomStorage() { }

    virtual ~DatesStorage() = default;

    std::optional<qsizetype> add(const QString& date) {
        return m_list->insert(date);
    }

    bool remove(const QString& date) {
        return m_list->remove(date);
    }

    std::optional<qsizetype> findId(const QString& date) const {
        return m_list->getId(date);
    }

    std::optional<QString> findStrValue(qsizetype id) const {
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
