#ifndef SERVICEDATESLIST_H
#define SERVICEDATESLIST_H

#include "DatesList.h"
#include "ServiceCustomList.h"

class ServiceDatesList final : public ServiceCustomList<qint32, DatesList> {
public:
    ServiceDatesList() : ServiceCustomList() { }

    virtual ~ServiceDatesList() = default;

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

    DatesList::ErrorType lastError() const {
        return m_list->lastError();
    }
};

#endif // ServiceDatesList_H
