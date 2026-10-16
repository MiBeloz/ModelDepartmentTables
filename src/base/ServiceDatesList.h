#ifndef SERVICEDATESLIST_H
#define SERVICEDATESLIST_H

#include "DatesList.h"
#include "ServiceCustomList.h"

class ServiceDatesList final : public ServiceCustomList<qint32, DatesList> {
public:
    using ServiceCustomList<qint32, DatesList>::add;
    using ServiceCustomList<qint32, DatesList>::remove;
    using ServiceCustomList<qint32, DatesList>::findId;

    ServiceDatesList();

    virtual ~ServiceDatesList();

    std::optional<qint32> add(const QString& date);
    bool remove(const QString& date);
    std::optional<qint32> findId(const QString& date) const;
    std::optional<QString> findStrValue(qint32 id) const;
    QList<QString> findAllStrValues() const;

    DatesList::ErrorType lastError() const;
};

#endif // ServiceDatesList_H
