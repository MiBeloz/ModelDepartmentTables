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

    virtual std::optional<qsizetype> insert(const T& data);
    virtual bool remove(const T& data);
    virtual std::optional<qsizetype> getId(const T& data) const;
    virtual std::optional<T> getValue(qsizetype id) const;
    virtual QList<T> getAllValues() const;

    virtual qsizetype size() const;
    virtual void clear();

    void serialize(QDataStream& out) const {
        QReadLocker locker(&m_lock);

        out << SERIALIZATION_VERSION;
        out << static_cast<quint32>(m_list.size());
        for (auto it = m_list.begin(); it != m_list.end(); ++it) {
            out << it.key() << it.value();
        }
        out << static_cast<qint64>(m_id);
        out << static_cast<quint32>(m_emptyId.size());
        for (auto id : m_emptyId) {
            out << static_cast<qint64>(id);
        }
    }

    void deserialize(QDataStream& in) {
        QWriteLocker locker(&m_lock);

        quint32 version;
        in >> version;

        m_list.clear();
        m_emptyId.clear();
        m_id = 0;

        quint32 size;
        in >> size;
        for (quint32 i = 0; i < size; ++i) {
            qsizetype key;
            alignas(T) T* value = reinterpret_cast<T*>(new char[sizeof(T)]());

            in >> key >> *value;
            m_list.insert(key, *value);

            value->~T();
            delete reinterpret_cast<char*>(value);
        }
        qint64 id;
        in >> id;
        m_id = static_cast<qsizetype>(id);
        quint32 emptySize;
        in >> emptySize;
        for (quint32 i = 0; i < emptySize; ++i) {
            qint64 emptyId;
            in >> emptyId;
            m_emptyId.enqueue(static_cast<qsizetype>(emptyId));
        }
    }

    void printState() const {
        QReadLocker locker(&m_lock);

        qDebug() << "ID counter: " << m_id;
        qDebug() << "Free IDs:";
        for (auto id : m_emptyId) {
            qDebug() << '\t' << id;
        }
        qDebug() << "Items: ";
        for (auto it = m_list.begin(); it != m_list.end(); ++it) {
            qDebug() << '\t' << it.key() << " -> " << it.value();
        }
    }

private:
    QHash<qsizetype, T> m_list;
    qsizetype m_id = 0;
    QQueue<qsizetype> m_emptyId;
    mutable QReadWriteLock m_lock;
    static constexpr quint32 SERIALIZATION_VERSION = 1;
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
inline std::optional<qsizetype> CustomList<T>::getId(const T& data) const {
    const QReadLocker locker(&m_lock);

    if (auto id = m_list.key(data, -1); id != -1) {
        return id;
    }
    return std::nullopt;
}

template<typename T>
inline std::optional<T> CustomList<T>::getValue(qsizetype id) const {
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

#endif // CUSTOMLIST_H
