#ifndef CACHELOCKER_H
#define CACHELOCKER_H

#include <QReadWriteLock>
#include <QHash>



#include <QDir>

class AbstractCache {
public:
    virtual ~AbstractCache() = default;
    virtual void clear() = 0;
    virtual qsizetype size() const = 0;
    virtual QString name() const = 0;
};

template<typename Data>
class CacheStorage : public AbstractCache {
public:
    using DataList = QList<Data>;
    using IdList = QList<qsizetype>;

    CacheStorage(const QString& name, bool uniqueKeys = true)
        : m_name(name), m_uniqueKeys(uniqueKeys) {}

    ~CacheStorage() override = default;

    bool insert(qsizetype id, const Data& data) {
        if (m_uniqueKeys) {
            if (m_uniqueCache.contains(id)) {
                return false;
            }
            m_uniqueCache.insert(id, data);
            return true;
        }
        else {
            m_multiCache.insert(id, data);
            return true;
        }
    }

    qsizetype insert(const Data& data) {
        qsizetype newId = generateId();
        insert(newId, data);
        return newId;
    }

    DataList get(qsizetype id) const {
        if (m_uniqueKeys) {
            auto it = m_uniqueCache.find(id);
            if (it != m_uniqueCache.end()) {
                return DataList{it.value()};
            }
        } else {
            return m_multiCache.values(id);
        }
        return DataList();
    }

    IdList getIdsByData(const Data& data) const {
        if (m_uniqueKeys) {
            return IdList{m_uniqueCache.key(data)};
        } else {
            IdList idList;
            for (auto it = m_multiCache.begin(); it != m_multiCache.end(); ++it) {
                if (it.value() == data) {
                    idList << it.key();
                }
            }
            return idList;
        }
    }

    void remove(qsizetype id) {
        if (m_uniqueKeys) {
            m_uniqueCache.remove(id);
        } else {
            m_multiCache.remove(id);
        }
    }

    bool contains(qsizetype id) const {
        return m_uniqueKeys ? m_uniqueCache.contains(id) : m_multiCache.contains(id);
    }

    bool containsIdByData(const Data& data) const {
        return m_uniqueKeys ? m_uniqueCache.values().contains(data) : m_multiCache.values().contains(data);
    }

    void clear() override {
        if (m_uniqueKeys) {
            m_uniqueCache.clear();
        } else {
            m_multiCache.clear();
        }
    }

    qsizetype size() const override {
        return m_uniqueKeys ? m_uniqueCache.size() : m_multiCache.size();
    }

    QString name() const override {
        return m_name;
    }

protected:
    QString m_name;
    bool m_uniqueKeys;
    QHash<qsizetype, Data> m_uniqueCache;
    QMultiHash<qsizetype, Data> m_multiCache;

    qsizetype generateId() const {
        static std::atomic<qsizetype> counter{0};
        return counter.fetch_add(1, std::memory_order_relaxed);
    }
};


template<typename Data>
class ThreadSafeCache : public CacheStorage<Data>  {
public:
    using Storage = CacheStorage<Data>;
    using typename Storage::DataList;
    using typename Storage::IdList;

    ThreadSafeCache(const QString& name, bool uniqueKeys = true)
        : Storage(name, uniqueKeys) {}

    ~ThreadSafeCache() override = default;

    bool insert(qsizetype id, const Data& data) {
        QWriteLocker writeLocker(&m_lock);
        return Storage::insert(id, data);
    }

    qsizetype insert(const Data& data) {
        QWriteLocker locker(&m_lock);
        return Storage::insert(data);
    }

    DataList get(qsizetype id) const {
        QReadLocker readLocker(&m_lock);
        return Storage::get(id);
    }

    IdList getIdsByData(const Data& data) const {
        QReadLocker readLocker(&m_lock);
        return Storage::getIdsByData(data);
    }

    void remove(qsizetype id) {
        QWriteLocker writeLocker(&m_lock);
        Storage::remove(id);
    }

    bool contains(qsizetype id) const {
        QReadLocker readLocker(&m_lock);
        return Storage::contains(id);
    }

    bool containsIdByData(const Data& data) const {
        QReadLocker readLocker(&m_lock);
        return Storage::containsIdByData(data);
    }

    void clear() override {
        QWriteLocker writeLocker(&m_lock);
        Storage::clear();
    }

    qsizetype size() const override {
        QReadLocker readLocker(&m_lock);
        return Storage::size();
    }

    QString name() const override {
        return Storage::name();
    }

    bool writeToFile(const QString &filename) const {
        QDir appDir;
        if (!appDir.cd("cache")) {
            if (!appDir.mkdir("cache")) {
                qWarning("Cannot create the cache directory");
                return false;
            }
            if (!appDir.cd("cache")) {
                qWarning("Cannot find the cache directory");
                return false;
            }
        }

        QFile file(appDir.filePath(filename));
        if (file.open(QIODevice::WriteOnly)) {
            QDataStream out(&file);
            out.setVersion(QDataStream::Version::Qt_6_7);

            QReadLocker readLocker(&m_lock);
            if (Storage::m_uniqueKeys) {
                for (auto it = Storage::m_uniqueCache.begin(); it != Storage::m_uniqueCache.end(); ++it) {
                    out << it.key() << it.value();
                }
            } else {
                for (auto it = Storage::m_multiCache.begin(); it != Storage::m_multiCache.end(); ++it) {
                    out << it.key() << it.value();
                }
            }
            file.close();
            return true;
        }
        else {
            QFileInfo fileInfo(file.fileName());
            qWarning() << "Cannot open the file" << fileInfo.absoluteFilePath();
            return false;
        }
    }

    bool readFromFile(const QString &fileName) {
        QDir appDir;
        if (!appDir.cd("cache")) {
            if (!appDir.mkdir("cache")) {
                qWarning("Cannot create the cache directory");
                return false;
            }
            if (!appDir.cd("cache")) {
                qWarning("Cannot find the cache directory");
                return false;
            }
        }

        QFile file(appDir.filePath(fileName));
        if (file.open(QIODevice::ReadOnly)) {
            QDataStream in(&file);
            in.setVersion(QDataStream::Version::Qt_6_7);

            QWriteLocker writeLocker(&m_lock);
            while (!in.atEnd()) {
                qsizetype key;
                Data value;
                in >> key >> value;
                if (Storage::m_uniqueKeys) {
                    Storage::m_uniqueCache.insert(key, value);
                } else {
                    Storage::m_multiCache.insert(key, value);
                }
            }
            file.close();
            return true;
        }
        else {
            QFileInfo fileInfo(file.fileName());
            qWarning() << "Cannot open the file" << fileInfo.absoluteFilePath();
            return false;
        }
    }

private:
    mutable QReadWriteLock m_lock;
};

template<typename Data>
class CacheSerializer {
public:
    static bool saveToFile(const ThreadSafeCache<Data>& cache, const QString& filename) {
        return false;
    }

    static bool loadFromFile(ThreadSafeCache<Data>& cache, const QString& filename) {
        return false;
    }
};

#endif // CACHELOCKER_H
