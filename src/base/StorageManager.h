#ifndef STORAGEMANAGER_H
#define STORAGEMANAGER_H

#include <QReadWriteLock>
#include <QSet>
#include <QString>

#include "Record.h"
#include "Storage.h"
#include "StorageLists.h"
#include "StorageRecordsLinks.h"

class StorageManager final : public Storage {
public:
    class Adder {
    public:
        explicit Adder(StorageManager& storageManager) : m_storageManager(storageManager) { }

        Adder& date(const QString& date) {
            const QWriteLocker locker(&m_storageManager.m_lock);
            m_storageManager.m_storageLists.dates().add(date);
            return *this;
        }
        Adder& date(const qint32 exelFormat) {
            const QWriteLocker locker(&m_storageManager.m_lock);
            m_storageManager.m_storageLists.dates().add(exelFormat);
            return *this;
        }
        Adder& drawing(const Drawing& drawing) {
            const QWriteLocker locker(&m_storageManager.m_lock);
            m_storageManager.m_storageLists.drawings().add(drawing);
            return *this;
        }
        Adder& amount(const qint32 amount) {
            const QWriteLocker locker(&m_storageManager.m_lock);
            m_storageManager.m_storageLists.amounts().add(amount);
            return *this;
        }
        Adder& executor(const QString& executor) {
            const QWriteLocker locker(&m_storageManager.m_lock);
            m_storageManager.m_storageLists.customStorage(CustomStorage::Executors).add(executor);
            return *this;
        }
        Adder& author(const QString& author) {
            const QWriteLocker locker(&m_storageManager.m_lock);
            m_storageManager.m_storageLists.customStorage(CustomStorage::Authors).add(author);
            return *this;
        }
        Adder& castingMaterial(const QString& castingMaterial) {
            const QWriteLocker locker(&m_storageManager.m_lock);
            m_storageManager.m_storageLists.customStorage(CustomStorage::CastingMaterials)
                .add(castingMaterial);
            return *this;
        }
        Adder& modelMaterial(const QString& modelMaterial) {
            const QWriteLocker locker(&m_storageManager.m_lock);
            m_storageManager.m_storageLists.customStorage(CustomStorage::ModelMaterials)
                .add(modelMaterial);
            return *this;
        }
        Adder& machine(const QString& machine) {
            const QWriteLocker locker(&m_storageManager.m_lock);
            m_storageManager.m_storageLists.customStorage(CustomStorage::Machines).add(machine);
            return *this;
        }
        Adder& note(const QString& note) {
            const QWriteLocker locker(&m_storageManager.m_lock);
            m_storageManager.m_storageLists.customStorage(CustomStorage::Notes).add(note);
            return *this;
        }

    private:
        StorageManager& m_storageManager;
    };

    class Remover {
    public:
        explicit Remover(StorageManager& storageManager) : m_storageManager(storageManager) { }

