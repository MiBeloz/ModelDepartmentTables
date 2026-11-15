#ifndef CUSTOMLIST_H
#define CUSTOMLIST_H

#include <optional>

#include <QHash>
#include <QStack>
#include <QVariant>

namespace mdt {
    namespace work {
        class CustomList final {
        public:
            CustomList() = default;
            ~CustomList() = default;

            CustomList(const CustomList&) = default;
            CustomList(CustomList&&) noexcept = default;
            CustomList& operator=(const CustomList&) = default;
            CustomList& operator=(CustomList&&) noexcept = default;

            bool operator ==(const CustomList& other) const;
            bool operator !=(const CustomList& other) const;

            void swap(CustomList& other);

            std::optional<qint32> insert(const QVariant& data);
            bool remove(const QVariant& data);
            std::optional<qint32> getId(const QVariant& data) const;
            std::optional<QVariant> getValue(qint32 id) const;
            QVariantList getAllValues() const;

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

            static constexpr int CUSTOMLIST_SCHEMA_VERSION = 1;
            static constexpr qint32 MAX_SIZE = 10000000;

            void throwStreamError(QDataStream::Status status) const;

            void deserializeVersion(QDataStream& in);
            QHash<qint32, QVariant> deserializeList(QDataStream& in);
            qint32 deserializeId(QDataStream& in);
            QStack<qint32> deserializeEmptyId(QDataStream& in);
            bool deserializeCommit(QDataStream& in);
        };
    } // namespace work
} // namespace mdt

#endif // CUSTOMLIST_H
