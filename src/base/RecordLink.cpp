#include "RecordLink.h"

#include "Exceptions.h"

namespace mdt {
    namespace work {
        RecordLink::RecordLink(qint32 idDate,
                               qint32 idDrawing,
                               qint32 idAmount,
                               const QSet<qint32> &idsExecutors,
                               const QSet<qint32> &idsAuthors,
                               const QSet<qint32> &idsCastingMaterials,
                               const QSet<qint32> &idsModelMaterials,
                               const QSet<qint32> &idsMachines,
                               const QSet<qint32> &idsNotes)
            : m_idDate(idDate)
            , m_idDrawing(idDrawing)
            , m_idAmount(idAmount)
            , m_idsExecutors(idsExecutors)
            , m_idsAuthors(idsAuthors)
            , m_idsCastingMaterials(idsCastingMaterials)
            , m_idsModelMaterials(idsModelMaterials)
            , m_idsMachines(idsMachines)
            , m_idsNotes(idsNotes) { }

        RecordLink::RecordLink(const RecordLink &other) {
            m_idDate = other.m_idDate;
            m_idDrawing = other.m_idDrawing;
            m_idAmount = other.m_idAmount;
            m_idsExecutors = other.m_idsExecutors;
            m_idsAuthors = other.m_idsAuthors;
            m_idsCastingMaterials = other.m_idsCastingMaterials;
            m_idsModelMaterials = other.m_idsModelMaterials;
            m_idsMachines = other.m_idsMachines;
            m_idsNotes = other.m_idsNotes;
        }

        RecordLink::RecordLink(RecordLink &&other) noexcept {
            m_idDate = other.m_idDate;
            m_idDrawing = other.m_idDrawing;
            m_idAmount = other.m_idAmount;
            m_idsExecutors = std::move(other.m_idsExecutors);
            m_idsAuthors = std::move(other.m_idsAuthors);
            m_idsCastingMaterials = std::move(other.m_idsCastingMaterials);
            m_idsModelMaterials = std::move(other.m_idsModelMaterials);
            m_idsMachines = std::move(other.m_idsMachines);
            m_idsNotes = std::move(other.m_idsNotes);

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
            m_idsExecutors = other.m_idsExecutors;
            m_idsAuthors = other.m_idsAuthors;
            m_idsCastingMaterials = other.m_idsCastingMaterials;
            m_idsModelMaterials = other.m_idsModelMaterials;
            m_idsMachines = other.m_idsMachines;
            m_idsNotes = other.m_idsNotes;
            return *this;
        }