        Remover& date(const QString& date) {
            const QWriteLocker locker(&m_storageManager.m_lock);
            m_storageManager.m_storageLists.dates().remove(date);
            m_storageManager.deleteBadLinks();
            return *this;
        }
        Remover& date(const qint32 exelFormat) {
            const QWriteLocker locker(&m_storageManager.m_lock);
            m_storageManager.m_storageLists.dates().remove(exelFormat);
            m_storageManager.deleteBadLinks();
            return *this;
        }
        Remover& drawing(const Drawing& drawing) {
            const QWriteLocker locker(&m_storageManager.m_lock);
            m_storageManager.m_storageLists.drawings().remove(drawing);
            m_storageManager.deleteBadLinks();
            return *this;
        }
        Remover& amount(const qint32 amount) {
            const QWriteLocker locker(&m_storageManager.m_lock);
            m_storageManager.m_storageLists.amounts().remove(amount);
            m_storageManager.deleteBadLinks();
            return *this;
        }
        Remover& executor(const QString& executor) {
            const QWriteLocker locker(&m_storageManager.m_lock);
            m_storageManager.m_storageLists.customStorage(CustomStorage::Executors).remove(executor);
            m_storageManager.deleteBadLinksList(IdsList::Executors, CustomStorage::Executors);
            return *this;
        }
        Remover& author(const QString& author) {
            const QWriteLocker locker(&m_storageManager.m_lock);
            m_storageManager.m_storageLists.customStorage(CustomStorage::Authors).remove(author);
            m_storageManager.deleteBadLinksList(IdsList::Authors, CustomStorage::Authors);
            return *this;
        }
        Remover& castingMaterial(const QString& castingMaterial) {
            const QWriteLocker locker(&m_storageManager.m_lock);
            m_storageManager.m_storageLists.customStorage(CustomStorage::CastingMaterials)
                .remove(castingMaterial);
            m_storageManager.deleteBadLinksList(IdsList::CastingMaterials,
                                                CustomStorage::CastingMaterials);
            return *this;
        }
        Remover& modelMaterial(const QString& modelMaterial) {
            const QWriteLocker locker(&m_storageManager.m_lock);
            m_storageManager.m_storageLists.customStorage(CustomStorage::ModelMaterials)
                .remove(modelMaterial);
            m_storageManager.deleteBadLinksList(IdsList::ModelMaterials,
                                                CustomStorage::ModelMaterials);
            return *this;
        }
        Remover& machine(const QString& machine) {
            const QWriteLocker locker(&m_storageManager.m_lock);
            m_storageManager.m_storageLists.customStorage(CustomStorage::Machines).remove(machine);
            m_storageManager.deleteBadLinksList(IdsList::Machines, CustomStorage::Machines);
            return *this;
        }
        Remover& note(const QString& note) {
            const QWriteLocker locker(&m_storageManager.m_lock);
            m_storageManager.m_storageLists.customStorage(CustomStorage::Notes).remove(note);
            m_storageManager.deleteBadLinksList(IdsList::Notes, CustomStorage::Notes);
            return *this;
        }

    private:
        StorageManager& m_storageManager;
    };

    class Getter {
    public:
        explicit Getter(const StorageManager& storageManager) : m_storageManager(storageManager) { }

        auto dates() const {
            const QReadLocker locker(&m_storageManager.m_lock);
            return m_storageManager.m_storageLists.dates().findAllValues();
        }
        auto datesStr() const {
            const QReadLocker locker(&m_storageManager.m_lock);
            return m_storageManager.m_storageLists.dates().findAllStrValues();
        }
        auto datesError() const {
            const QReadLocker locker(&m_storageManager.m_lock);
            return m_storageManager.m_storageLists.dates().lastError();
        }
        auto drawings() const {
            const QReadLocker locker(&m_storageManager.m_lock);
            return m_storageManager.m_storageLists.drawings().findAllValues();
        }
        auto amounts() const {
            const QReadLocker locker(&m_storageManager.m_lock);
            return m_storageManager.m_storageLists.amounts().findAllValues();
        }
        auto executors() const {
            const QReadLocker locker(&m_storageManager.m_lock);
            return m_storageManager.m_storageLists.customStorage(CustomStorage::Executors)
                .findAllValues();
        }
        auto authors() const {
            const QReadLocker locker(&m_storageManager.m_lock);
            return m_storageManager.m_storageLists.customStorage(CustomStorage::Authors)
                .findAllValues();
        }
        auto castingMaterials() const {
            const QReadLocker locker(&m_storageManager.m_lock);
            return m_storageManager.m_storageLists.customStorage(CustomStorage::CastingMaterials)
                .findAllValues();
        }
        auto modelMaterials() const {
            const QReadLocker locker(&m_storageManager.m_lock);
            return m_storageManager.m_storageLists.customStorage(CustomStorage::ModelMaterials)
                .findAllValues();
        }
        auto machines() const {
            const QReadLocker locker(&m_storageManager.m_lock);
            return m_storageManager.m_storageLists.customStorage(CustomStorage::Machines)
                .findAllValues();
        }
        auto notes() const {
            const QReadLocker locker(&m_storageManager.m_lock);
            return m_storageManager.m_storageLists.customStorage(CustomStorage::Notes).findAllValues();
        }

