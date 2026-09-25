#ifndef STORAGESERVICE_H
#define STORAGESERVICE_H

#include "ServiceCustomList.h"
#include "ServiceDatesList.h"
#include "Drawing.h"

class StorageService final {
public:
    StorageService()
        : m_dateStorage(std::make_unique<ServiceDatesList>())
        , m_drawingStorage(std::make_unique<ServiceCustomList<Drawing>>())
        , m_executorStorage(std::make_unique<ServiceCustomList<QString>>())
        , m_authorStorage(std::make_unique<ServiceCustomList<QString>>())
        , m_castingMaterialStorage(std::make_unique<ServiceCustomList<QString>>())
        , m_modelMaterialStorage(std::make_unique<ServiceCustomList<QString>>())
        , m_machineStorage(std::make_unique<ServiceCustomList<QString>>())
        , m_noteStorage(std::make_unique<ServiceCustomList<QString>>())
        , m_amountStorage(std::make_unique<ServiceCustomList<qint32>>()) { }

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

    ServiceCustomList<qint32>& amounts() {
        return *m_amountStorage;
    }
    const ServiceCustomList<qint32>& amounts() const {
        return *m_amountStorage;
    }

private:
    std::unique_ptr<ServiceDatesList> m_dateStorage;
    std::unique_ptr<ServiceCustomList<Drawing>> m_drawingStorage;
    std::unique_ptr<ServiceCustomList<QString>> m_executorStorage;
    std::unique_ptr<ServiceCustomList<QString>> m_authorStorage;
    std::unique_ptr<ServiceCustomList<QString>> m_castingMaterialStorage;
    std::unique_ptr<ServiceCustomList<QString>> m_modelMaterialStorage;
    std::unique_ptr<ServiceCustomList<QString>> m_machineStorage;
    std::unique_ptr<ServiceCustomList<QString>> m_noteStorage;
    std::unique_ptr<ServiceCustomList<qint32>> m_amountStorage;
};

#endif // STORAGESERVICE_H
