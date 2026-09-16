#ifndef CUSTOMLIST_H
#define CUSTOMLIST_H

#include <QDebug>
#include <QHash>
#include <QQueue>
#include <QReadWriteLock>

#include "Constants.h"

template<typename T>
class CustomList {
public:
    CustomList() = default;
    virtual ~CustomList() = default;

    virtual std::optional<qint64> insert(const T& data);
    virtual bool remove(const T& data);
    virtual std::optional<qint64> getId(const T& data) const;
    virtual std::optional<T> getValue(qint64 id) const;
    virtual QList<T> getAllValues() const;

    qint64 size() const;
    void clear();

    void serialize(QDataStream& out) const {
        QReadLocker locker(&m_lock);

        out << SERIALIZATION_VERSION;
        out << static_cast<qint64>(m_list.size());
        for (auto it = m_list.begin(); it != m_list.end(); ++it) {
            out << it.key() << it.value();
        }
        out << m_id;
        out << static_cast<qint64>(m_emptyId.size());
        for (auto id : m_emptyId) {
            out << id;
        }
    }

    void deserialize(QDataStream& in) {
        QWriteLocker locker(&m_lock);

        quint32 version { };
        in >> version;
        if (in.status() != QDataStream::Ok) {
            return;
        }
        if (version != CURRENT_SERIALIZATION_VERSION) {
            in.setStatus(QDataStream::Status::ReadCorruptData);
            return;
        }

        qint64 listSize;
        in >> listSize;
        if (in.status() != QDataStream::Ok) {
            return;
        }
        if (listSize < 0) {
            in.setStatus(QDataStream::Status::ReadCorruptData);
            return;
        }

        QHash<qint64, T> tmpList;
        for (qint64 i = 0; i < listSize; ++i) {
            qint64 key;
            alignas(T) T* value = reinterpret_cast<T*>(new char[sizeof(T)]());

            try {
                in >> key >> *value;
            } catch (...) {
                value->~T();
                delete reinterpret_cast<char*>(value);
                in.setStatus(QDataStream::Status::ReadCorruptData);
                return;
            }
            if (in.status() != QDataStream::Ok) {
                value->~T();
                delete reinterpret_cast<char*>(value);
                return;
            }
            try {
                tmpList.insert(key, *value);
            } catch (...) {
                value->~T();
                delete reinterpret_cast<char*>(value);
                in.setStatus(QDataStream::Status::ReadCorruptData);
                return;
            }
        }

        qint64 tmpId { };
        in >> tmpId;
        if (in.status() != QDataStream::Ok) {
            return;
        }
        if (tmpId < 0) {
            in.setStatus(QDataStream::Status::ReadCorruptData);
            return;
        }

        qint64 emptyIdSize;
        in >> emptyIdSize;
        if (in.status() != QDataStream::Ok) {
            return;
        }
        if (emptyIdSize < 0) {
            in.setStatus(QDataStream::Status::ReadCorruptData);
            return;
        }

        QQueue<qint64> tmpEmptyId;
        for (qint64 i = 0; i < emptyIdSize; ++i) {
            qint64 emptyId;
            in >> emptyId;
            if (in.status() != QDataStream::Ok) {
                return;
            }
            tmpEmptyId.enqueue(emptyId);
        }

        m_list = std::move(tmpList);
        m_emptyId = std::move(tmpEmptyId);
        m_id = tmpId;
    }

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
    QHash<qint64, T> m_list;
    qint64 m_id = 0;
    QQueue<qint64> m_emptyId;
    mutable QReadWriteLock m_lock;
    static constexpr quint32 SERIALIZATION_VERSION = 1;
};

template<typename T>
inline std::optional<qint64> CustomList<T>::insert(const T& data) {
    const QWriteLocker locker(&m_lock);

    for (auto [k, v] : m_list.asKeyValueRange()) {
        if (v == data) {
            return k;
        }
    }

    qint64 id = -1;
    if (m_emptyId.isEmpty()) {
        ++m_id;
        id = m_id;
    } else {
        id = m_emptyId.dequeue();
    }
    m_list.insert(id, data);
    return id;
}

template<typename T>
inline bool CustomList<T>::remove(const T& data) {
    const QWriteLocker locker(&m_lock);

    for (auto it = m_list.begin(); it != m_list.end(); ++it) {
        if (it.value() == data) {
            m_emptyId.enqueue(it.key());
            m_list.erase(it);
            return true;
        }
    }
    return false;
}

template<typename T>
inline std::optional<qint64> CustomList<T>::getId(const T& data) const {
    const QReadLocker locker(&m_lock);

    if (auto id = m_list.key(data, -1); id != -1) {
        return id;
    }
    return std::nullopt;
}

template<typename T>
inline std::optional<T> CustomList<T>::getValue(qint64 id) const {
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
inline qint64 CustomList<T>::size() const {
    const QReadLocker locker(&m_lock);

    return static_cast<qint64>(m_list.size());
}

template<typename T>
inline void CustomList<T>::clear() {
    const QWriteLocker locker(&m_lock);

    m_list.clear();
    m_emptyId.clear();
    m_id = 0;
}

#endif // CUSTOMLIST_H
