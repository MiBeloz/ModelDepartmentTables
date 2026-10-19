#ifndef STORAGERECORDSLINKS_H
#define STORAGERECORDSLINKS_H

#include "RecordLink.h"
#include "Storage.h"

namespace mdt {
    namespace work {
        class StorageRecordsLinks final : public Storage {
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

            void clear();
            void reset();
            void commit();

            qsizetype count() const;
            qsizetype countCommitted() const;

            void serialize(QDataStream& out) const override;
            void deserialize(QDataStream& in) override;

        private:
            QSet<RecordLink> m_setRecordLink;
            QSet<RecordLink> m_setRecordLinkTmp;
            bool m_commit = true;

            void throwStreamError(QDataStream::Status status) const;

            void deserializeVersion(QDataStream& in) const;
            QSet<RecordLink> deserializeRecordsLinks(QDataStream& in) const;
            bool deserializeCommit(QDataStream& in) const;
        };
    } // namespace work
} // namespace mdt

#endif // STORAGERECORDSLINKS_H
