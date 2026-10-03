#ifndef STORAGELISTS_H
#define STORAGELISTS_H

#include "Drawing.h"
#include "ServiceCustomList.h"
#include "ServiceDatesList.h"

class StorageLists final {
public:
    StorageLists()
        : m_dateStorage(std::make_unique<ServiceDatesList>())
        , m_drawingStorage(std::make_unique<ServiceCustomList<Drawing>>())
        , m_amountStorage(std::make_unique<ServiceCustomList<qint32>>())
        , m_executorStorage(std::make_unique<ServiceCustomList<QString>>())
        , m_authorStorage(std::make_unique<ServiceCustomList<QString>>())
        , m_castingMaterialStorage(std::make_unique<ServiceCustomList<QString>>())
        , m_modelMaterialStorage(std::make_unique<ServiceCustomList<QString>>())
        , m_machineStorage(std::make_unique<ServiceCustomList<QString>>())
        , m_noteStorage(std::make_unique<ServiceCustomList<QString>>()) { }

    ServiceDatesList& dates() {
        return *m_dateStorage;
    }
    const ServiceDatesList& dates() const {
        return *m_dateStorage;
    }

    ServiceCustomList<Drawing>& drawings() {
        return *m_drawingStorage;
    }
    const ServiceCustomList<Drawing>& drawings() const {
        return *m_drawingStorage;
    }

    ServiceCustomList<qint32>& amounts() {
        return *m_amountStorage;
    }
    const ServiceCustomList<qint32>& amounts() const {
        return *m_amountStorage;
    }

    ServiceCustomList<QString>& executors() {
        return *m_executorStorage;
    }
    const ServiceCustomList<QString>& executors() const {
        return *m_executorStorage;
    }

    ServiceCustomList<QString>& authors() {
        return *m_authorStorage;
    }
    const ServiceCustomList<QString>& authors() const {
        return *m_authorStorage;
    }

    ServiceCustomList<QString>& castingMaterials() {
        return *m_castingMaterialStorage;
    }
    const ServiceCustomList<QString>& castingMaterials() const {
        return *m_castingMaterialStorage;
    }

    ServiceCustomList<QString>& modelMaterials() {
        return *m_modelMaterialStorage;
    }
    const ServiceCustomList<QString>& modelMaterials() const {
        return *m_modelMaterialStorage;
    }

    ServiceCustomList<QString>& machines() {
        return *m_machineStorage;
    }
    const ServiceCustomList<QString>& machines() const {
        return *m_machineStorage;
    }

    ServiceCustomList<QString>& notes() {
        return *m_noteStorage;
    }
    const ServiceCustomList<QString>& notes() const {
        return *m_noteStorage;
    }

    void reset() {
        m_dateStorage->reset();
        m_drawingStorage->reset();
        m_amountStorage->reset();
        m_executorStorage->reset();
        m_authorStorage->reset();
        m_castingMaterialStorage->reset();
        m_modelMaterialStorage->reset();
        m_machineStorage->reset();
        m_noteStorage->reset();
    }

    void commit() {
        m_dateStorage->commit();
        m_drawingStorage->commit();
        m_amountStorage->commit();
        m_executorStorage->commit();
        m_authorStorage->commit();
        m_castingMaterialStorage->commit();
        m_modelMaterialStorage->commit();
        m_machineStorage->commit();
        m_noteStorage->commit();
    }

    void clear() {
        m_dateStorage->clear();
        m_drawingStorage->clear();
        m_amountStorage->clear();
        m_executorStorage->clear();
        m_authorStorage->clear();
        m_castingMaterialStorage->clear();
        m_modelMaterialStorage->clear();
        m_machineStorage->clear();
        m_noteStorage->clear();
    }

    void serialize(QDataStream& out) const {
        m_dateStorage->serialize(out);
        m_drawingStorage->serialize(out);
        m_amountStorage->serialize(out);
        m_executorStorage->serialize(out);
        m_authorStorage->serialize(out);
        m_castingMaterialStorage->serialize(out);
        m_modelMaterialStorage->serialize(out);
        m_machineStorage->serialize(out);
        m_noteStorage->serialize(out);
    }

    void deserialize(QDataStream& in) {
        m_dateStorage->deserialize(in);
        m_drawingStorage->deserialize(in);
        m_amountStorage->deserialize(in);
        m_executorStorage->deserialize(in);
        m_authorStorage->deserialize(in);
        m_castingMaterialStorage->deserialize(in);
        m_modelMaterialStorage->deserialize(in);
        m_machineStorage->deserialize(in);
        m_noteStorage->deserialize(in);
    }

private:
    std::unique_ptr<ServiceDatesList> m_dateStorage;
    std::unique_ptr<ServiceCustomList<Drawing>> m_drawingStorage;
    std::unique_ptr<ServiceCustomList<qint32>> m_amountStorage;
    std::unique_ptr<ServiceCustomList<QString>> m_executorStorage;
    std::unique_ptr<ServiceCustomList<QString>> m_authorStorage;
    std::unique_ptr<ServiceCustomList<QString>> m_castingMaterialStorage;
    std::unique_ptr<ServiceCustomList<QString>> m_modelMaterialStorage;
    std::unique_ptr<ServiceCustomList<QString>> m_machineStorage;
    std::unique_ptr<ServiceCustomList<QString>> m_noteStorage;
};

#endif // STORAGELISTS_H
