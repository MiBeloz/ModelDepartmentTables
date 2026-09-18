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
    CustomList(CustomList&& other);

    CustomList& operator =(const CustomList& other);
    CustomList& operator =(CustomList&& other);

    bool operator ==(const CustomList& other) const;
    bool operator !=(const CustomList& other) const;

    virtual std::optional<qint32> insert(const T& data);
    virtual bool remove(const T& data);
    virtual std::optional<qint32> getId(const T& data) const;
    virtual std::optional<T> getValue(qint32 id) const;
    virtual QList<T> getAllValues() const;

    qsizetype size() const;
    void clear();

    void serialize(QDataStream& out) const;
    void deserialize(QDataStream& in);

    void printState() const {
        QReadLocker locker(&m_lock);

        qDebug() << "\tID counter: " << m_id;
        qDebug() << "\tFree IDs:";
        for (auto id : m_emptyId) {
            qDebug() << "\t\t" << id;
        }
        qDebug() << "\tItems: ";
        for (auto it = m_list.begin(); it != m_list.end(); ++it) {
            qDebug() << "\t\t" << it.key() << " -> " << it.value();
        }
        qDebug() << Qt::endl;
    }

private:
    QHash<qint32, T> m_list;
    qint32 m_id = 0;
    QStack<qint32> m_emptyId;
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
    m_id = other.m_id;
    m_emptyId = other.m_emptyId;
}

template<typename T>
inline CustomList<T>::CustomList(CustomList&& other) {
    QWriteLocker otherLocker(&other.m_lock);

    m_list = std::move(other.m_list);
    m_id = other.m_id;
    other.m_id = 0;
    m_emptyId = std::move(other.m_emptyId);
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
    m_id = other.m_id;
    m_emptyId = other.m_emptyId;
    return *this;
}

template<typename T>
inline CustomList<T>& CustomList<T>::operator =(CustomList&& other) {
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

    m_list = std::move(other.m_list);
    m_id = other.m_id;
    other.m_id = 0;
    m_emptyId = std::move(other.m_emptyId);
    return *this;
}

template<typename T>
inline bool CustomList<T>::operator ==(const CustomList& other) const {
    QReadLocker locker(&m_lock);
    QReadLocker otherLocker(&other.m_lock);

    return m_list == other.m_list && m_id == other.m_id && m_emptyId == other.m_emptyId;
}

template<typename T>
inline bool CustomList<T>::operator !=(const CustomList& other) const {
    QReadLocker locker(&m_lock);
    QReadLocker otherLocker(&other.m_lock);

    return !(*this == other);
}

template<typename T>
inline std::optional<qint32> CustomList<T>::insert(const T& data) {
    const QWriteLocker locker(&m_lock);

    for (auto [k, v] : m_list.asKeyValueRange()) {
        if (v == data) {
            return k;
        }
    }

    qint32 id = -1;
    if (m_emptyId.isEmpty()) {
        ++m_id;
        id = m_id;
    } else {
        id = m_emptyId.pop();
    }
    m_list.insert(id, data);
    return id;
}

template<typename T>
inline bool CustomList<T>::remove(const T& data) {
    const QWriteLocker locker(&m_lock);

    for (auto it = m_list.begin(); it != m_list.end(); ++it) {
        if (it.value() == data) {
            m_emptyId.push(it.key());
            m_list.erase(it);
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
inline qsizetype CustomList<T>::size() const {
    const QReadLocker locker(&m_lock);

    return m_list.size();
}

template<typename T>
inline void CustomList<T>::clear() {
    const QWriteLocker locker(&m_lock);

    m_list.clear();
    m_emptyId.clear();
    m_id = 0;
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
    QHash<qint32, T> tmpList = deserializeList(in);
    qint32 tmpId = deserializeId(in);
    QStack<qint32> tmpEmptyId = deserializeEmptyId(in);

    m_list = tmpList;
    m_id = tmpId;
    m_emptyId = tmpEmptyId;
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
        alignas(T) T* value = reinterpret_cast<T*>(new char[sizeof(T)]());

        in >> key >> *value;
        if (in.status() != QDataStream::Ok) {
            value->~T();
            delete reinterpret_cast<char*>(value);
            throwStreamError(in.status());
        }
        list.insert(key, *value);

        value->~T();
        delete reinterpret_cast<char*>(value);
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
