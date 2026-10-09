#ifndef STORAGERECORDSLINKS_H
#define STORAGERECORDSLINKS_H

#include "RecordLink.h"

class StorageRecordsLinks final {
public:
    StorageRecordsLinks() = default;

    StorageRecordsLinks(const StorageRecordsLinks& other);
    StorageRecordsLinks(StorageRecordsLinks&& other) noexcept;

    StorageRecordsLinks& operator =(const StorageRecordsLinks& other);
    StorageRecordsLinks& operator =(StorageRecordsLinks&& other) noexcept;

    bool operator ==(const StorageRecordsLinks& other) const;
    bool operator !=(const StorageRecordsLinks& other) const;

    void swap(StorageRecordsLinks& other);

    void add(const RecordLink& recordLink);
    void remove(const RecordLink& recordLink);
    QSet<RecordLink> get() const;

    void reset();
    void commit();

    qsizetype count() const;
    qsizetype countCommitted() const;

    void clear();

    void serialize(QDataStream& out) const;
    void deserialize(QDataStream& in);

private:
    QSet<RecordLink> m_setRecordLink;
    QSet<RecordLink> m_setRecordLinkTmp;
    bool m_commit = true;
    // mutable QReadWriteLock m_lock;

    void throwStreamError(QDataStream::Status status) const;

    void deserializeVersion(QDataStream& in) const;
    QSet<RecordLink> deserializeRecordsLinks(QDataStream& in) const;
    bool deserializeCommit(QDataStream& in) const;
};

#endif // STORAGERECORDSLINKS_H
