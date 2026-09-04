#ifndef CUSTOMLIST_H
#define CUSTOMLIST_H

#include <QHash>
#include <QQueue>
#include <QReadWriteLock>

template<typename T>
class CustomList {
public:
    CustomList() = default;
    virtual ~CustomList() = default;

    virtual std::optional<qsizetype> insert(const T& data);
    virtual bool remove(const T& data);
    virtual std::optional<qsizetype> getID(const T& data) const;
    virtual std::optional<T> getValue(qsizetype id) const;
    virtual QList<T> getAllValues() const;

    virtual qsizetype size() const;
    virtual void clear();

protected:
    QHash<qsizetype, T> m_list;
    qsizetype m_id = 0;
    QQueue<qsizetype> m_emptyIDs;
    mutable QReadWriteLock m_lock;
};

template<typename T>
inline std::optional<qsizetype> CustomList<T>::insert(const T& data) {
    const QWriteLocker locker(&m_lock);
    for (auto [k, v] : m_list.asKeyValueRange()) {
        if (v == data) {
            return k;
        }
    }

    qsizetype id = -1;
    if (m_emptyIDs.isEmpty()) {
        ++m_id;
        id = m_id;
    } else {
        id = m_emptyIDs.dequeue();
    }
    m_list.insert(id, data);
    return id;
}

template<typename T>
inline bool CustomList<T>::remove(const T& data) {
    const QWriteLocker locker(&m_lock);
    if (auto id = m_list.key(data, -1); id != -1) {
        m_list.remove(id);
        m_emptyIDs.enqueue(id);
        return true;
    }
    return false;
}

template<typename T>
inline std::optional<qsizetype> CustomList<T>::getID(const T& data) const {
    const QReadLocker locker(&m_lock);
    if (auto id = m_list.key(data, -1); id != -1) {
        return id;
    }
    return std::nullopt;
}

template<typename T>
inline std::optional<T> CustomList<T>::getValue(qsizetype id) const {
    const QReadLocker locker(&m_lock);
    // if (T value = m_list.value(id, T()); value != T()) {
    //     return value;
    // }
    return std::nullopt;
}

template<typename T>
inline QList<T> CustomList<T>::getAllValues() const {
    const QReadLocker locker(&m_lock);
    return m_list.values();
}

template<typename T>
inline qsizetype CustomList<T>::size() const {
    const QReadLocker locker(&m_lock);
    return m_list.size();
}

template<typename T>
inline void CustomList<T>::clear() {
    const QWriteLocker locker(&m_lock);
    m_list.clear();
    m_emptyIDs.clear();
    m_id = 0;
}

#endif // CUSTOMLIST_H
