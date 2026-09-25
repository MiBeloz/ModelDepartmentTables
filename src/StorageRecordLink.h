#ifndef STORAGERECORDLINK_H
#define STORAGERECORDLINK_H

#include "RecordLink.h"

class StorageRecordLink final {
public:
    StorageRecordLink() = default;

    StorageRecordLink(const StorageRecordLink& other);
    StorageRecordLink(StorageRecordLink&& other) noexcept;

    StorageRecordLink& operator =(const StorageRecordLink& other);
    StorageRecordLink& operator =(StorageRecordLink&& other) noexcept;

    bool operator ==(const StorageRecordLink& other) const;
    bool operator !=(const StorageRecordLink& other) const;

    void swap(StorageRecordLink& other);

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
    QSet<RecordLink> m_SetRecordLink;
    QSet<RecordLink> m_SetRecordLinkTmp;
    bool m_commit = true;
    mutable QReadWriteLock m_lock;

    void throwStreamError(QDataStream::Status status) const;

    void deserializeVersion(QDataStream& in) const;
    QSet<RecordLink> deserializeRecordsLinks(QDataStream& in) const;
    bool deserializeCommit(QDataStream& in) const;
};

#endif // STORAGERECORDLINK_H
