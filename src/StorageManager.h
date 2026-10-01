#ifndef STORAGEMANAGER_H
#define STORAGEMANAGER_H

#include "Record.h"
#include "Saver.h"
#include "StorageLists.h"
#include "StorageRecordsLinks.h"

class StorageManager final {
public:
    class Adder {
    public:
        explicit Adder(StorageManager& storageManager) : m_storageManager(storageManager) { }

        Adder& executor(const QString& executor) {
            m_storageManager.m_storageLists.executors().add(executor);
            return *this;
        }
        Adder& author(const QString& author) {
            m_storageManager.m_storageLists.authors().add(author);
            return *this;
        }
        Adder& castingMaterial(const QString& castingMaterial) {
            m_storageManager.m_storageLists.castingMaterials().add(castingMaterial);
            return *this;
        }
        Adder& modelMaterial(const QString& modelMaterial) {
            m_storageManager.m_storageLists.modelMaterials().add(modelMaterial);
            return *this;
        }
        Adder& machine(const QString& machine) {
            m_storageManager.m_storageLists.machines().add(machine);
            return *this;
        }
        Adder& note(const QString& note) {
            m_storageManager.m_storageLists.notes().add(note);
            return *this;
        }

    private:
        StorageManager& m_storageManager;
    };

    class Remover {
    public:
        explicit Remover(StorageManager& storageManager) : m_storageManager(storageManager) { }

        Remover& executor(const QString& executor) {
            m_storageManager.m_storageLists.executors().remove(executor);
            return *this;
        }
        Remover& author(const QString& author) {
            m_storageManager.m_storageLists.authors().remove(author);
            return *this;
        }
        Remover& castingMaterial(const QString& castingMaterial) {
            m_storageManager.m_storageLists.castingMaterials().remove(castingMaterial);
            return *this;
        }
        Remover& modelMaterial(const QString& modelMaterial) {
            m_storageManager.m_storageLists.modelMaterials().remove(modelMaterial);
            return *this;
        }
        Remover& machine(const QString& machine) {
            m_storageManager.m_storageLists.machines().remove(machine);
            return *this;
        }
        Remover& note(const QString& note) {
            m_storageManager.m_storageLists.notes().remove(note);
            return *this;
        }

    private:
        StorageManager& m_storageManager;
    };

    friend class Adder;
    friend class Remover;

    StorageManager(std::unique_ptr<Saver<StorageLists>>& saverLists,
                   std::unique_ptr<Saver<StorageRecordsLinks>>& saverRecordsLinks)
        : m_saverFileStorageLists(std::move(saverLists))
        , m_saverStorageFileRecordsLinks(std::move(saverRecordsLinks)) { }

    ~StorageManager() { }

    StorageManager(const StorageManager&) = delete;
    StorageManager& operator =(const StorageManager&) = delete;

    Adder add() {
        return Adder(*this);
    }

    Remover remove() {
        return Remover(*this);
    }

