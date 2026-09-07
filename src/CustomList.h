#ifndef CUSTOMLIST_H
#define CUSTOMLIST_H

#include <QDebug>
#include <QHash>
#include <QQueue>
#include <QReadWriteLock>

template<typename T>
class CustomListMemento;

template<typename T>
struct CustomListState {
    QHash<qsizetype, T> m_list;
    qsizetype m_id = 0;
    QQueue<qsizetype> m_emptyId;

    CustomListState() = default; // Добавлен конструктор по умолчанию

    CustomListState(const QHash<qsizetype, T>& list, qsizetype id, const QQueue<qsizetype>& emptyId)
        : m_list(list)
        , m_id(id)
        , m_emptyId(emptyId) { }
};

class MementoBase {
public:
    virtual ~MementoBase() = default;
};

template<typename T>
class CustomList;

template<typename T>
class CustomListMemento : public MementoBase {
private:
    CustomListState<T> m_state;

    explicit CustomListMemento(const CustomListState<T>& state) : m_state(state) { }

public:
    const CustomListState<T>& getState() const {
        return m_state;
    }

    friend class CustomList<T>;
};

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

    // Создать снимок состояния
    virtual std::shared_ptr<MementoBase> createMemento() const {
        QReadLocker locker(&m_lock);

        CustomListState<T> state(m_list, m_id, m_emptyId);
        return std::shared_ptr<MementoBase>(new CustomListMemento<T>(state));
    }

    // Восстановить состояние из снимка
    virtual void restoreFromMemento(const std::shared_ptr<MementoBase>& memento) {
        auto specific = std::dynamic_pointer_cast<CustomListMemento<T>>(memento);
        if (!specific) {
            throw std::runtime_error("Invalid memento type for CustomList");
        }

        QWriteLocker locker(&m_lock);
        const auto& state = specific->getState();
        m_list = state.m_list;
        m_id = state.m_id;
        m_emptyId = state.m_emptyId;
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
