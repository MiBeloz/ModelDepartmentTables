#ifndef CUSTOMLIST_H
#define CUSTOMLIST_H

#include <QDebug>
#include <QHash>
#include <QObject>
#include <QReadWriteLock>
#include <QStack>

#include "Constants.h"
#include "Exceptions.h"

template<typename T>
class CustomList {
public:
    CustomList() = default;
    virtual ~CustomList() = default;

    CustomList(const CustomList& other);
    CustomList(CustomList&& other) noexcept;

    CustomList& operator =(const CustomList& other);
    CustomList& operator =(CustomList&& other) noexcept;

    bool operator ==(const CustomList& other) const;
    bool operator !=(const CustomList& other) const;

    virtual std::optional<qint32> insert(const T& data);
    virtual bool remove(const T& data);
    virtual std::optional<qint32> getId(const T& data) const;
    virtual std::optional<T> getValue(qint32 id) const;
    virtual QList<T> getAllValues() const;

    void reset();
    void commit();

    qsizetype size() const;
    qsizetype sizeNotCommitted() const;
    void clear();

    void serialize(QDataStream& out) const;
    void deserialize(QDataStream& in);

private:
    QHash<qint32, T> m_list;
    QHash<qint32, T> m_listTmp;
    qint32 m_id = 0;
    qint32 m_idTmp = 0;
    QStack<qint32> m_emptyId;
    QStack<qint32> m_emptyIdTmp;

    bool m_commit = true;

    mutable QReadWriteLock m_lock;

    void throwStreamError(QDataStream::Status status) const;

    void deserializeVersion(QDataStream& in) const;
    QHash<qint32, T> deserializeList(QDataStream& in) const;
    qint32 deserializeId(QDataStream& in) const;
    QStack<qint32> deserializeEmptyId(QDataStream& in) const;
};

template<typename T>
inline CustomList<T>::CustomList(const CustomList& other) {
    QReadLocker otherLocker(&other.m_lock);

    m_list = other.m_list;
    m_listTmp = other.m_listTmp;
    m_id = other.m_id;
    m_idTmp = other.m_idTmp;
    m_emptyId = other.m_emptyId;
    m_emptyIdTmp = other.m_emptyIdTmp;
    m_commit = other.m_commit;
}

template<typename T>
inline CustomList<T>::CustomList(CustomList&& other) noexcept {
    QWriteLocker otherLocker(&other.m_lock);

    m_list = std::move(other.m_list);
    m_listTmp = std::move(other.m_listTmp);
    m_id = other.m_id;
    m_idTmp = other.m_idTmp;
    m_emptyId = std::move(other.m_emptyId);
    m_emptyIdTmp = std::move(other.m_emptyIdTmp);
    m_commit = other.m_commit;

    other.m_id = 0;
    other.m_idTmp = 0;
    other.m_commit = true;
}

template<typename T>
inline CustomList<T>& CustomList<T>::operator =(const CustomList& other) {
    if (this == &other) {
        return *this;
    }

    QReadWriteLock* first = &m_lock;
    QReadWriteLock* second = &other.m_lock;
    if (second < first) {
        std::swap(first, second);
    }

    QWriteLocker l1(first);
    QWriteLocker l2(second);

    m_list = other.m_list;
    m_listTmp = other.m_listTmp;
    m_id = other.m_id;
    m_idTmp = other.m_idTmp;
    m_emptyId = other.m_emptyId;
    m_emptyIdTmp = other.m_emptyIdTmp;
    m_commit = other.m_commit;
    return *this;
}

template<typename T>
inline CustomList<T>& CustomList<T>::operator =(CustomList&& other) noexcept {
    if (this == &other) {
        return *this;
    }

    QReadWriteLock* first  = &m_lock;
    QReadWriteLock* second = &other.m_lock;
    if (second < first) std::swap(first, second);

    QWriteLocker l1(first);
    QWriteLocker l2(second);

    m_list = std::move(other.m_list);
    m_listTmp = std::move(other.m_listTmp);
    m_id = other.m_id;
    m_idTmp = other.m_idTmp;
    m_emptyId = std::move(other.m_emptyId);
    m_emptyIdTmp = std::move(other.m_emptyIdTmp);
    m_commit = other.m_commit;

    other.m_id = 0;
    other.m_idTmp = 0;
    other.m_commit = true;

    return *this;
}

template<typename T>
inline bool CustomList<T>::operator ==(const CustomList& other) const {
    if (this == &other) {
        return true;
    }

    QReadWriteLock* first  = &m_lock;
    QReadWriteLock* second = &other.m_lock;
    if (second < first) std::swap(first, second);

    QReadLocker l1(first);
    QReadLocker l2(second);

    return m_list == other.m_list && m_id == other.m_id && m_emptyId == other.m_emptyId;
}

template<typename T>
inline bool CustomList<T>::operator !=(const CustomList& other) const {
    return !(*this == other);
}

template<typename T>
inline std::optional<qint32> CustomList<T>::insert(const T& data) {
    const QWriteLocker locker(&m_lock);

    for (auto [k, v] : m_listTmp.asKeyValueRange()) {
        if (v == data) {
            return k;
        }
    }

    qint32 id = -1;
    if (m_emptyIdTmp.isEmpty()) {
        ++m_idTmp;
        id = m_idTmp;
    } else {
        id = m_emptyIdTmp.pop();
    }
    m_listTmp.insert(id, data);

    m_commit = false;
    return id;
}

