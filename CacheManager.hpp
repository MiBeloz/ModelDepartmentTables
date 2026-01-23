#ifndef CACHEMANAGER_H
#define CACHEMANAGER_H

#include <QDir>

#include "CacheLocker.hpp"
#include "ItemData.h"


class CacheManager {
public:
    CacheManager() {
        m_caches.append(std::make_shared<ThreadSafeCache<uint>>("date", true));
        m_caches.append(std::make_shared<ThreadSafeCache<Drawing>>("drawing", true));
        m_caches.append(std::make_shared<ThreadSafeCache<QString>>("executor", true));
        m_caches.append(std::make_shared<ThreadSafeCache<QString>>("author", true));
        m_caches.append(std::make_shared<ThreadSafeCache<uint>>("amount", true));
        m_caches.append(std::make_shared<ThreadSafeCache<QString>>("castingMaterial", true));
        m_caches.append(std::make_shared<ThreadSafeCache<QString>>("modelMaterial", true));
        m_caches.append(std::make_shared<ThreadSafeCache<QString>>("machine", true));
        m_caches.append(std::make_shared<ThreadSafeCache<QString>>("note", true));
        m_caches.append(std::make_shared<ThreadSafeCache<Item>>("item", true));

        m_caches.append(std::make_shared<ThreadSafeCache<uint>>("itemExecutor", false));
        m_caches.append(std::make_shared<ThreadSafeCache<uint>>("itemAuthor", false));
        m_caches.append(std::make_shared<ThreadSafeCache<uint>>("itemCastingMaterial", false));
        m_caches.append(std::make_shared<ThreadSafeCache<uint>>("itemModelMaterial", false));
        m_caches.append(std::make_shared<ThreadSafeCache<uint>>("itemMachine", false));
        m_caches.append(std::make_shared<ThreadSafeCache<uint>>("itemNote", false));
    }

    std::shared_ptr<AbstractCache> getCache(int index) const {
        if (index >= 0 && index < m_caches.size()) {
            return m_caches[index];
        }
        return nullptr;
    }

    std::shared_ptr<AbstractCache> getCache(const QString& name) const {
        for (const auto& cache : m_caches) {
            if (cache->name() == name) {
                return cache;
            }
        }
        return nullptr;
    }

    template<typename Data>
    std::shared_ptr<ThreadSafeCache<Data>> getTypedCache(int index) const {
        if (index >= 0 && index < m_caches.size()) {
            return std::dynamic_pointer_cast<ThreadSafeCache<Data>>(m_caches[index]);
        }
        return nullptr;
    }

    // QList<WorkItem> Cache::getWorkItems() const {
    //     QList<WorkItem> workItems;
    //     for (qsizetype i = 0; i < m_item_cache.size(); ++i) {
    //         uint date = m_workDate_cache.getValue(m_item_cache.getValue(i).date_id);
    //         Drawing drawing = m_drawing_cache.getValue(m_item_cache.getValue(i).drawing_id);
    //         uint amount = m_amount_cache.getValue(m_item_cache.getValue(i).amount_id);

    //         QStringList executors;
    //         QList<uint> executors_id = m_itemExecutor_cache.getValues(i);
    //         for (const auto& executor_id : std::as_const(executors_id)) {
    //             executors << m_executor_cache.getValue(executor_id);
    //         }

    //     QStringList castingMaterials;
    //     QList<int> castingMaterials_id = m_itemCastingMaterial_cache.values(it.value());
    //     for (const auto& castingMaterial_id : std::as_const(castingMaterials_id)) {
    //         castingMaterials << m_castingMaterial_cache.key(castingMaterial_id);
    //     }

    //     QStringList modelMaterials;
    //     QList<int> modelMaterials_id = m_itemModelMaterial_cache.values(it.value());
    //     for (const auto& modelMaterial_id : std::as_const(modelMaterials_id)) {
    //         modelMaterials << m_modelMaterial_cache.key(modelMaterial_id);
    //     }

    //     QStringList machines;
    //     QList<int> machines_id = m_itemMachine_cache.values(it.value());
    //     for (const auto& machine_id : std::as_const(machines_id)) {
    //         machines << m_machine_cache.key(machine_id);
    //     }

    //     QStringList notes;
    //     QList<int> notes_id = m_itemNote_cache.values(it.value());
    //     for (const auto& note_id : std::as_const(notes_id)) {
    //         notes << m_note_cache.key(note_id);
    //     }

    //     workItems.append(WorkItem { Item { date, drawing, amount }, executors, castingMaterials, modelMaterials, machines, notes });
    //}
    //return workItems;

private:
    QVector<std::shared_ptr<AbstractCache>> m_caches;
};

#endif // CACHEMANAGER_H