    private:
        const StorageManager& m_storageManager;
    };

    class Counter {
    public:
        explicit Counter(const StorageManager& storageManager)
            : m_storageManager(storageManager) { }

        auto dates() const {
            const QReadLocker locker(&m_storageManager.m_lock);
            return m_storageManager.m_storageLists.dates().count();
        }
        auto drawings() const {
            const QReadLocker locker(&m_storageManager.m_lock);
            return m_storageManager.m_storageLists.drawings().count();
        }
        auto amounts() const {
            const QReadLocker locker(&m_storageManager.m_lock);
            return m_storageManager.m_storageLists.amounts().count();
        }
        auto executors() const {
            const QReadLocker locker(&m_storageManager.m_lock);
            return m_storageManager.m_storageLists.customStorage(CustomStorage::Executors).count();
        }
        auto authors() const {
            const QReadLocker locker(&m_storageManager.m_lock);
            return m_storageManager.m_storageLists.customStorage(CustomStorage::Authors).count();
        }
        auto castingMaterials() const {
            const QReadLocker locker(&m_storageManager.m_lock);
            return m_storageManager.m_storageLists.customStorage(CustomStorage::CastingMaterials)
                .count();
        }
        auto modelMaterials() const {
            const QReadLocker locker(&m_storageManager.m_lock);
            return m_storageManager.m_storageLists.customStorage(CustomStorage::ModelMaterials)
                .count();
        }
        auto machines() const {
            const QReadLocker locker(&m_storageManager.m_lock);
            return m_storageManager.m_storageLists.customStorage(CustomStorage::Machines).count();
        }
        auto notes() const {
            const QReadLocker locker(&m_storageManager.m_lock);
            return m_storageManager.m_storageLists.customStorage(CustomStorage::Notes).count();
        }

    private:
        const StorageManager& m_storageManager;
    };

    StorageManager() = default;

    Adder add() {
        return Adder(*this);
    }

    Remover remove() {
        return Remover(*this);
    }

    Getter get() const {
        return Getter(*this);
    }

    Counter count() const {
        return Counter(*this);
    }

    bool addRecord(const Record& record) {
        if (!record.isValid()) {
            return false;
        }

        const QWriteLocker locker(&m_lock);

        auto idDate = m_storageLists.dates().add(record.date());
        auto idDrawing = m_storageLists.drawings().add(record.drawing());
        auto idAmount = m_storageLists.amounts().add(record.amount());
        if (!idDate.has_value() || !idDrawing.has_value() || !idAmount.has_value()) {
            return false;
        }

        auto idExecutors = addHelper(record.executors(),
                                     m_storageLists.customStorage(CustomStorage::Executors));
        auto idAuthors = addHelper(record.authors(),
                                   m_storageLists.customStorage(CustomStorage::Authors));
        auto idCastingMaterials = addHelper(record.castingMaterials(),
                                            m_storageLists.customStorage(
                                                CustomStorage::CastingMaterials));
        auto idModelMaterials = addHelper(record.modelMaterials(),
                                          m_storageLists.customStorage(
                                              CustomStorage::ModelMaterials));
        auto idMachines = addHelper(record.machines(),
                                    m_storageLists.customStorage(CustomStorage::Machines));
        auto idNotes = addHelper(record.notes(), m_storageLists.customStorage(CustomStorage::Notes));

        RecordLink recordLink(*idDate,
                              *idDrawing,
                              *idAmount,
                              idExecutors,
                              idAuthors,
                              idCastingMaterials,
                              idModelMaterials,
                              idMachines,
                              idNotes);

        m_storageRecordsLinks.add(recordLink);
        m_recordsIsValid = false;
        return true;
    }