    bool add(const Record& record) {
        if (!checkRecord(record)) {
            return false;
        }

        auto idDate = m_storageLists.dates().add(record.date);
        auto idDrawing = m_storageLists.drawings().add(record.drawing);
        auto idAmount = m_storageLists.amounts().add(record.amount);
        if (!idDate.has_value() || !idDrawing.has_value() || !idAmount.has_value()) {
            return false;
        }

        auto idExecutors = addHelper(record.executors, m_storageLists.executors());
        auto idAuthors = addHelper(record.authors, m_storageLists.authors());
        auto idCastingMaterials = addHelper(record.castingMaterials,
                                            m_storageLists.castingMaterials());
        auto idModelMaterials = addHelper(record.modelMaterials, m_storageLists.modelMaterials());
        auto idMachines = addHelper(record.machines, m_storageLists.machines());
        auto idNotes = addHelper(record.notes, m_storageLists.notes());

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

    bool remove(const Record& record) {
        if (!checkRecord(record)) {
            return false;
        }

        auto idDate = m_storageLists.dates().findId(record.date);
        auto idDrawing = m_storageLists.drawings().findId(record.drawing);
        auto idAmount = m_storageLists.amounts().findId(record.amount);
        if (!idDate.has_value() || !idDrawing.has_value() || !idAmount.has_value()) {
            return false;
        }

        auto idExecutors = findIdHelper(record.executors, m_storageLists.executors());
        auto idAuthors = findIdHelper(record.authors, m_storageLists.authors());
        auto idCastingMaterials = findIdHelper(record.castingMaterials,
                                               m_storageLists.castingMaterials());
        auto idModelMaterials = findIdHelper(record.modelMaterials, m_storageLists.modelMaterials());
        auto idMachines = findIdHelper(record.machines, m_storageLists.machines());
        auto idNotes = findIdHelper(record.notes, m_storageLists.notes());

        RecordLink recordLink(idDate.value(),
                              idDrawing.value(),
                              idAmount.value(),
                              idExecutors,
                              idAuthors,
                              idCastingMaterials,
                              idModelMaterials,
                              idMachines,
                              idNotes);

        auto oldSize = m_storageRecordsLinks.count();
        m_storageRecordsLinks.remove(recordLink);

        if (oldSize > m_storageRecordsLinks.count()) {
            m_recordsIsValid = false;
            return true;
        }
        return false;
    }

    QList<Record> get() const {
        if (m_recordsIsValid) {
            return m_records;
        }

        m_records.clear();
        const auto recordsLinks = m_storageRecordsLinks.get();
        for (const auto& it : recordsLinks) {
            auto date = m_storageLists.dates().findStrValue(it.get().idDate()).value();
            Drawing drawing(
                m_storageLists.drawings().findValue(it.get().idDrawing()).value().getNumber(),
                m_storageLists.drawings().findValue(it.get().idDrawing()).value().getTitle());
            auto amount = m_storageLists.amounts().findValue(it.get().idAmount()).value();

            QStringList executors;
            QSet executorsIds = it.get().idsExecutors();
            for (auto it = executorsIds.begin(); it != executorsIds.end(); ++it) {
                executors << m_storageLists.executors().findValue(*it).value();
            }

            QStringList authors;
            QSet authorsIds = it.get().idsAuthors();
            for (auto it2 = authorsIds.begin(); it2 != authorsIds.end(); ++it2) {
                authors << m_storageLists.authors().findValue(*it2).value();
            }

            QStringList castingMaterials;
            QSet castingMaterialsIds = it.get().idsCastingMaterials();
            for (auto it2 = castingMaterialsIds.begin(); it2 != castingMaterialsIds.end(); ++it2) {
                castingMaterials << m_storageLists.castingMaterials().findValue(*it2).value();
            }

            QStringList modelMaterials;
            QSet modelMaterialsIds = it.get().idsModelMaterials();
            for (auto it2 = modelMaterialsIds.begin(); it2 != modelMaterialsIds.end(); ++it2) {
                modelMaterials << m_storageLists.modelMaterials().findValue(*it2).value();
            }

            QStringList machines;
            QSet machinesIds = it.get().idsMachines();
            for (auto it2 = machinesIds.begin(); it2 != machinesIds.end(); ++it2) {
                machines << m_storageLists.machines().findValue(*it2).value();
            }

            QStringList notes;
            QSet notesIds = it.get().idsNotes();
            for (auto it2 = notesIds.begin(); it2 != notesIds.end(); ++it2) {
                notes << m_storageLists.notes().findValue(*it2).value();
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

        return m_records;
    }

    bool reset() {
        m_storageRecordsLinks.reset();
        m_storageLists.reset();

        if (m_saverStorageFileRecordsLinks->write(m_storageRecordsLinks) &&
            m_saverFileStorageLists->write(m_storageLists)) {
            m_recordsIsValid = false;
            return true;
        }
        return false;
    }

    bool commit() {
        m_storageRecordsLinks.commit();
        m_storageLists.commit();

        if (m_saverStorageFileRecordsLinks->write(m_storageRecordsLinks) &&
            m_saverFileStorageLists->write(m_storageLists)) {
            return true;
        }

        return false;
    }

    bool save() {
        if (m_saverStorageFileRecordsLinks->save() && m_saverFileStorageLists->save()) {
            return true;
        }
        return false;
    }

    bool load() {
        if (m_saverStorageFileRecordsLinks->read(m_storageRecordsLinks) &&
            m_saverFileStorageLists->read(m_storageLists)) {
            return true;
        }
        return false;
    }

    qsizetype count() const {
        return m_storageRecordsLinks.count();
    }

    void clear() {
        m_storageRecordsLinks.clear();
        m_storageLists.clear();

        m_recordsIsValid = false;
    }

    void deleteBadLinks() {
        const auto links = m_storageRecordsLinks.get();
        bool linksChanged = false;
        for (auto link : links) {
            bool bad = false;
            if (auto date = m_storageLists.dates().findValue(link.get().idDate());
                !date.has_value()) {
                bad = true;
            } else if (auto drawing = m_storageLists.drawings().findValue(link.get().idDrawing());
                       !drawing.has_value()) {
                bad = true;
            } else if (auto amount = m_storageLists.amounts().findValue(link.get().idAmount());
                       !amount.has_value()) {
                bad = true;
            }
            if (bad) {
                m_storageRecordsLinks.remove(link);
                linksChanged = true;
            }

            auto filterIds = [](const QSet<qint32>& ids, const auto& storage) {
                QSet<qint32> result;
                for (auto id : ids) {
                    if (auto value = storage.findValue(id); value.has_value()) {
                        result.insert(id);
                    }
                }
                return result;
            };

            const auto executorsIdsOld = link.get().idsExecutors();
            const auto authorsIdsOld = link.get().idsAuthors();
            const auto castingMaterialsIdsOld = link.get().idsCastingMaterials();
            const auto modelMaterialsIdOld = link.get().idsModelMaterials();
            const auto machinesIdsOld = link.get().idsMachines();
            const auto notesIdsOld = link.get().idsNotes();

            const auto executorsIdsNew = filterIds(executorsIdsOld, m_storageLists.executors());
            const auto authorsIdsNew = filterIds(authorsIdsOld, m_storageLists.authors());
            const auto castingMaterialsIdsNew = filterIds(castingMaterialsIdsOld,
                                                          m_storageLists.castingMaterials());
            const auto modelMaterialsIdNew = filterIds(modelMaterialsIdOld,
                                                       m_storageLists.modelMaterials());
            const auto machinesIdsNew = filterIds(machinesIdsOld, m_storageLists.machines());
            const auto notesIdsNew = filterIds(notesIdsOld, m_storageLists.notes());

            const bool changed = executorsIdsNew != executorsIdsOld ||
                                 authorsIdsNew != authorsIdsOld ||
                                 castingMaterialsIdsNew != castingMaterialsIdsOld ||
                                 modelMaterialsIdNew != modelMaterialsIdOld ||
                                 machinesIdsNew != machinesIdsOld || notesIdsNew != notesIdsOld;

            if (!changed) {
                continue;
            }

            m_storageRecordsLinks.remove(link);
            link.remove()
                .idsExecutors(executorsIdsOld)
                .idsAuthors(authorsIdsOld)
                .idsCastingMaterials(castingMaterialsIdsOld)
                .idsModelMaterials(modelMaterialsIdOld)
                .idsMachines(machinesIdsOld)
                .idsNotes(notesIdsOld);

            link.add()
                .idsExecutors(executorsIdsNew)
                .idsAuthors(authorsIdsNew)
                .idsCastingMaterials(castingMaterialsIdsNew)
                .idsModelMaterials(modelMaterialsIdNew)
                .idsMachines(machinesIdsNew)
                .idsNotes(notesIdsNew);
            m_storageRecordsLinks.add(link);
            linksChanged = true;
        }
        if (linksChanged) {
            m_recordsIsValid = false;
        }
    }

private:
    StorageRecordsLinks m_storageRecordsLinks;
    StorageLists m_storageLists;
    std::unique_ptr<Saver<StorageLists>> m_saverFileStorageLists;
    std::unique_ptr<Saver<StorageRecordsLinks>> m_saverStorageFileRecordsLinks;
    mutable QList<Record> m_records;
    mutable bool m_recordsIsValid = true;

    bool checkRecord(const Record& record) {
        if (!DatesList::checkDate(record.date) || !record.drawing.isValid() || record.amount < 1) {
            return false;
        }
        return true;
    }

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
};

#endif // STORAGEMANAGER_H
