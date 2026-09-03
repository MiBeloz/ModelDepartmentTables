#ifndef ILIST_H
#define ILIST_H

#include <QHash>
#include <QQueue>
#include <QReadWriteLock>

template<typename T>
class IList {
public:
    virtual ~IList() { }

    virtual std::optional<qsizetype> insert(const T& data) = 0;
    virtual bool remove(const T& data) = 0;
    virtual std::optional<qsizetype> getID(const T& data) const = 0;
    virtual std::optional<T> getValue(qsizetype id) const = 0;

    qsizetype size() const {
        const QReadLocker locker(&m_lock);
        return m_list.size();
    }

    void clear() {
        const QWriteLocker locker(&m_lock);
        m_list.clear();
        m_emptyIDs.clear();
        m_id = 0;
    }

protected:
    QHash<qsizetype, T> m_list;
    qsizetype m_id = 0;
    QQueue<qsizetype> m_emptyIDs;
    mutable QReadWriteLock m_lock;
};

#endif // ILIST_H
