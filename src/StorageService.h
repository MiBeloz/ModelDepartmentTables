#ifndef STORAGESERVICE_H
#define STORAGESERVICE_H

#include "CustomStorage.h"
#include "DatesStorage.h"
#include "Drawing.h"

class StorageService final {
public:
    StorageService()
        : m_dateStorage(std::make_unique<DatesStorage>())
        , m_drawingStorage(std::make_unique<CustomStorage<Drawing>>())
        , m_executorStorage(std::make_unique<CustomStorage<QString>>())
        , m_authorStorage(std::make_unique<CustomStorage<QString>>())
        , m_castingMaterialStorage(std::make_unique<CustomStorage<QString>>())
        , m_modelMaterialStorage(std::make_unique<CustomStorage<QString>>())
        , m_machineStorage(std::make_unique<CustomStorage<QString>>())
        , m_noteStorage(std::make_unique<CustomStorage<QString>>())
        , m_amountStorage(std::make_unique<CustomStorage<qint32>>()) { }

    DatesStorage& dates() {
        return *m_dateStorage;
    }
    const DatesStorage& dates() const {
        return *m_dateStorage;
    }

    CustomStorage<Drawing>& drawings() {
        return *m_drawingStorage;
    }
    const CustomStorage<Drawing>& drawings() const {
        return *m_drawingStorage;
    }

    CustomStorage<QString>& executors() {
        return *m_executorStorage;
    }
    const CustomStorage<QString>& executors() const {
        return *m_executorStorage;
    }

    CustomStorage<QString>& authors() {
        return *m_authorStorage;
    }
    const CustomStorage<QString>& authors() const {
        return *m_authorStorage;
    }

    CustomStorage<QString>& castingMaterials() {
        return *m_castingMaterialStorage;
    }
    const CustomStorage<QString>& castingMaterials() const {
        return *m_castingMaterialStorage;
    }

    CustomStorage<QString>& modelMaterials() {
        return *m_modelMaterialStorage;
    }
    const CustomStorage<QString>& modelMaterials() const {
        return *m_modelMaterialStorage;
    }

    CustomStorage<QString>& machines() {
        return *m_machineStorage;
    }
    const CustomStorage<QString>& machines() const {
        return *m_machineStorage;
    }

    CustomStorage<QString>& notes() {
        return *m_noteStorage;
    }
    const CustomStorage<QString>& notes() const {
        return *m_noteStorage;
    }

    CustomStorage<qint32>& amounts() {
        return *m_amountStorage;
    }
    const CustomStorage<qint32>& amounts() const {
        return *m_amountStorage;
    }

private:
    std::unique_ptr<DatesStorage> m_dateStorage;
    std::unique_ptr<CustomStorage<Drawing>> m_drawingStorage;
    std::unique_ptr<CustomStorage<QString>> m_executorStorage;
    std::unique_ptr<CustomStorage<QString>> m_authorStorage;
    std::unique_ptr<CustomStorage<QString>> m_castingMaterialStorage;
    std::unique_ptr<CustomStorage<QString>> m_modelMaterialStorage;
    std::unique_ptr<CustomStorage<QString>> m_machineStorage;
    std::unique_ptr<CustomStorage<QString>> m_noteStorage;
    std::unique_ptr<CustomStorage<qint32>> m_amountStorage;
};

#endif // STORAGESERVICE_H
