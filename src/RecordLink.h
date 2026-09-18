#ifndef RECORDLINK_H
#define RECORDLINK_H

#include <QObject>
#include <QReadWriteLock>
#include <QSet>

#include "Constants.h"
#include "Exceptions.h"

class RecordLink {
public:
    explicit RecordLink(qint32 idDate,
                        qint32 idDrawing,
                        qint32 idAmount,
                        const QSet<qint32>& idExecutors,
                        const QSet<qint32>& idAuthors,
                        const QSet<qint32>& idCastingMaterials,
                        const QSet<qint32>& idModelMaterials,
                        const QSet<qint32>& idMachines,
                        const QSet<qint32>& idNotes)
        : m_idDate(idDate)
        , m_idDrawing(idDrawing)
        , m_idAmount(idAmount)
        , m_idExecutors(idExecutors)
        , m_idAuthors(idAuthors)
        , m_idCastingMaterials(idCastingMaterials)
        , m_idModelMaterials(idModelMaterials)
        , m_idMachines(idMachines)
        , m_idNotes(idNotes) { }

    RecordLink(const RecordLink& other) {
        QReadLocker otherLocker(&other.m_lock);

        m_idDate = other.m_idDate;
        m_idDrawing = other.m_idDrawing;
        m_idAmount = other.m_idAmount;
        m_idExecutors = other.m_idExecutors;
        m_idAuthors = other.m_idAuthors;
        m_idCastingMaterials = other.m_idCastingMaterials;
        m_idModelMaterials = other.m_idModelMaterials;
        m_idMachines = other.m_idMachines;
        m_idNotes = other.m_idNotes;
    }

    RecordLink(RecordLink&& other) {
        swap(other);

        QWriteLocker otherLocker(&other.m_lock);

        other.m_idDate = 0;
        other.m_idDrawing = 0;
        other.m_idAmount = 0;
        other.m_idExecutors.clear();
        other.m_idAuthors.clear();
        other.m_idCastingMaterials.clear();
        other.m_idModelMaterials.clear();
        other.m_idMachines.clear();
        other.m_idNotes.clear();
    }

    RecordLink& operator =(const RecordLink& other) {
        if (this == &other) {
            return *this;
        }

        QReadWriteLock* first = &m_lock;
        QReadWriteLock* second = &other.m_lock;
        if (second < first) {
            std::swap(first, second);
        }

        QWriteLocker l1(first);
        QWriteLocker l2(second);

        m_idDate = other.m_idDate;
        m_idDrawing = other.m_idDrawing;
        m_idAmount = other.m_idAmount;
        m_idExecutors = other.m_idExecutors;
        m_idAuthors = other.m_idAuthors;
        m_idCastingMaterials = other.m_idCastingMaterials;
        m_idModelMaterials = other.m_idModelMaterials;
        m_idMachines = other.m_idMachines;
        m_idNotes = other.m_idNotes;
        return *this;
    }
    RecordLink& operator =(RecordLink&& other) {
        if (this == &other) {
            return *this;
        }

        swap(other);

        QWriteLocker otherLocker(&other.m_lock);

        other.m_idDate = 0;
        other.m_idDrawing = 0;
        other.m_idAmount = 0;
        other.m_idExecutors.clear();
        other.m_idAuthors.clear();
        other.m_idCastingMaterials.clear();
        other.m_idModelMaterials.clear();
        other.m_idMachines.clear();
        other.m_idNotes.clear();
        return *this;
    }

    bool operator ==(const RecordLink& other) const {
        QReadLocker locker(&m_lock);
        QReadLocker otherLocker(&other.m_lock);
        return m_idDate == other.m_idDate && m_idDrawing == other.m_idDrawing &&
               m_idAmount == other.m_idAmount && m_idExecutors == other.m_idExecutors &&
               m_idAuthors == other.m_idAuthors &&
               m_idCastingMaterials == other.m_idCastingMaterials &&
               m_idModelMaterials == other.m_idModelMaterials &&
               m_idMachines == other.m_idMachines && m_idNotes == other.m_idNotes;
    }

    bool operator !=(const RecordLink& other) const {
        return !(*this == other);
    }