    bool removeRecord(const Record& record) {
        if (!record.isValid()) {
            return false;
        }

        const QWriteLocker locker(&m_lock);

        auto idDate = m_storageLists.dates().findId(record.date());
        auto idDrawing = m_storageLists.drawings().findId(record.drawing());
        auto idAmount = m_storageLists.amounts().findId(record.amount());
        if (!idDate.has_value() || !idDrawing.has_value() || !idAmount.has_value()) {
            return false;
        }

        auto idExecutors = findIdHelper(record.executors(),
                                        m_storageLists.customStorage(CustomStorage::Executors));
        auto idAuthors = findIdHelper(record.authors(),
                                      m_storageLists.customStorage(CustomStorage::Authors));
        auto idCastingMaterials = findIdHelper(record.castingMaterials(),
                                               m_storageLists.customStorage(
                                                   CustomStorage::CastingMaterials));
        auto idModelMaterials = findIdHelper(record.modelMaterials(),
                                             m_storageLists.customStorage(
                                                 CustomStorage::ModelMaterials));
        auto idMachines = findIdHelper(record.machines(),
                                       m_storageLists.customStorage(CustomStorage::Machines));
        auto idNotes = findIdHelper(record.notes(),
                                    m_storageLists.customStorage(CustomStorage::Notes));

        RecordLink recordLink(idDate.value(),
                              idDrawing.value(),
                              idAmount.value(),
                              idExecutors,
                              idAuthors,
                              idCastingMaterials,
                              idModelMaterials,
                              idMachines,
                              idNotes);

        m_storageRecordsLinks.remove(recordLink);
        m_recordsIsValid = false;
        return true;
    }

    const QList<Record>& getRecords() const {
        {
            const QReadLocker readLocker(&m_lock);
            if (m_recordsIsValid) {
                return m_records;
            }
        }

        const QWriteLocker writeLocker(&m_lock);
        if (m_recordsIsValid) {
            return m_records;
        }

        m_records.clear();
        const auto recordsLinks = m_storageRecordsLinks.get();
        for (const auto& it : recordsLinks) {
            QString date;
            if (auto v = m_storageLists.dates().findStrValue(it.getIdDate()); v.has_value()) {
                date = v.value();
            } else {
                continue;
            }

            Drawing drawing(Drawing::Null);
            if (auto v = m_storageLists.drawings().findValue(it.getIdDrawing()); v.has_value()) {
                drawing = v.value();
            } else {
                continue;
            }

            qint32 amount { };
            if (auto v = m_storageLists.amounts().findValue(it.getIdAmount()); v.has_value()) {
                amount = v.value();
            } else {
                continue;
            }

            QStringList executors;
            QSet executorsIds = it.getIds(IdsList::Executors);
            for (auto id = executorsIds.begin(); id != executorsIds.end(); ++id) {
                if (auto v = m_storageLists.customStorage(CustomStorage::Executors).findValue(*id);
                    v.has_value()) {
                    executors << v.value();
                }
            }

            QStringList authors;
            QSet authorsIds = it.getIds(IdsList::Authors);
            for (auto id = authorsIds.begin(); id != authorsIds.end(); ++id) {
                if (auto v = m_storageLists.customStorage(CustomStorage::Authors).findValue(*id);
                    v.has_value()) {
                    authors << v.value();
                }
            }

            QStringList castingMaterials;
            QSet castingMaterialsIds = it.getIds(IdsList::CastingMaterials);
            for (auto id = castingMaterialsIds.begin(); id != castingMaterialsIds.end(); ++id) {
                if (auto v =
                        m_storageLists.customStorage(CustomStorage::CastingMaterials).findValue(*id);
                    v.has_value()) {
                    castingMaterials << v.value();
                }
            }

            QStringList modelMaterials;
            QSet modelMaterialsIds = it.getIds(IdsList::ModelMaterials);
            for (auto id = modelMaterialsIds.begin(); id != modelMaterialsIds.end(); ++id) {
                if (auto v =
                        m_storageLists.customStorage(CustomStorage::ModelMaterials).findValue(*id);
                    v.has_value()) {
                    modelMaterials << v.value();
                }
            }

            QStringList machines;
            QSet machinesIds = it.getIds(IdsList::Machines);
            for (auto id = machinesIds.begin(); id != machinesIds.end(); ++id) {
                if (auto v = m_storageLists.customStorage(CustomStorage::Machines).findValue(*id);
                    v.has_value()) {
                    machines << v.value();
                }
            }

            QStringList notes;
            QSet notesIds = it.getIds(IdsList::Notes);
            for (auto id = notesIds.begin(); id != notesIds.end(); ++id) {
                if (auto v = m_storageLists.customStorage(CustomStorage::Notes).findValue(*id);
                    v.has_value()) {
                    notes << v.value();
                }
            }

            Record r(date,
                     drawing,
                     amount,
                     executors,
                     authors,
                     castingMaterials,
                     modelMaterials,
                     machines,
                     notes);

            m_records.append(r);
        }

        m_recordsIsValid = true;
        return m_records;
    }

