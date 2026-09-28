#include "StorageRecordsLinks.h"

#include "Exceptions.h"

StorageRecordsLinks::StorageRecordsLinks(const StorageRecordsLinks &other) {
    const QReadLocker otherLocker(&other.m_lock);
    m_SetRecordLink = other.m_SetRecordLink;
    m_SetRecordLinkTmp = other.m_SetRecordLinkTmp;
    m_commit = other.m_commit;
}

StorageRecordsLinks::StorageRecordsLinks(StorageRecordsLinks &&other) noexcept {
    const QWriteLocker otherLocker(&other.m_lock);

    m_SetRecordLink = std::move(other.m_SetRecordLink);
    m_SetRecordLinkTmp = std::move(other.m_SetRecordLinkTmp);
    m_commit = other.m_commit;
    other.m_commit = true;
}

StorageRecordsLinks &StorageRecordsLinks::operator =(const StorageRecordsLinks &other) {
    if (this == &other) {
        return *this;
    }

    QReadWriteLock *first = &m_lock;
    QReadWriteLock *second = &other.m_lock;
    if (second < first) {
        std::swap(first, second);
    }

    const QWriteLocker l1(first);
    const QWriteLocker l2(second);

    m_SetRecordLink = other.m_SetRecordLink;
    m_SetRecordLinkTmp = other.m_SetRecordLinkTmp;
    m_commit = other.m_commit;
    return *this;
}

StorageRecordsLinks &StorageRecordsLinks::operator =(StorageRecordsLinks &&other) noexcept {
    if (this == &other) {
        return *this;
    }

    QReadWriteLock *first = &m_lock;
    QReadWriteLock *second = &other.m_lock;
    if (second < first) {
        std::swap(first, second);
    }

    const QWriteLocker l1(first);
    const QWriteLocker l2(second);

    m_SetRecordLink = std::move(other.m_SetRecordLink);
    m_SetRecordLinkTmp = std::move(other.m_SetRecordLinkTmp);
    m_commit = other.m_commit;
    other.m_commit = true;
    return *this;
}

bool StorageRecordsLinks::operator ==(const StorageRecordsLinks &other) const {
    if (this == &other) {
        return true;
    }

    QReadWriteLock *first = &m_lock;
    QReadWriteLock *second = &other.m_lock;
    if (second < first) {
        std::swap(first, second);
    }

    const QReadLocker l1(first);
    const QReadLocker l2(second);

    return m_SetRecordLink == other.m_SetRecordLink &&
           m_SetRecordLinkTmp == other.m_SetRecordLinkTmp && m_commit == other.m_commit;
}

bool StorageRecordsLinks::operator !=(const StorageRecordsLinks &other) const {
    return !(*this == other);
}

void StorageRecordsLinks::swap(StorageRecordsLinks &other) {
    if (this == &other) {
        return;
    }

    QReadWriteLock *first = &m_lock;
    QReadWriteLock *second = &other.m_lock;
    if (second < first) {
        std::swap(first, second);
    }

    const QWriteLocker l1(first);
    const QWriteLocker l2(second);

    m_SetRecordLink.swap(other.m_SetRecordLink);
    m_SetRecordLinkTmp.swap(other.m_SetRecordLinkTmp);
    std::swap(m_commit, other.m_commit);
}

void StorageRecordsLinks::add(const RecordLink &recordLink) {
    const QWriteLocker locker(&m_lock);
    m_SetRecordLinkTmp.insert(recordLink);
    m_commit = false;
}

void StorageRecordsLinks::remove(const RecordLink &recordLink) {
    const QWriteLocker locker(&m_lock);
    m_SetRecordLinkTmp.remove(recordLink);
    m_commit = false;
}

QSet<RecordLink> StorageRecordsLinks::get() const {
    const QReadLocker locker(&m_lock);
    return m_SetRecordLinkTmp;
}

void StorageRecordsLinks::reset() {
    const QWriteLocker locker(&m_lock);
    m_SetRecordLinkTmp = m_SetRecordLink;
    m_commit = true;
}

void StorageRecordsLinks::commit() {
    const QWriteLocker locker(&m_lock);
    m_SetRecordLink = m_SetRecordLinkTmp;
    m_commit = true;
}

qsizetype StorageRecordsLinks::count() const {
    const QReadLocker locker(&m_lock);
    return m_SetRecordLinkTmp.count();
}

qsizetype StorageRecordsLinks::countCommitted() const {
    const QReadLocker locker(&m_lock);
    return m_SetRecordLink.count();
}

void StorageRecordsLinks::clear() {
    const QWriteLocker locker(&m_lock);
    m_SetRecordLinkTmp.clear();
    m_commit = false;
}

void StorageRecordsLinks::serialize(QDataStream &out) const {
    const QReadLocker locker(&m_lock);

    out << out.version();

    out << static_cast<qint32>(m_SetRecordLink.size());
    for (auto it = m_SetRecordLink.begin(); it != m_SetRecordLink.end(); ++it) {
        it->serialize(out);
    }

    out << static_cast<qint32>(m_SetRecordLinkTmp.size());
    for (auto it = m_SetRecordLinkTmp.begin(); it != m_SetRecordLinkTmp.end(); ++it) {
        it->serialize(out);
    }

    out << m_commit;

    if (out.status() != QDataStream::Ok) {
        throwStreamError(out.status());
    }
}

void StorageRecordsLinks::deserialize(QDataStream &in) {
    const QWriteLocker locker(&m_lock);

    deserializeVersion(in);
    QSet<RecordLink> setRecordLink = deserializeRecordsLinks(in);
    QSet<RecordLink> setRecordLinkTmp = deserializeRecordsLinks(in);
    bool commit = deserializeCommit(in);

    m_SetRecordLink = setRecordLink;
    m_SetRecordLinkTmp = setRecordLinkTmp;
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