    void swap(RecordLink& other) {
        QReadWriteLock* first = &m_lock;
        QReadWriteLock* second = &other.m_lock;
        if (second < first) {
            std::swap(first, second);
        }

        QWriteLocker l1(first);
        QWriteLocker l2(second);

        std::swap(m_idDate, other.m_idDate);
        std::swap(m_idDrawing, other.m_idDrawing);
        std::swap(m_idAmount, other.m_idAmount);
        m_idExecutors.swap(other.m_idExecutors);
        m_idAuthors.swap(other.m_idAuthors);
        m_idCastingMaterials.swap(other.m_idCastingMaterials);
        m_idModelMaterials.swap(other.m_idModelMaterials);
        m_idMachines.swap(other.m_idMachines);
        m_idNotes.swap(other.m_idNotes);
    }

    void setIdDate(qint32 idDate) {
        QWriteLocker locker(&m_lock);
        m_idDate = idDate;
    }

    void setIdDrawing(qint32 idDrawing) {
        QWriteLocker locker(&m_lock);
        m_idDrawing = idDrawing;
    }

    void setIdAmount(qint32 idAmount) {
        QWriteLocker locker(&m_lock);
        m_idAmount = idAmount;
    }

    void addExecutors(const QSet<qint32>& idExecutors) {
        QWriteLocker locker(&m_lock);
        m_idExecutors.unite(idExecutors);
    }

    void addAuthors(const QSet<qint32>& idAuthors) {
        QWriteLocker locker(&m_lock);
        m_idAuthors.unite(idAuthors);
    }

    void addCastingMaterials(const QSet<qint32>& idCastingMaterials) {
        QWriteLocker locker(&m_lock);
        m_idCastingMaterials.unite(idCastingMaterials);
    }

    void addModelMaterials(const QSet<qint32>& idModelMaterials) {
        QWriteLocker locker(&m_lock);
        m_idModelMaterials.unite(idModelMaterials);
    }

    void addMachines(const QSet<qint32>& idMachines) {
        QWriteLocker locker(&m_lock);
        m_idMachines.unite(idMachines);
    }

    void addNotes(const QSet<qint32>& idNotes) {
        QWriteLocker locker(&m_lock);
        m_idNotes.unite(idNotes);
    }

    void removeExecutors(const QSet<qint32>& idExecutors) {
        QWriteLocker locker(&m_lock);
        m_idExecutors.subtract(idExecutors);
    }

    void removeAuthors(const QSet<qint32>& idAuthors) {
        QWriteLocker locker(&m_lock);
        m_idAuthors.subtract(idAuthors);
    }

    void removeCastingMaterials(const QSet<qint32>& idCastingMaterials) {
        QWriteLocker locker(&m_lock);
        m_idCastingMaterials.subtract(idCastingMaterials);
    }

    void removeModelMaterials(const QSet<qint32>& idModelMaterials) {
        QWriteLocker locker(&m_lock);
        m_idModelMaterials.subtract(idModelMaterials);
    }

    void removeMachines(const QSet<qint32>& idMachines) {
        QWriteLocker locker(&m_lock);
        m_idMachines.subtract(idMachines);
    }

    void removeNotes(const QSet<qint32>& idNotes) {
        QWriteLocker locker(&m_lock);
        m_idNotes.subtract(idNotes);
    }

    qint32 getIdDate() const {
        QReadLocker locker(&m_lock);
        return m_idDate;
    }

    qint32 getIdDrawing() const {
        QReadLocker locker(&m_lock);
        return m_idDrawing;
    }

    qint32 getIdAmount() const {
        QReadLocker locker(&m_lock);
        return m_idAmount;
    }

    QSet<qint32> getIdExecutors() const {
        QReadLocker locker(&m_lock);
        return m_idExecutors;
    }

    QSet<qint32> getIdAuthors() const {
        QReadLocker locker(&m_lock);
        return m_idAuthors;
    }

    QSet<qint32> getIdCastingMaterials() const {
        QReadLocker locker(&m_lock);
        return m_idCastingMaterials;
    }

    QSet<qint32> getIdModelMaterials() const {
        QReadLocker locker(&m_lock);
        return m_idModelMaterials;
    }

    QSet<qint32> getIdMachines() const {
        QReadLocker locker(&m_lock);
        return m_idMachines;
    }

    QSet<qint32> getIdNotes() const {
        QReadLocker locker(&m_lock);
        return m_idNotes;
    }

