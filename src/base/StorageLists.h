#ifndef STORAGELISTS_H
#define STORAGELISTS_H

#include "CustomList.h"
#include "DatesList.h"
#include "Storage.h"

namespace mdt {
    namespace work {
        class StorageLists final : public Storage {
        public:
            StorageLists() = default;

            void addList();

            qsizetype customListCount() const;

            DatesList& dates();
            const DatesList& dates() const;

            CustomList& drawings();
            const CustomList& drawings() const;

            CustomList& amounts();
            const CustomList& amounts() const;

            CustomList& customStorage(qsizetype customListIndex);
            const CustomList& customStorage(qsizetype customListIndex) const;

            void reset();
            void commit();
            void clear();

            void serialize(QDataStream& out) const override;
            void deserialize(QDataStream& in) override;

        private:
            DatesList m_dateStorage;
            CustomList m_drawingStorage;
            CustomList m_amountStorage;
            QList<CustomList> m_customStorage;
        };
    } // namespace work
} // namespace mdt

#endif // STORAGELISTS_H