        RecordLink &RecordLink::operator =(RecordLink &&other) noexcept {
            if (this == &other) {
                return *this;
            }

            m_idDate = other.m_idDate;
            m_idDrawing = other.m_idDrawing;
            m_idAmount = other.m_idAmount;
            m_idsExecutors = std::move(other.m_idsExecutors);
            m_idsAuthors = std::move(other.m_idsAuthors);
            m_idsCastingMaterials = std::move(other.m_idsCastingMaterials);
            m_idsModelMaterials = std::move(other.m_idsModelMaterials);
            m_idsMachines = std::move(other.m_idsMachines);
            m_idsNotes = std::move(other.m_idsNotes);

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
                   m_idAmount == other.m_idAmount && m_idsExecutors == other.m_idsExecutors &&
                   m_idsAuthors == other.m_idsAuthors &&
                   m_idsCastingMaterials == other.m_idsCastingMaterials &&
                   m_idsModelMaterials == other.m_idsModelMaterials &&
                   m_idsMachines == other.m_idsMachines && m_idsNotes == other.m_idsNotes;
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
            m_idsExecutors.swap(other.m_idsExecutors);
            m_idsAuthors.swap(other.m_idsAuthors);
            m_idsCastingMaterials.swap(other.m_idsCastingMaterials);
            m_idsModelMaterials.swap(other.m_idsModelMaterials);
            m_idsMachines.swap(other.m_idsMachines);
            m_idsNotes.swap(other.m_idsNotes);
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

        void RecordLink::addIds(IdsList idsList, const QSet<qint32> &ids) {
            switch (idsList) {
                case IdsList::Executors:
                    m_idsExecutors.unite(ids);
                    return;
                case IdsList::Authors:
                    m_idsAuthors.unite(ids);
                    return;
                case IdsList::CastingMaterials:
                    m_idsCastingMaterials.unite(ids);
                    return;
                case IdsList::ModelMaterials:
                    m_idsModelMaterials.unite(ids);
                    return;
                case IdsList::Machines:
                    m_idsMachines.unite(ids);
                    return;
                case IdsList::Notes:
                    m_idsNotes.unite(ids);
                    return;
            }
        }

        void RecordLink::removeIds(IdsList idsList, const QSet<qint32> &ids) {
            switch (idsList) {
                case IdsList::Executors:
                    m_idsExecutors.subtract(ids);
                    return;
                case IdsList::Authors:
                    m_idsAuthors.subtract(ids);
                    return;
                case IdsList::CastingMaterials:
                    m_idsCastingMaterials.subtract(ids);
                    return;
                case IdsList::ModelMaterials:
                    m_idsModelMaterials.subtract(ids);
                    return;
                case IdsList::Machines:
                    m_idsMachines.subtract(ids);
                    return;
                case IdsList::Notes:
                    m_idsNotes.subtract(ids);
                    return;
            }
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

        const QSet<qint32> &RecordLink::getIds(IdsList idsList) const {
            switch (idsList) {
                case IdsList::Executors:
                    return m_idsExecutors;
                case IdsList::Authors:
                    return m_idsAuthors;
                case IdsList::CastingMaterials:
                    return m_idsCastingMaterials;
                case IdsList::ModelMaterials:
                    return m_idsModelMaterials;
                case IdsList::Machines:
                    return m_idsMachines;
                case IdsList::Notes:
                    return m_idsNotes;
            }
            Q_UNREACHABLE();
        }

        void RecordLink::replaceIds(IdsList idsList, const QSet<qint32> &newIds) {
            switch (idsList) {
                case IdsList::Executors:
                    m_idsExecutors = newIds;
                    return;
                case IdsList::Authors:
                    m_idsAuthors = newIds;
                    return;
                case IdsList::CastingMaterials:
                    m_idsCastingMaterials = newIds;
                    return;
                case IdsList::ModelMaterials:
                    m_idsModelMaterials = newIds;
                    return;
                case IdsList::Machines:
                    m_idsMachines = newIds;
                    return;
                case IdsList::Notes:
                    m_idsNotes = newIds;
                    return;
            }
        }

        void RecordLink::serialize(QDataStream &out) const {
            out << out.version();

            out << m_idDate;
            out << m_idDrawing;
            out << m_idAmount;

            out << static_cast<qint32>(m_idsExecutors.size());
            for (auto &v : std::as_const(m_idsExecutors)) {
                out << v;
            }

            out << static_cast<qint32>(m_idsAuthors.size());
            for (auto &v : std::as_const(m_idsAuthors)) {
                out << v;
            }

            out << static_cast<qint32>(m_idsCastingMaterials.size());
            for (auto &v : std::as_const(m_idsCastingMaterials)) {
                out << v;
            }

            out << static_cast<qint32>(m_idsModelMaterials.size());
            for (auto &v : std::as_const(m_idsModelMaterials)) {
                out << v;
            }

            out << static_cast<qint32>(m_idsMachines.size());
            for (auto &v : std::as_const(m_idsMachines)) {
                out << v;
            }

            out << static_cast<qint32>(m_idsNotes.size());
            for (auto &v : std::as_const(m_idsNotes)) {
                out << v;
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
            QSet<qint32> tmpExecutors = deserializeIds(in);
            QSet<qint32> tmpAuthors = deserializeIds(in);
            QSet<qint32> tmpCastingMaterials = deserializeIds(in);
            QSet<qint32> tmpModelMaterials = deserializeIds(in);
            QSet<qint32> tmpMachines = deserializeIds(in);
            QSet<qint32> tmpNotes = deserializeIds(in);

            m_idDate = tmpDate;
            m_idDrawing = tmpDrawing;
            m_idAmount = tmpAmount;
            m_idsExecutors = tmpExecutors;
            m_idsAuthors = tmpAuthors;
            m_idsCastingMaterials = tmpCastingMaterials;
            m_idsModelMaterials = tmpModelMaterials;
            m_idsMachines = tmpMachines;
            m_idsNotes = tmpNotes;
        }

        size_t RecordLink::hash(size_t seed) const {
            return qHashMulti(seed,
                              m_idDate,
                              m_idDrawing,
                              m_idAmount,
                              m_idsExecutors,
                              m_idsAuthors,
                              m_idsCastingMaterials,
                              m_idsModelMaterials,
                              m_idsMachines,
                              m_idsNotes);
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
                    QObject::tr("Version error. Required version: '%1', Current version: '%2'")
                        .arg(QString::number(in.version()), QString::number(version)));
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

        QSet<qint32> RecordLink::deserializeIds(QDataStream &in) const {
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
    } // namespace work
} // namespace mdt
