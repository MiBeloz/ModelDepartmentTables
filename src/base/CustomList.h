#ifndef CUSTOMLIST_H
#define CUSTOMLIST_H

#include <QDebug>
#include <QHash>
#include <QObject>
#include <QReadWriteLock>
#include <QStack>
#include <QVariant>

namespace mdt {
    namespace work {
        class CustomList {
        public:
            CustomList() = default;
            virtual ~CustomList() = default;

            CustomList(const CustomList&) = default;
            CustomList(CustomList&&) noexcept = default;
            CustomList& operator=(const CustomList&) = default;
            CustomList& operator=(CustomList&&) noexcept = default;

            bool operator ==(const CustomList& other) const;
            bool operator !=(const CustomList& other) const;

            void swap(CustomList& other);

            virtual std::optional<qint32> insert(const QVariant& data);
            virtual bool remove(const QVariant& data);
            virtual std::optional<qint32> getId(const QVariant& data) const;
            virtual std::optional<QVariant> getValue(qint32 id) const;
            virtual QVariantList getAllValues() const;

            void clear();
            void reset();
            void commit();

            qsizetype size() const;
            qsizetype sizeCommitted() const;

            void serialize(QDataStream& out) const;
            void deserialize(QDataStream& in);

        private:
            QHash<qint32, QVariant> m_list;
            QHash<qint32, QVariant> m_listTmp;
            qint32 m_id { };
            qint32 m_idTmp { };
            QStack<qint32> m_emptyId;
            QStack<qint32> m_emptyIdTmp;
            bool m_commit = true;

            void throwStreamError(QDataStream::Status status) const;

            void deserializeVersion(QDataStream& in) const;
            QHash<qint32, QVariant> deserializeList(QDataStream& in) const;
            qint32 deserializeId(QDataStream& in) const;
            QStack<qint32> deserializeEmptyId(QDataStream& in) const;
            bool deserializeCommit(QDataStream& in) const;
        };
    } // namespace work
} // namespace mdt

#endif // CUSTOMLIST_H