    void serialize(QDataStream& out) const {
        QReadLocker locker(&m_lock);

        out << REQUIRED_SERIALIZATION_VERSION;

        out << m_idDate;
        out << m_idDrawing;
        out << m_idAmount;

        out << static_cast<qint32>(m_idExecutors.size());
        for (auto it = m_idExecutors.begin(); it != m_idExecutors.end(); ++it) {
            out << *it;
        }

        out << static_cast<qint32>(m_idAuthors.size());
        for (auto it = m_idAuthors.begin(); it != m_idAuthors.end(); ++it) {
            out << *it;
        }

        out << static_cast<qint32>(m_idCastingMaterials.size());
        for (auto it = m_idCastingMaterials.begin(); it != m_idCastingMaterials.end(); ++it) {
            out << *it;
        }

        out << static_cast<qint32>(m_idModelMaterials.size());
        for (auto it = m_idModelMaterials.begin(); it != m_idModelMaterials.end(); ++it) {
            out << *it;
        }

        out << static_cast<qint32>(m_idMachines.size());
        for (auto it = m_idMachines.begin(); it != m_idMachines.end(); ++it) {
            out << *it;
        }

        out << static_cast<qint32>(m_idNotes.size());
        for (auto it = m_idNotes.begin(); it != m_idNotes.end(); ++it) {
            out << *it;
        }

        if (out.status() != QDataStream::Ok) {
            throwStreamError(out.status());
        }
    }

    void deserialize(QDataStream& in) {
        QWriteLocker locker(&m_lock);

        deserializeVersion(in);

        qint32 tmpDate = deserializeId(in);
        qint32 tmpDrawing = deserializeId(in);
        qint32 tmpAmount = deserializeId(in);
        QSet<qint32> tmpExecutors = deserializeIds(in);
        QSet<qint32> tmpAuthors = deserializeIds(in);
        QSet<qint32> tmpCastingMaterials = deserializeIds(in);
        QSet<qint32> tmpModelMaterials = deserializeIds(in);
        QSet<qint32> tmpMachines = deserializeIds(in);
        QSet<qint32> tmpNotes = deserializeIds(in);

        m_idDate = tmpDate;
        m_idDrawing = tmpDrawing;
        m_idAmount = tmpAmount;
        m_idExecutors = tmpExecutors;
        m_idAuthors = tmpAuthors;
        m_idCastingMaterials = tmpCastingMaterials;
        m_idModelMaterials = tmpModelMaterials;
        m_idMachines = tmpMachines;
        m_idNotes = tmpNotes;
    }

private:
    qint32 m_idDate = 0;
    qint32 m_idDrawing = 0;
    qint32 m_idAmount = 0;
    QSet<qint32> m_idExecutors;
    QSet<qint32> m_idAuthors;
    QSet<qint32> m_idCastingMaterials;
    QSet<qint32> m_idModelMaterials;
    QSet<qint32> m_idMachines;
    QSet<qint32> m_idNotes;
    mutable QReadWriteLock m_lock;

    friend uint qHash(const RecordLink& recordLink, uint seed);

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

    qint32 deserializeId(QDataStream& in) const {
        qint32 id { };
        in >> id;
        if (in.status() != QDataStream::Ok) {
            throwStreamError(in.status());
        }
        if (id < 0) {
            in.setStatus(QDataStream::Status::ReadCorruptData);
            throwStreamError(in.status());
        }
        return id;
    }

    QSet<qint32> deserializeIds(QDataStream& in) const {
        qint32 size;
        in >> size;
        if (in.status() != QDataStream::Ok) {
            throwStreamError(in.status());
        }
        if (size < 0) {
            in.setStatus(QDataStream::Status::ReadCorruptData);
            throwStreamError(in.status());
        }

        QSet<qint32> ids;
        for (qint32 i = 0; i < size; ++i) {
            qint32 id;
            in >> id;
            if (in.status() != QDataStream::Ok) {
                throwStreamError(in.status());
            }
            ids.insert(id);
        }
        return ids;
    }
};

inline uint qHash(const RecordLink& recordLink, uint seed = 0) {
    return qHash(recordLink.m_idDate, seed) ^ qHash(recordLink.m_idDrawing, seed) ^
           qHash(recordLink.m_idAmount, seed) ^ qHash(recordLink.m_idExecutors, seed) ^
           qHash(recordLink.m_idAuthors, seed) ^ qHash(recordLink.m_idCastingMaterials, seed) ^
           qHash(recordLink.m_idModelMaterials, seed) ^ qHash(recordLink.m_idMachines, seed) ^
           qHash(recordLink.m_idNotes, seed);
}

#endif // RECORDLINK_H
