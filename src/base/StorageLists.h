#ifndef STORAGELISTS_H
#define STORAGELISTS_H

#include "Drawing.h"
#include "ServiceCustomList.h"
#include "ServiceDatesList.h"
#include "Storage.h"

enum class CustomStorage : quint8 {
    Executors,
    Authors,
    CastingMaterials,
    ModelMaterials,
    Machines,
    Notes
};

class StorageLists final : public Storage {
public:
    StorageLists();

    ServiceDatesList& dates();
    const ServiceDatesList& dates() const;

    ServiceCustomList<Drawing>& drawings();
    const ServiceCustomList<Drawing>& drawings() const;

    ServiceCustomList<qint32>& amounts();
    const ServiceCustomList<qint32>& amounts() const;

    ServiceCustomList<QString>& customStorage(CustomStorage customStorage);
    const ServiceCustomList<QString>& customStorage(CustomStorage customStorage) const;

    void reset();
    void commit();
    void clear();

    void serialize(QDataStream& out) const override;
    void deserialize(QDataStream& in) override;

private:
    ServiceDatesList m_dateStorage;
    ServiceCustomList<Drawing> m_drawingStorage;
    ServiceCustomList<qint32> m_amountStorage;
    QList<ServiceCustomList<QString>> m_customStorage;
};

#endif // STORAGELISTS_H
