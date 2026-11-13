#ifndef RECORDLINK_H
#define RECORDLINK_H

#include <QObject>
#include <QReadWriteLock>
#include <QSet>

namespace mdt {
    namespace work {
        class RecordLink {
        public:
            RecordLink() = default;
            ~RecordLink() = default;

            explicit RecordLink(qint32 idDate,
                                qint32 idDrawing,
                                qint32 idAmount,
                                QList<QSet<qint32>> idsCustom);

            RecordLink(const RecordLink& other) = default;
            RecordLink(RecordLink&& other) noexcept = default;
            RecordLink& operator=(const RecordLink& other) = default;
            RecordLink& operator=(RecordLink&& other) noexcept = default;

            bool operator ==(const RecordLink& other) const;
            bool operator !=(const RecordLink& other) const;

            void swap(RecordLink& other);

            void setIdDate(qint32 idDate);
            void setIdDrawing(qint32 idDrawing);
            void setIdAmount(qint32 idAmount);
            void addIdsCustom(qsizetype customIndex, const QSet<qint32>& ids);
            void removeIdsCustom(qsizetype customIndex, const QSet<qint32>& ids);
            qint32 getIdDate() const;
            qint32 getIdDrawing() const;
            qint32 getIdAmount() const;
            const QSet<qint32>& getIdsCustom(qsizetype customIndex) const;
            void replaceIdsCustom(qsizetype customIndex, const QSet<qint32>& newIds);
            qsizetype idsCustomCount() const;

            void serialize(QDataStream& out) const;
            void deserialize(QDataStream& in);

            size_t hash(size_t seed = 0) const;

            static const RecordLink Null;

        private:
            qint32 m_idDate = 0;
            qint32 m_idDrawing = 0;
            qint32 m_idAmount = 0;
            QList<QSet<qint32>> m_idsCustom;

            void throwStreamError(QDataStream::Status status) const;

            void deserializeVersion(QDataStream& in) const;
            void deserializeRecordLinkVersion(QDataStream& in) const;
            qint32 deserializeId(QDataStream& in) const;
            QList<QSet<qint32>> deserializeIds(QDataStream& in) const;

            static const int m_recordLinkVersion = 1;
        };

        inline const RecordLink RecordLink::Null { };

        inline size_t qHash(const RecordLink& recordLink, size_t seed = 0) {
            return recordLink.hash(seed);
        }
    } // namespace work
} // namespace mdt

#endif // RECORDLINK_H