template<typename T>
inline bool CustomList<T>::remove(const T& data) {
    const QWriteLocker locker(&m_lock);

    for (auto it = m_listTmp.begin(); it != m_listTmp.end(); ++it) {
        if (it.value() == data) {
            m_emptyIdTmp.push(it.key());
            m_listTmp.erase(it);

            m_commit = false;
            return true;
        }
    }
    return false;
}

template<typename T>
inline std::optional<qint32> CustomList<T>::getId(const T& data) const {
    const QReadLocker locker(&m_lock);

    if (auto id = m_list.key(data, -1); id != -1) {
        return id;
    }
    return std::nullopt;
}

template<typename T>
inline std::optional<T> CustomList<T>::getValue(qint32 id) const {
    const QReadLocker locker(&m_lock);

    if (auto it = m_list.find(id); it != m_list.end()) {
        return *it;
    }
    return std::nullopt;
}

template<typename T>
inline QList<T> CustomList<T>::getAllValues() const {
    const QReadLocker locker(&m_lock);
    return m_list.values();
}

template<typename T>
inline void CustomList<T>::reset() {
    m_listTmp = m_list;
    m_idTmp = m_id;
    m_emptyIdTmp = m_emptyId;
    m_commit = true;
}

template<typename T>
inline void CustomList<T>::commit() {
    m_list = m_listTmp;
    m_id = m_idTmp;
    m_emptyId = m_emptyIdTmp;
    m_commit = true;
}

template<typename T>
inline qsizetype CustomList<T>::size() const {
    const QReadLocker locker(&m_lock);

    return m_list.size();
}

template<typename T>
inline qsizetype CustomList<T>::sizeNotCommitted() const {
    const QReadLocker locker(&m_lock);

    return m_listTmp.size();
}

template<typename T>
inline void CustomList<T>::clear() {
    const QWriteLocker locker(&m_lock);

    m_listTmp.clear();
    m_emptyIdTmp.clear();
    m_idTmp = 0;
    m_commit = false;
}

template<typename T>
inline void CustomList<T>::serialize(QDataStream& out) const {
    QReadLocker locker(&m_lock);

    out << REQUIRED_SERIALIZATION_VERSION;
    out << static_cast<qint32>(m_list.size());
    for (auto it = m_list.begin(); it != m_list.end(); ++it) {
        out << it.key() << it.value();
    }
    out << m_id;
    out << static_cast<qint32>(m_emptyId.size());
    for (auto id : m_emptyId) {
        out << id;
    }

    if (out.status() != QDataStream::Ok) {
        throwStreamError(out.status());
    }
}

template<typename T>
inline void CustomList<T>::deserialize(QDataStream& in) {
    QWriteLocker locker(&m_lock);

    deserializeVersion(in);
    QHash<qint32, T> list = deserializeList(in);
    qint32 id = deserializeId(in);
    QStack<qint32> emptyId = deserializeEmptyId(in);

    m_list = list;
    m_listTmp = m_list;
    m_id = id;
    m_idTmp = m_id;
    m_emptyId = emptyId;
    m_emptyIdTmp = m_emptyId;
    m_commit = true;
}

template<typename T>
inline void CustomList<T>::throwStreamError(QDataStream::Status status) const {
    throw RuntimeError(
        QObject::tr("QDataStream error. Error code: '%1'.").arg(static_cast<int>(status)));
}

template<typename T>
inline void CustomList<T>::deserializeVersion(QDataStream& in) const {
    quint32 version { };
    in >> version;
    if (in.status() != QDataStream::Ok) {
        throwStreamError(in.status());
    }
    if (version != REQUIRED_SERIALIZATION_VERSION) {
        in.setStatus(QDataStream::Status::ReadCorruptData);
        throw RuntimeError(
            QObject::tr("Version error. Required version: '%1', Current version: '%2'")
                .arg(QString::number(REQUIRED_SERIALIZATION_VERSION), QString::number(version)));
    }
}

template<typename T>
inline QHash<qint32, T> CustomList<T>::deserializeList(QDataStream& in) const {
    qint32 size;
    in >> size;
    if (in.status() != QDataStream::Ok) {
        throwStreamError(in.status());
    }
    if (size < 0) {
        in.setStatus(QDataStream::Status::ReadCorruptData);
        throwStreamError(in.status());
    }

    QHash<qint32, T> list;
    for (qint32 i = 0; i < size; ++i) {
        qint32 key;
        T value;

        in >> key >> value;
        if (in.status() != QDataStream::Ok) {
            throwStreamError(in.status());
        }
        list.insert(key, value);
    }
    return list;
}

template<typename T>
inline qint32 CustomList<T>::deserializeId(QDataStream& in) const {
    qint32 id { };
    in >> id;
    if (in.status() != QDataStream::Ok) {
        throwStreamError(in.status());
    }
    if (id < 0) {
        in.setStatus(QDataStream::Status::ReadCorruptData);
        throwStreamError(in.status());
    }
    return id;
}

template<typename T>
inline QStack<qint32> CustomList<T>::deserializeEmptyId(QDataStream& in) const {
    qint32 size;
    in >> size;
    if (in.status() != QDataStream::Ok) {
        throwStreamError(in.status());
    }
    if (size < 0) {
        in.setStatus(QDataStream::Status::ReadCorruptData);
        throwStreamError(in.status());
    }

    QStack<qint32> ids;
    for (qint32 i = 0; i < size; ++i) {
        qint32 id;
        in >> id;
        if (in.status() != QDataStream::Ok) {
            throwStreamError(in.status());
        }
        ids.push(id);
    }
    return ids;
}

#endif // CUSTOMLIST_H
