#include "StorageLists.h"

namespace mdt {
    namespace work {
        StorageLists::StorageLists() {
            for (int i = 0; i < 6; ++i) {
                m_customStorage.append(ServiceCustomList<QString>());
            }
        }

        ServiceDatesList &StorageLists::dates() {
            return m_dateStorage;
        }

        const ServiceDatesList &StorageLists::dates() const {
            return m_dateStorage;
        }

        ServiceCustomList<Drawing> &StorageLists::drawings() {
            return m_drawingStorage;
        }

        const ServiceCustomList<Drawing> &StorageLists::drawings() const {
            return m_drawingStorage;
        }

        ServiceCustomList<qint32> &StorageLists::amounts() {
            return m_amountStorage;
        }

        const ServiceCustomList<qint32> &StorageLists::amounts() const {
            return m_amountStorage;
        }

        ServiceCustomList<QString> &StorageLists::customStorage(CustomStorage customStorage) {
            return m_customStorage[static_cast<qsizetype>(customStorage)];
        }

        const ServiceCustomList<QString> &StorageLists::customStorage(
            CustomStorage customStorage) const {
            return m_customStorage[static_cast<qsizetype>(customStorage)];
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
