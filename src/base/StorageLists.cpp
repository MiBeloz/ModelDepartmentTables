#include "StorageLists.h"

namespace mdt {
    namespace work {
        void StorageLists::setListsCount(qint32 count) {
            m_customStorage.resize(count);
        }

        qsizetype StorageLists::customListCount() const {
            return m_customStorage.count();
        }

        DatesList &StorageLists::dates() {
            return m_dateStorage;
        }

        const DatesList &StorageLists::dates() const {
            return m_dateStorage;
        }

        CustomList &StorageLists::drawings() {
            return m_drawingStorage;
        }

        const CustomList &StorageLists::drawings() const {
            return m_drawingStorage;
        }

        CustomList &StorageLists::amounts() {
            return m_amountStorage;
        }

        const CustomList &StorageLists::amounts() const {
            return m_amountStorage;
        }

        CustomList &StorageLists::customStorage(qsizetype customListIndex) {
            return m_customStorage[static_cast<qsizetype>(customListIndex)];
        }

        const CustomList &StorageLists::customStorage(qsizetype customListIndex) const {
            return m_customStorage[static_cast<qsizetype>(customListIndex)];
        }

        void StorageLists::reset() {
            m_dateStorage.reset();
            m_drawingStorage.reset();
            m_amountStorage.reset();
            for (auto &it : m_customStorage) {
                it.reset();
            }
        }

        void StorageLists::commit() {
            m_dateStorage.commit();
            m_drawingStorage.commit();
            m_amountStorage.commit();
            for (auto &it : m_customStorage) {
                it.commit();
            }
        }

        void StorageLists::clear() {
            m_dateStorage.clear();
            m_drawingStorage.clear();
            m_amountStorage.clear();
            for (auto &it : m_customStorage) {
                it.clear();
            }
        }

        void StorageLists::serialize(QDataStream &out) const {
            m_dateStorage.serialize(out);
            m_drawingStorage.serialize(out);
            m_amountStorage.serialize(out);
            for (auto &it : m_customStorage) {
                it.serialize(out);
            }
        }

        void StorageLists::deserialize(QDataStream &in) {
            m_dateStorage.deserialize(in);
            m_drawingStorage.deserialize(in);
            m_amountStorage.deserialize(in);
            for (auto &it : m_customStorage) {
                it.deserialize(in);
            }
        }
    } // namespace work
} // namespace mdt
