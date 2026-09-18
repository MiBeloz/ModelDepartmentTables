#ifndef STORAGEOFRECORDLINK_H
#define STORAGEOFRECORDLINK_H

#include "RecordLink.h"

class StorageOfRecordLink final {
public:
    StorageOfRecordLink() = default;
    ~StorageOfRecordLink() = default;

    StorageOfRecordLink(const StorageOfRecordLink& other) {
        QReadLocker otherLocker(&other.m_lock);
        m_storageOfRecordLink = other.m_storageOfRecordLink;
        m_storageOfRecordLinkTmp = other.m_storageOfRecordLinkTmp;
        m_commit = other.m_commit;
    }

    StorageOfRecordLink(StorageOfRecordLink&& other) {
        swap(other);

        QWriteLocker otherLocker(&other.m_lock);
        other.m_storageOfRecordLink.clear();
        other.m_storageOfRecordLinkTmp.clear();
        other.m_commit = true;
    }

    StorageOfRecordLink& operator =(const StorageOfRecordLink& other) {
        if (this == &other) {
            return *this;
        }

        QWriteLocker locker(&m_lock);
        QReadLocker otherLocker(&other.m_lock);

        m_storageOfRecordLink = other.m_storageOfRecordLink;
        m_storageOfRecordLinkTmp = other.m_storageOfRecordLinkTmp;
        m_commit = other.m_commit;
        return *this;
    }
    StorageOfRecordLink& operator =(StorageOfRecordLink&& other) {
        if (this == &other) {
            return *this;
        }

        swap(other);

        QWriteLocker otherLocker(&other.m_lock);
        other.m_storageOfRecordLink.clear();
        other.m_storageOfRecordLinkTmp.clear();
        other.m_commit = true;
        return *this;
    }

    bool operator ==(const StorageOfRecordLink& other) const {
        QReadLocker locker(&m_lock);
        QReadLocker otherLocker(&other.m_lock);
        return m_storageOfRecordLink == other.m_storageOfRecordLink &&
               m_storageOfRecordLinkTmp == other.m_storageOfRecordLinkTmp &&
               m_commit == other.m_commit;
    }

    bool operator !=(const StorageOfRecordLink& other) const {
        return !(*this == other);
    }

    void swap(StorageOfRecordLink& other) {
        if (this == &other) {
            return;
        }

        QReadWriteLock* first = &m_lock;
        QReadWriteLock* second = &other.m_lock;
        if (second < first) {
            std::swap(first, second);
        }

        QWriteLocker l1(first);
        QWriteLocker l2(second);

        m_storageOfRecordLink.swap(other.m_storageOfRecordLink);
        m_storageOfRecordLinkTmp.swap(other.m_storageOfRecordLinkTmp);
        std::swap(m_commit, other.m_commit);
    }

    void add(const RecordLink& recordLink) {
        QWriteLocker locker(&m_lock);
        m_storageOfRecordLinkTmp.insert(recordLink);
        m_commit = false;
    }

    void remove(const RecordLink& recordLink) {
        QWriteLocker locker(&m_lock);
        m_storageOfRecordLinkTmp.remove(recordLink);
        m_commit = false;
    }

    void clear() {
        QWriteLocker locker(&m_lock);
        m_storageOfRecordLinkTmp.clear();
        m_commit = false;
    }

    void reset() {
        QWriteLocker locker(&m_lock);
        m_storageOfRecordLinkTmp = m_storageOfRecordLink;
        m_commit = true;
    }

    void commit() {
        QWriteLocker locker(&m_lock);
        m_storageOfRecordLink = m_storageOfRecordLinkTmp;
        m_commit = true;
    }

    QSet<RecordLink> get() const {
        QReadLocker locker(&m_lock);
        return m_storageOfRecordLink;
    }

    QSet<RecordLink> getTmp() const {
        QReadLocker locker(&m_lock);
        return m_storageOfRecordLinkTmp;
    }

    qsizetype count() const {
        QReadLocker locker(&m_lock);
        return m_storageOfRecordLink.count();
    }

    qsizetype countTmp() const {
        QReadLocker locker(&m_lock);
        return m_storageOfRecordLinkTmp.count();
    }

    void serialize(QDataStream& out) const {
        QReadLocker locker(&m_lock);

        out << REQUIRED_SERIALIZATION_VERSION;

        out << static_cast<qint32>(m_storageOfRecordLink.size());
        for (auto it = m_storageOfRecordLink.begin(); it != m_storageOfRecordLink.end(); ++it) {
            it->serialize(out);
        }

        if (out.status() != QDataStream::Ok) {
            throwStreamError(out.status());
        }
    }

    void deserialize(QDataStream& in) {
        QWriteLocker locker(&m_lock);

        deserializeVersion(in);
        QSet<RecordLink> TmpRecordsLinks = deserializeRecordsLinks(in);

        m_storageOfRecordLink = TmpRecordsLinks;
        m_storageOfRecordLinkTmp = m_storageOfRecordLink;
        m_commit = true;
    }

private:
    QSet<RecordLink> m_storageOfRecordLink;
    QSet<RecordLink> m_storageOfRecordLinkTmp;
    bool m_commit = true;
    mutable QReadWriteLock m_lock;

    void throwStreamError(QDataStream::Status status) const {
        throw RuntimeError(
            QObject::tr("QDataStream error. Error code: '%1'.").arg(static_cast<int>(status)));
    }

    void deserializeVersion(QDataStream& in) const {
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

    QSet<RecordLink> deserializeRecordsLinks(QDataStream& in) const {
        qint32 size;
        in >> size;
        if (in.status() != QDataStream::Ok) {
            throwStreamError(in.status());
        }
        if (size < 0) {
            in.setStatus(QDataStream::Status::ReadCorruptData);
            throwStreamError(in.status());
        }

        QSet<RecordLink> recordsLinks;
        for (qint32 i = 0; i < size; ++i) {
            RecordLink recordLink(0, 0, 0, { }, { }, { }, { }, { }, { });
            recordLink.deserialize(in);
            if (in.status() != QDataStream::Ok) {
                throwStreamError(in.status());
            }
            recordsLinks.insert(recordLink);
        }
        return recordsLinks;
    }
};

#endif // STORAGEOFRECORDLINK_H
