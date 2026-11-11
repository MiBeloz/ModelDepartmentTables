#include "RecordLink.h"

#include "Exceptions.h"

namespace mdt {
    namespace work {
        RecordLink::RecordLink(qint32 idDate,
                               qint32 idDrawing,
                               qint32 idAmount,
                               QList<QSet<qint32>> idsCustom)
            : m_idDate(idDate)
            , m_idDrawing(idDrawing)
            , m_idAmount(idAmount)
            , m_idsCustom(idsCustom) {}

        RecordLink::RecordLink(const RecordLink &other) {
            m_idDate = other.m_idDate;
            m_idDrawing = other.m_idDrawing;
            m_idAmount = other.m_idAmount;
            m_idsCustom = other.m_idsCustom;
        }

        RecordLink::RecordLink(RecordLink &&other) noexcept {
            m_idDate = other.m_idDate;
            m_idDrawing = other.m_idDrawing;
            m_idAmount = other.m_idAmount;
            m_idsCustom = std::move(other.m_idsCustom);

            other.m_idDate = 0;
            other.m_idDrawing = 0;
            other.m_idAmount = 0;
        }

        RecordLink &RecordLink::operator =(const RecordLink &other) {
            if (this == &other) {
                return *this;
            }

            m_idDate = other.m_idDate;
            m_idDrawing = other.m_idDrawing;
            m_idAmount = other.m_idAmount;
            m_idsCustom = other.m_idsCustom;
            return *this;
        }

        RecordLink &RecordLink::operator =(RecordLink &&other) noexcept {
            if (this == &other) {
                return *this;
            }

            m_idDate = other.m_idDate;
            m_idDrawing = other.m_idDrawing;
            m_idAmount = other.m_idAmount;
            m_idsCustom = std::move(other.m_idsCustom);

            other.m_idDate = 0;
            other.m_idDrawing = 0;
            other.m_idAmount = 0;

            return *this;
        }

        bool RecordLink::operator ==(const RecordLink &other) const {
            if (this == &other) {
                return true;
            }

            return m_idDate == other.m_idDate && m_idDrawing == other.m_idDrawing &&
                   m_idAmount == other.m_idAmount && m_idsCustom == other.m_idsCustom;
        }

        bool RecordLink::operator !=(const RecordLink &other) const {
            return !(*this == other);
        }

        void RecordLink::swap(RecordLink &other) {
            if (this == &other) {
                return;
            }

            std::swap(m_idDate, other.m_idDate);
            std::swap(m_idDrawing, other.m_idDrawing);
            std::swap(m_idAmount, other.m_idAmount);
            m_idsCustom.swap(other.m_idsCustom);
        }

        void RecordLink::setIdDate(qint32 idDate) {
            m_idDate = idDate;
        }

        void RecordLink::setIdDrawing(qint32 idDrawing) {
            m_idDrawing = idDrawing;
        }

        void RecordLink::setIdAmount(qint32 idAmount) {
            m_idAmount = idAmount;
        }

        void RecordLink::addIdsCustom(qsizetype customIndex, const QSet<qint32> &ids) {
            m_idsCustom[customIndex].unite(ids);
        }

        void RecordLink::removeIdsCustom(qsizetype customIndex, const QSet<qint32> &ids) {
            m_idsCustom[customIndex].subtract(ids);
        }

        qint32 RecordLink::getIdDate() const {
            return m_idDate;
        }

        qint32 RecordLink::getIdDrawing() const {
            return m_idDrawing;
        }

        qint32 RecordLink::getIdAmount() const {
            return m_idAmount;
        }

        const QSet<qint32> &RecordLink::getIdsCustom(qsizetype customIndex) const {
            return m_idsCustom[customIndex];
        }

        void RecordLink::replaceIdsCustom(qsizetype customIndex, const QSet<qint32> &newIds) {
            m_idsCustom[customIndex] = newIds;
        }

