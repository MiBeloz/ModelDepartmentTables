#ifndef CACHELOCKER_H
#define CACHELOCKER_H

#include <QReadWriteLock>
#include <QHash>


class AbstractCache {
public:
    virtual ~AbstractCache() = default;
    virtual void clear() = 0;
    virtual size_t size() const = 0;
    virtual QString name() const = 0;
};

template<typename Id, typename Data>
class Cache : public AbstractCache {
public:
    Cache(const QString& name, bool uniqueKeys = true)
        : m_name(name), m_uniqueKeys(uniqueKeys) {}

    ~Cache() override = default;

    void insert(const Id& key, const Data& value) {
        QWriteLocker writeLocker(&m_lock);
        if (m_uniqueKeys) {
            m_uniqueCache.insert(key, value);
        }
        else {
            m_multiCache.insert(key, value);
        }
    }

    std::optional<Data> get(const Id& key) const {
        QReadLocker readLocker(&m_lock);
        if (m_uniqueKeys) {
            auto it = m_uniqueCache.find(key);
            if (it != m_uniqueCache.end()) {
                return it.value();
            }
        } else {
            if (m_multiCache.contains(key)) {
                return m_multiCache.value(key);
            }
        }
        return std::nullopt;
    }

    QList<Data> getAll(const Id& key) const {
        QReadLocker readLocker(&m_lock);
        if (!m_uniqueKeys) {
            return m_multiCache.values(key);
        }
        return QList<Data>();
    }

    int remove(const Id& key) {
        QWriteLocker writeLocker(&m_lock);
        int removedCount = 0;
        if (m_uniqueKeys) {
            removedCount = m_uniqueCache.remove(key);
        } else {
            removedCount = m_multiCache.remove(key);
        }
        return removedCount;
    }

    bool contains(const Id& key) const {
        QReadLocker readLocker(&m_lock);
        return m_uniqueKeys ? m_uniqueCache.contains(key) : m_multiCache.contains(key);
    }

    void clear() override {
        QWriteLocker writeLocker(&m_lock);
        if (m_uniqueKeys) {
            m_uniqueCache.clear();
        } else {
            m_multiCache.clear();
        }
    }

    size_t size() const override {
        QReadLocker readLocker(&m_lock);
        return m_uniqueKeys ? m_uniqueCache.size() : m_multiCache.size();
    }

    QString name() const override {
        QReadLocker readLocker(&m_lock);
        return m_name;
    }

protected:
    QString m_name;
    bool m_uniqueKeys;
    QHash<Id, Data> m_uniqueCache;
    QMultiHash<Id, Data> m_multiCache;
    mutable QReadWriteLock m_lock;
};

#endif // CACHELOCKER_H
