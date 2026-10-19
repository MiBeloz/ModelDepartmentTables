#include "StorageRecordsLinks.h"

#include "Exceptions.h"

namespace mdt {
    namespace work {
        StorageRecordsLinks::StorageRecordsLinks(const StorageRecordsLinks &other) {
            m_setRecordLink = other.m_setRecordLink;
            m_setRecordLinkTmp = other.m_setRecordLinkTmp;
            m_commit = other.m_commit;
        }

        StorageRecordsLinks::StorageRecordsLinks(StorageRecordsLinks &&other) noexcept {
            m_setRecordLink = std::move(other.m_setRecordLink);
            m_setRecordLinkTmp = std::move(other.m_setRecordLinkTmp);
            m_commit = other.m_commit;
            other.m_commit = true;
        }

        StorageRecordsLinks &StorageRecordsLinks::operator =(const StorageRecordsLinks &other) {
            if (this == &other) {
                return *this;
            }

            m_setRecordLink = other.m_setRecordLink;
            m_setRecordLinkTmp = other.m_setRecordLinkTmp;
            m_commit = other.m_commit;
            return *this;
        }

        StorageRecordsLinks &StorageRecordsLinks::operator =(StorageRecordsLinks &&other) noexcept {
            if (this == &other) {
                return *this;
            }

            m_setRecordLink = std::move(other.m_setRecordLink);
            m_setRecordLinkTmp = std::move(other.m_setRecordLinkTmp);
            m_commit = other.m_commit;
            other.m_commit = true;
            return *this;
        }

        bool StorageRecordsLinks::operator ==(const StorageRecordsLinks &other) const {
            if (this == &other) {
                return true;
            }

            return m_setRecordLink == other.m_setRecordLink &&
                   m_setRecordLinkTmp == other.m_setRecordLinkTmp && m_commit == other.m_commit;
        }

        bool StorageRecordsLinks::operator !=(const StorageRecordsLinks &other) const {
            return !(*this == other);
        }

        void StorageRecordsLinks::swap(StorageRecordsLinks &other) {
            if (this == &other) {
                return;
            }

            m_setRecordLink.swap(other.m_setRecordLink);
            m_setRecordLinkTmp.swap(other.m_setRecordLinkTmp);
            std::swap(m_commit, other.m_commit);
        }

        void StorageRecordsLinks::add(const RecordLink &recordLink) {
            m_setRecordLinkTmp.insert(recordLink);
            m_commit = false;
        }

        void StorageRecordsLinks::remove(const RecordLink &recordLink) {
            m_setRecordLinkTmp.remove(recordLink);
            m_commit = false;
        }

        QSet<RecordLink> StorageRecordsLinks::get() const {
            return m_setRecordLinkTmp;
        }

        void StorageRecordsLinks::clear() {
            m_setRecordLinkTmp.clear();
            m_commit = false;
        }

        void StorageRecordsLinks::reset() {
            m_setRecordLinkTmp = m_setRecordLink;
            m_commit = true;
        }

        void StorageRecordsLinks::commit() {
            m_setRecordLink = m_setRecordLinkTmp;
            m_commit = true;
        }

        qsizetype StorageRecordsLinks::count() const {
            return m_setRecordLinkTmp.count();
        }

        qsizetype StorageRecordsLinks::countCommitted() const {
            return m_setRecordLink.count();
        }

        void StorageRecordsLinks::serialize(QDataStream &out) const {
            out << out.version();

            out << static_cast<qint32>(m_setRecordLink.size());
            for (auto &v : std::as_const(m_setRecordLink)) {
                v.serialize(out);
            }

            out << static_cast<qint32>(m_setRecordLinkTmp.size());
            for (auto &v : std::as_const(m_setRecordLinkTmp)) {
                v.serialize(out);
            }

            out << m_commit;

            if (out.status() != QDataStream::Ok) {
                throwStreamError(out.status());
            }
        }

        void StorageRecordsLinks::deserialize(QDataStream &in) {
            deserializeVersion(in);
            QSet<RecordLink> setRecordLink = deserializeRecordsLinks(in);
            QSet<RecordLink> setRecordLinkTmp = deserializeRecordsLinks(in);
            bool commit = deserializeCommit(in);

            m_setRecordLink = setRecordLink;
            m_setRecordLinkTmp = setRecordLinkTmp;
            m_commit = commit;
        }

        void StorageRecordsLinks::throwStreamError(QDataStream::Status status) const {
            throw RuntimeError(
                QObject::tr("QDataStream error. Error code: '%1'.").arg(static_cast<int>(status)));
        }

        void StorageRecordsLinks::deserializeVersion(QDataStream &in) const {
            int version { };
            in >> version;
            if (in.status() != QDataStream::Ok) {
                throwStreamError(in.status());
            }
            if (version != in.version()) {
                in.setStatus(QDataStream::Status::ReadCorruptData);
                throw RuntimeError(
                    QObject::tr("Version error. Required version: '%1', Current version: '%2'")
                        .arg(QString::number(in.version()), QString::number(version)));
            }
        }

        QSet<RecordLink> StorageRecordsLinks::deserializeRecordsLinks(QDataStream &in) const {
            qint32 size;
            in >> size;
            if (in.status() != QDataStream::Ok) {
                throwStreamError(in.status());
            }
            if (size < 0) {
                in.setStatus(QDataStream::Status::ReadCorruptData);
                throwStreamError(in.status());
            }

            QSet<RecordLink> setRecordLink;
            for (qint32 i = 0; i < size; ++i) {
                RecordLink recordLink(RecordLink::Null);
                recordLink.deserialize(in);
                if (in.status() != QDataStream::Ok) {
                    throwStreamError(in.status());
                }
                setRecordLink.insert(recordLink);
            }
            return setRecordLink;
        }

        bool StorageRecordsLinks::deserializeCommit(QDataStream &in) const {
            bool commit { };
            in >> commit;
            if (in.status() != QDataStream::Ok) {
                throwStreamError(in.status());
            }
            return commit;
        }
    } // namespace work
} // namespace mdt
