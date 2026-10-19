#ifndef RECORDLINK_H
#define RECORDLINK_H

#include <QObject>
#include <QReadWriteLock>
#include <QSet>

namespace mdt {
    namespace work {
        enum class IdsList : quint8 {
            Executors,
            Authors,
            CastingMaterials,
            ModelMaterials,
            Machines,
            Notes
        };

        class RecordLink {
        public:
            RecordLink() = default;

            explicit RecordLink(qint32 idDate,
                                qint32 idDrawing,
                                qint32 idAmount,
                                const QSet<qint32>& idExecutors,
                                const QSet<qint32>& idAuthors,
                                const QSet<qint32>& idCastingMaterials,
                                const QSet<qint32>& idModelMaterials,
                                const QSet<qint32>& idMachines,
                                const QSet<qint32>& idNotes);

            RecordLink(const RecordLink& other);
            RecordLink(RecordLink&& other) noexcept;
            RecordLink& operator =(const RecordLink& other);
            RecordLink& operator =(RecordLink&& other) noexcept;

            bool operator ==(const RecordLink& other) const;
            bool operator !=(const RecordLink& other) const;

            void swap(RecordLink& other);

            void setIdDate(qint32 idDate);
            void setIdDrawing(qint32 idDrawing);
            void setIdAmount(qint32 idAmount);
            void addIds(IdsList idsList, const QSet<qint32>& ids);
            void removeIds(IdsList idsList, const QSet<qint32>& ids);
            qint32 getIdDate() const;
            qint32 getIdDrawing() const;
            qint32 getIdAmount() const;
            const QSet<qint32>& getIds(IdsList idsList) const;
            void replaceIds(IdsList idsList, const QSet<qint32>& newIds);

            void serialize(QDataStream& out) const;
            void deserialize(QDataStream& in);

            size_t hash(size_t seed = 0) const;

            static const RecordLink Null;

        private:
            qint32 m_idDate = 0;
            qint32 m_idDrawing = 0;
            qint32 m_idAmount = 0;
            QSet<qint32> m_idsExecutors;
            QSet<qint32> m_idsAuthors;
            QSet<qint32> m_idsCastingMaterials;
            QSet<qint32> m_idsModelMaterials;
            QSet<qint32> m_idsMachines;
            QSet<qint32> m_idsNotes;

            void throwStreamError(QDataStream::Status status) const;

            void deserializeVersion(QDataStream& in) const;
            qint32 deserializeId(QDataStream& in) const;
            QSet<qint32> deserializeIds(QDataStream& in) const;
        };

        inline const RecordLink RecordLink::Null { };

        inline size_t qHash(const RecordLink& recordLink, size_t seed = 0) {
            return recordLink.hash(seed);
        }
    } // namespace work
} // namespace mdt

#endif // RECORDLINK_H