    void reset() {
        const QWriteLocker locker(&m_lock);
        m_storageRecordsLinks.reset();
        m_storageLists.reset();
        m_recordsIsValid = false;
    }

    void commit() {
        const QWriteLocker locker(&m_lock);
        m_storageRecordsLinks.commit();
        m_storageLists.commit();
    }

    void clear() {
        const QWriteLocker locker(&m_lock);
        m_storageRecordsLinks.clear();
        m_storageLists.clear();
        m_recordsIsValid = false;
    }

    void serialize(QDataStream& out) const override {
        const QReadLocker locker(&m_lock);
        m_storageRecordsLinks.serialize(out);
        m_storageLists.serialize(out);
    }

    void deserialize(QDataStream& in) override {
        const QWriteLocker locker(&m_lock);
        m_storageRecordsLinks.deserialize(in);
        m_storageLists.deserialize(in);
        m_recordsIsValid = false;
    }

    qsizetype countRecords() const {
        const QReadLocker locker(&m_lock);
        return m_storageRecordsLinks.count();
    }

private:
    StorageRecordsLinks m_storageRecordsLinks;
    StorageLists m_storageLists;
    mutable QList<Record> m_records;
    mutable bool m_recordsIsValid = true;
    mutable QReadWriteLock m_lock;
    mutable QReadWriteLock m_cacheLock;

    template<typename T>
    QSet<qint32> addHelper(const QStringList& values, ServiceCustomList<T>& storage) {
        QSet<qint32> result;
        for (auto it = values.begin(); it != values.end(); ++it) {
            if (auto id = storage.add(*it); id.has_value()) {
                result.insert(id.value());
            }
        }
        return result;
    }

    template<typename T>
    QSet<qint32> findIdHelper(const QStringList& values, const ServiceCustomList<T>& storage) const {
        QSet<qint32> result;
        for (auto it = values.begin(); it != values.end(); ++it) {
            if (auto id = storage.findId(*it); id.has_value()) {
                result.insert(id.value());
            }
        }
        return result;
    }

    void deleteBadLinks() {
        const auto links = m_storageRecordsLinks.get();
        for (auto& link : links) {
            if (!m_storageLists.dates().findValue(link.getIdDate()).has_value() ||
                !m_storageLists.drawings().findValue(link.getIdDrawing()).has_value() ||
                !m_storageLists.amounts().findValue(link.getIdAmount()).has_value()) {
                m_storageRecordsLinks.remove(link);
                m_recordsIsValid = false;
            }
        }
    }

    void deleteBadLinksList(IdsList idsList, CustomStorage storage) {
        const auto links = m_storageRecordsLinks.get();
        bool linksChanged = false;
        for (auto link : links) {
            const auto idsOld = link.getIds(idsList);

            QSet<qint32> idsNew;
            for (auto id : idsOld) {
                if (auto value = m_storageLists.customStorage(storage).findValue(id);
                    value.has_value()) {
                    idsNew.insert(id);
                }
            }

            const bool changed = idsNew != idsOld;
            if (!changed) {
                continue;
            }

            m_storageRecordsLinks.remove(link);
            link.replaceIds(idsList, idsNew);
            m_storageRecordsLinks.add(link);

            linksChanged = true;
        }

        if (linksChanged) {
            m_recordsIsValid = false;
        }
    }
};

#endif // STORAGEMANAGER_H
