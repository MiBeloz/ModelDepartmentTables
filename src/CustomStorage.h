#ifndef CUSTOMSTORAGE_H
#define CUSTOMSTORAGE_H

#include "CustomList.h"

template<typename ValueType, typename CustomListType = CustomList<ValueType>>
class CustomStorage {
public:
    CustomStorage() {
        m_list = std::make_unique<CustomListType>();
    }

    virtual ~CustomStorage() { }

    virtual std::optional<qint64> add(const ValueType& value) {
        return m_list->insert(value);
    }

    virtual bool remove(const ValueType& value) {
        return m_list->remove(value);
    }

    virtual std::optional<qint64> findId(const ValueType& value) const {
        return m_list->getId(value);
    }

    virtual std::optional<ValueType> findValue(qint64 id) const {
        return m_list->getValue(id);
    }

    virtual QList<ValueType> findAllValues() const {
        return m_list->getAllValues();
    }

    qint64 count() const {
        return m_list->size();
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

    void printState() const {
        m_list->printState();
    }

protected:
    std::unique_ptr<CustomListType> m_list;
};

#endif // CUSTOMSTORAGE_H
