#ifndef SERVICECUSTOMLIST_H
#define SERVICECUSTOMLIST_H

#include "CustomList.h"

template<typename ValueType, typename CustomListType = CustomList<ValueType>>
class ServiceCustomList {
public:
    ServiceCustomList() {
        m_list = std::make_unique<CustomListType>();
    }

    virtual ~ServiceCustomList() { }

    ServiceCustomList(ServiceCustomList&) = delete;
    ServiceCustomList& operator=(ServiceCustomList&) = delete;

    virtual std::optional<qint32> add(const ValueType& value) {
        return m_list->insert(value);
    }

    virtual bool remove(const ValueType& value) {
        return m_list->remove(value);
    }

    virtual std::optional<qint32> findId(const ValueType& value) const {
        return m_list->getId(value);
    }

    virtual std::optional<ValueType> findValue(qint32 id) const {
        return m_list->getValue(id);
    }

    virtual QList<ValueType> findAllValues() const {
        return m_list->getAllValues();
    }

    void reset() {
        m_list->reset();
    }

    void commit() {
        m_list->commit();
    }

    qsizetype count() const {
        return m_list->size();
    }

    qsizetype countNotCommitted() const {
        return m_list->sizeNotCommitted();
    }

    void clear() {
        return m_list->clear();
    }

    void serialize(QDataStream& out) const {
        m_list->serialize(out);
    }
    void deserialize(QDataStream& in) {
        m_list->deserialize(in);
    }

protected:
    std::unique_ptr<CustomListType> m_list;
};

#endif // SERVICECUSTOMLIST_H
