#ifndef CUSTOMLIST_H
#define CUSTOMLIST_H

#include <QDebug>
#include <QHash>
#include <QQueue>
#include <QReadWriteLock>

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

        quint32 version;
        in >> version;

        m_list.clear();
        m_emptyId.clear();
        m_id = 0;

        qint64 size;
        in >> size;
        for (qint64 i = 0; i < size; ++i) {
            qint64 key;
            alignas(T) T* value = reinterpret_cast<T*>(new char[sizeof(T)]());

            in >> key >> *value;
            m_list.insert(key, *value);

            value->~T();
            delete reinterpret_cast<char*>(value);
        }

        in >> m_id;

        qint64 emptySize;
        in >> emptySize;
        for (qint64 i = 0; i < emptySize; ++i) {
            qint64 emptyId;
            in >> emptyId;
            m_emptyId.enqueue(emptyId);
        }
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
