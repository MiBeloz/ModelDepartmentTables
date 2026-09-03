#ifndef CUSTOMLIST_H
#define CUSTOMLIST_H

#include "IList.h"

template<typename T>
class CustomList final : public IList<T> {
public:
    CustomList() { }

    std::optional<qsizetype> insert(const T& data) override;
    bool remove(const T& data) override;
    std::optional<qsizetype> getID(const T& data) const override;
    std::optional<T> getValue(qsizetype id) const override;
};

template<typename T>
inline std::optional<qsizetype> CustomList<T>::insert(const T& data) {
    const QWriteLocker locker(&m_lock);
    if (auto id = m_list.key(data, -1); id != -1) {
        return id;
    }

    qsizetype id = -1;
    if (m_emptyIDs.isEmpty()) {
        ++m_id;
        id = m_id;
    } else {
        id = m_emptyIDs.first();
        m_emptyIDs.removeFirst();
    }
    m_list.insert(id, data);
    return id;
}

template<typename T>
inline bool CustomList<T>::remove(const T& data) {
    const QWriteLocker locker(&m_lock);
    qsizetype id;
    if (id = m_list.key(data, -1); id != -1) {
        m_customData.remove(id);
        m_emptyIDs.append(id);
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
    if (T value = m_list.value(id, T()); value != T()) {
        return value;
    }
    return std::nullopt;
}

#endif // CUSTOMLIST_H
