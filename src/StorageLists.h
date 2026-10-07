#ifndef STORAGELISTS_H
#define STORAGELISTS_H

#include "Drawing.h"
#include "ServiceCustomList.h"
#include "ServiceDatesList.h"

enum class CustomStorage : quint8 {
    Executors,
    Authors,
    CastingMaterials,
    ModelMaterials,
    Machines,
    Notes
};

class StorageLists final {
public:
    StorageLists() {
        for (int i = 0; i < 6; ++i) {
            m_customStorage.append(ServiceCustomList<QString>());
        }
    }

    ServiceDatesList& dates() {
        return m_dateStorage;
    }
    const ServiceDatesList& dates() const {
        return m_dateStorage;
    }

    ServiceCustomList<Drawing>& drawings() {
        return m_drawingStorage;
    }
    const ServiceCustomList<Drawing>& drawings() const {
        return m_drawingStorage;
    }

    ServiceCustomList<qint32>& amounts() {
        return m_amountStorage;
    }
    const ServiceCustomList<qint32>& amounts() const {
        return m_amountStorage;
    }

    ServiceCustomList<QString>& customStorage(CustomStorage customStorage) {
        return m_customStorage[static_cast<qsizetype>(customStorage)];
    }
    const ServiceCustomList<QString>& customStorage(CustomStorage customStorage) const {
        return m_customStorage[static_cast<qsizetype>(customStorage)];
    }

    void reset() {
        m_dateStorage.reset();
        m_drawingStorage.reset();
        m_amountStorage.reset();
        for (auto& it : m_customStorage) {
            it.reset();
        }
    }

    void commit() {
        m_dateStorage.commit();
        m_drawingStorage.commit();
        m_amountStorage.commit();
        for (auto& it : m_customStorage) {
            it.commit();
        }
    }

    void clear() {
        m_dateStorage.clear();
        m_drawingStorage.clear();
        m_amountStorage.clear();
        for (auto& it : m_customStorage) {
            it.clear();
        }
    }

    void serialize(QDataStream& out) const {
        m_dateStorage.serialize(out);
        m_drawingStorage.serialize(out);
        m_amountStorage.serialize(out);
        for (auto& it : m_customStorage) {
            it.serialize(out);
        }
    }

    void deserialize(QDataStream& in) {
        m_dateStorage.deserialize(in);
        m_drawingStorage.deserialize(in);
        m_amountStorage.deserialize(in);
        for (auto& it : m_customStorage) {
            it.deserialize(in);
        }
    }

private:
    ServiceDatesList m_dateStorage;
    ServiceCustomList<Drawing> m_drawingStorage;
    ServiceCustomList<qint32> m_amountStorage;
    QList<ServiceCustomList<QString>> m_customStorage;
};

#endif // STORAGELISTS_H