        void RecordLink::serialize(QDataStream &out) const {
            out << out.version();
            out << m_recordLinkVersion;

            out << m_idDate;
            out << m_idDrawing;
            out << m_idAmount;

            out << static_cast<qint32>(m_idsCustom.size());
            for (auto &l : std::as_const(m_idsCustom)) {
                out << static_cast<qint32>(l.size());
                for (auto &s : std::as_const(l)) {
                    out << s;
                }
            }

            if (out.status() != QDataStream::Ok) {
                throwStreamError(out.status());
            }
        }

        void RecordLink::deserialize(QDataStream &in) {
            deserializeVersion(in);

            qint32 tmpDate = deserializeId(in);
            qint32 tmpDrawing = deserializeId(in);
            qint32 tmpAmount = deserializeId(in);
            QList<QSet<qint32>> tmpCustom = deserializeIds(in);

            m_idDate = tmpDate;
            m_idDrawing = tmpDrawing;
            m_idAmount = tmpAmount;
            m_idsCustom = tmpCustom;
        }

        size_t RecordLink::hash(size_t seed) const {
            return qHashMulti(seed, m_idDate, m_idDrawing, m_idAmount, m_idsCustom);
        }

        void RecordLink::throwStreamError(QDataStream::Status status) const {
            throw RuntimeError(
                QObject::tr("QDataStream error. Error code: '%1'.").arg(static_cast<int>(status)));
        }

        void RecordLink::deserializeVersion(QDataStream &in) const {
            int version { };
            in >> version;
            if (in.status() != QDataStream::Ok) {
                throwStreamError(in.status());
            }
            if (version != in.version()) {
                in.setStatus(QDataStream::Status::ReadCorruptData);
                throw RuntimeError(
                    QObject::tr(
                        "DataStream version error. Required version: '%1', Current version: '%2'")
                        .arg(QString::number(in.version()), QString::number(version)));
            }
        }

        void RecordLink::deserializeRecordLinkVersion(QDataStream &in) const {
            int recordLinkVersion {};
            in >> recordLinkVersion;
            if (in.status() != QDataStream::Ok) {
                throwStreamError(in.status());
            }
            if (recordLinkVersion != m_recordLinkVersion) {
                in.setStatus(QDataStream::Status::ReadCorruptData);
                throw RuntimeError(
                    QObject::tr(
                        "RecordLink version error. Required version: '%1', Current version: '%2'")
                        .arg(QString::number(m_recordLinkVersion),
                             QString::number(recordLinkVersion)));
            }
        }

        qint32 RecordLink::deserializeId(QDataStream &in) const {
            qint32 id { };
            in >> id;
            if (in.status() != QDataStream::Ok) {
                throwStreamError(in.status());
            }
            return id;
        }

        QList<QSet<qint32>> RecordLink::deserializeIds(QDataStream &in) const {
            qint32 sizeList;
            in >> sizeList;
            if (in.status() != QDataStream::Ok) {
                throwStreamError(in.status());
            }
            if (sizeList < 0) {
                in.setStatus(QDataStream::Status::ReadCorruptData);
                throwStreamError(in.status());
            }

            QList<QSet<qint32>> idsList;
            for (qint32 i = 0; i < sizeList; ++i) {
                qint32 sizeSet;
                in >> sizeSet;
                if (in.status() != QDataStream::Ok) {
                    throwStreamError(in.status());
                }
                if (sizeSet < 0) {
                    in.setStatus(QDataStream::Status::ReadCorruptData);
                    throwStreamError(in.status());
                }

                QSet<qint32> ids;
                for (qint32 i = 0; i < sizeSet; ++i) {
                    qint32 id;
                    in >> id;
                    if (in.status() != QDataStream::Ok) {
                        throwStreamError(in.status());
                    }
                    ids.insert(id);
                }
                idsList.append(ids);
            }
            return idsList;
        }
    } // namespace work
} // namespace mdt
