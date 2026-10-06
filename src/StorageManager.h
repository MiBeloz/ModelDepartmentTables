#ifndef STORAGEMANAGER_H
#define STORAGEMANAGER_H

#include "Record.h"
#include "StorageLists.h"
#include "StorageRecordsLinks.h"

class StorageManager final {
public:
    class Adder {
    public:
        explicit Adder(StorageManager& storageManager) : m_storageManager(storageManager) { }

        Adder& date(const QString& date) {
            m_storageManager.m_storageLists.dates().add(date);
            return *this;
        }
        Adder& date(const qint32 exelFormat) {
            m_storageManager.m_storageLists.dates().add(exelFormat);
            return *this;
        }
        Adder& drawing(const Drawing& drawing) {
            m_storageManager.m_storageLists.drawings().add(drawing);
            return *this;
        }
        Adder& amount(const qint32 amount) {
            m_storageManager.m_storageLists.amounts().add(amount);
            return *this;
        }
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

        Remover& date(const QString& date) {
            const QWriteLocker locker(&m_storageManager.m_dataLock);
            m_storageManager.m_storageLists.dates().remove(date);
            m_storageManager.deleteBadLinks();
            return *this;
        }
        Remover& date(const qint32 exelFormat) {
            const QWriteLocker locker(&m_storageManager.m_dataLock);
            m_storageManager.m_storageLists.dates().remove(exelFormat);
            m_storageManager.deleteBadLinks();
            return *this;
        }
        Remover& drawing(const Drawing& drawing) {
            const QWriteLocker locker(&m_storageManager.m_dataLock);
            m_storageManager.m_storageLists.drawings().remove(drawing);
            m_storageManager.deleteBadLinks();
            return *this;
        }
        Remover& amount(const qint32 amount) {
            const QWriteLocker locker(&m_storageManager.m_dataLock);
            m_storageManager.m_storageLists.amounts().remove(amount);
            m_storageManager.deleteBadLinks();
            return *this;
        }
        Remover& executor(const QString& executor) {
            const QWriteLocker locker(&m_storageManager.m_dataLock);
            m_storageManager.m_storageLists.executors().remove(executor);
            m_storageManager.deleteBadLinksExecutors();
            return *this;
        }
        Remover& author(const QString& author) {
            const QWriteLocker locker(&m_storageManager.m_dataLock);
            m_storageManager.m_storageLists.authors().remove(author);
            m_storageManager.deleteBadLinksAuthors();
            return *this;
        }
        Remover& castingMaterial(const QString& castingMaterial) {
            const QWriteLocker locker(&m_storageManager.m_dataLock);
            m_storageManager.m_storageLists.castingMaterials().remove(castingMaterial);
            m_storageManager.deleteBadLinksCastingMaterials();
            return *this;
        }
        Remover& modelMaterial(const QString& modelMaterial) {
            const QWriteLocker locker(&m_storageManager.m_dataLock);
            m_storageManager.m_storageLists.modelMaterials().remove(modelMaterial);
            m_storageManager.deleteBadLinksModelMaterials();
            return *this;
        }
        Remover& machine(const QString& machine) {
            const QWriteLocker locker(&m_storageManager.m_dataLock);
            m_storageManager.m_storageLists.machines().remove(machine);
            m_storageManager.deleteBadLinksMachines();
            return *this;
        }
        Remover& note(const QString& note) {
            const QWriteLocker locker(&m_storageManager.m_dataLock);
            m_storageManager.m_storageLists.notes().remove(note);
            m_storageManager.deleteBadLinksNotes();
            return *this;
        }

    private:
        StorageManager& m_storageManager;
    };

    class Getter {
    public:
        explicit Getter(const StorageManager& storageManager) : m_storageManager(storageManager) { }

        auto dates() const {
            return m_storageManager.m_storageLists.dates().findAllValues();
        }
        auto datesStr() const {
            return m_storageManager.m_storageLists.dates().findAllStrValues();
        }
        auto datesError() const {
            return m_storageManager.m_storageLists.dates().lastError();
        }
        auto drawings() const {
            return m_storageManager.m_storageLists.drawings().findAllValues();
        }
        auto amounts() const {
            return m_storageManager.m_storageLists.amounts().findAllValues();
        }
        auto executors() const {
            return m_storageManager.m_storageLists.executors().findAllValues();
        }
        auto authors() const {
            return m_storageManager.m_storageLists.authors().findAllValues();
        }
        auto castingMaterials() const {
            return m_storageManager.m_storageLists.castingMaterials().findAllValues();
        }
        auto modelMaterials() const {
            return m_storageManager.m_storageLists.modelMaterials().findAllValues();
        }
        auto machines() const {
            return m_storageManager.m_storageLists.machines().findAllValues();
        }
        auto notes() const {
            return m_storageManager.m_storageLists.notes().findAllValues();
        }

    private:
        const StorageManager& m_storageManager;
    };

    class Counter {
    public:
        explicit Counter(const StorageManager& storageManager)
            : m_storageManager(storageManager) { }

        auto dates() const {
            return m_storageManager.m_storageLists.dates().count();
        }
        auto drawings() const {
            return m_storageManager.m_storageLists.drawings().count();
        }
        auto amounts() const {
            return m_storageManager.m_storageLists.amounts().count();
        }
        auto executors() const {
            return m_storageManager.m_storageLists.executors().count();
        }
        auto authors() const {
            return m_storageManager.m_storageLists.authors().count();
        }
        auto castingMaterials() const {
            return m_storageManager.m_storageLists.castingMaterials().count();
        }
        auto modelMaterials() const {
            return m_storageManager.m_storageLists.modelMaterials().count();
        }
        auto machines() const {
            return m_storageManager.m_storageLists.machines().count();
        }
        auto notes() const {
            return m_storageManager.m_storageLists.notes().count();
        }

    private:
        const StorageManager& m_storageManager;
    };

    friend class Adder;
    friend class Remover;
    friend class Getter;
    friend class Counter;

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
        const QWriteLocker locker(&m_dataLock);
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

    bool removeRecord(const Record& record) {
        const QWriteLocker locker(&m_dataLock);
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

        m_storageRecordsLinks.remove(recordLink);
        m_recordsIsValid = false;
        return true;
    }

    QList<Record> getRecords() const {
        const QWriteLocker locker(&m_cacheLock);
        if (m_recordsIsValid) {
            return m_records;
        }

        m_records.clear();
        const auto recordsLinks = m_storageRecordsLinks.get();
        for (const auto& it : recordsLinks) {
            QString date;
            if (auto v = m_storageLists.dates().findStrValue(it.get().idDate()); v.has_value()) {
                date = v.value();
            } else {
                continue;
            }

            Drawing drawing(Drawing::Null);
            if (auto v = m_storageLists.drawings().findValue(it.get().idDrawing()); v.has_value()) {
                drawing = v.value();
            } else {
                continue;
            }

            qint32 amount { };
            if (auto v = m_storageLists.amounts().findValue(it.get().idAmount()); v.has_value()) {
                amount = v.value();
            } else {
                continue;
            }

            QStringList executors;
            QSet executorsIds = it.get().idsExecutors();
            for (auto id = executorsIds.begin(); id != executorsIds.end(); ++id) {
                if (auto v = m_storageLists.executors().findValue(*id); v.has_value()) {
                    executors << v.value();
                }
            }

            QStringList authors;
            QSet authorsIds = it.get().idsAuthors();
            for (auto id = authorsIds.begin(); id != authorsIds.end(); ++id) {
                if (auto v = m_storageLists.authors().findValue(*id); v.has_value()) {
                    authors << v.value();
                }
            }

            QStringList castingMaterials;
            QSet castingMaterialsIds = it.get().idsCastingMaterials();
            for (auto id = castingMaterialsIds.begin(); id != castingMaterialsIds.end(); ++id) {
                if (auto v = m_storageLists.castingMaterials().findValue(*id); v.has_value()) {
                    castingMaterials << v.value();
                }
            }

            QStringList modelMaterials;
            QSet modelMaterialsIds = it.get().idsModelMaterials();
            for (auto id = modelMaterialsIds.begin(); id != modelMaterialsIds.end(); ++id) {
                if (auto v = m_storageLists.modelMaterials().findValue(*id); v.has_value()) {
                    modelMaterials << v.value();
                }
            }

            QStringList machines;
            QSet machinesIds = it.get().idsMachines();
            for (auto id = machinesIds.begin(); id != machinesIds.end(); ++id) {
                if (auto v = m_storageLists.machines().findValue(*id); v.has_value()) {
                    machines << v.value();
                }
            }

            QStringList notes;
            QSet notesIds = it.get().idsNotes();
            for (auto id = notesIds.begin(); id != notesIds.end(); ++id) {
                if (auto v = m_storageLists.notes().findValue(*id); v.has_value()) {
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
        const QWriteLocker locker(&m_dataLock);
        m_storageRecordsLinks.reset();
        m_storageLists.reset();
        m_recordsIsValid = false;
    }

    void commit() {
        const QWriteLocker locker(&m_dataLock);
        m_storageRecordsLinks.commit();
        m_storageLists.commit();
    }

    void clear() {
        const QWriteLocker locker(&m_dataLock);
        m_storageRecordsLinks.clear();
        m_storageLists.clear();
        m_recordsIsValid = false;
    }

    void serialize(QDataStream& out) const {
        const QReadLocker locker(&m_dataLock);
        m_storageRecordsLinks.serialize(out);
        m_storageLists.serialize(out);
    }

    void deserialize(QDataStream& in) {
        const QWriteLocker locker(&m_dataLock);
        m_storageRecordsLinks.deserialize(in);
        m_storageLists.deserialize(in);
        m_recordsIsValid = false;
    }

    qsizetype countRecords() const {
        return m_storageRecordsLinks.count();
    }

private:
    StorageRecordsLinks m_storageRecordsLinks;
    StorageLists m_storageLists;
    mutable QList<Record> m_records;
    mutable bool m_recordsIsValid = true;
    mutable QReadWriteLock m_dataLock;
    mutable QReadWriteLock m_cacheLock;

    bool checkRecord(const Record& record) const {
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

    void deleteBadLinks() {
        bool again = true;
        while (again) {
            again = false;

            const auto links = m_storageRecordsLinks.get();
            for (auto& link : links) {
                bool bad = false;
                if (auto date = m_storageLists.dates().findValue(link.get().idDate());
                    !date.has_value()) {
                    bad = true;
                } else if (auto drawing = m_storageLists.drawings().findValue(
                               link.get().idDrawing());
                           !drawing.has_value()) {
                    bad = true;
                } else if (auto amount = m_storageLists.amounts().findValue(link.get().idAmount());
                           !amount.has_value()) {
                    bad = true;
                }

                if (bad) {
                    m_storageRecordsLinks.remove(link);
                    again = true;
                    continue;
                }
            }
        }
    }

    void deleteBadLinksExecutors() {
        const auto links = m_storageRecordsLinks.get();
        bool linksChanged = false;
        for (auto link : links) {
            const auto executorsIdsOld = link.get().idsExecutors();

            QSet<qint32> executorsIdsNew;
            for (auto id : executorsIdsOld) {
                if (auto value = m_storageLists.executors().findValue(id); value.has_value()) {
                    executorsIdsNew.insert(id);
                }
            }

            const bool changed = executorsIdsNew != executorsIdsOld;
            if (!changed) {
                continue;
            }

            m_storageRecordsLinks.remove(link);
            link.replace().idsExecutors(executorsIdsNew);
            m_storageRecordsLinks.add(link);

            linksChanged = true;
        }

        if (linksChanged) {
            m_recordsIsValid = false;
        }
    }

    void deleteBadLinksAuthors() {
        const auto links = m_storageRecordsLinks.get();
        bool linksChanged = false;
        for (auto link : links) {
            const auto authorsIdsOld = link.get().idsAuthors();

            QSet<qint32> authorsIdsNew;
            for (auto id : authorsIdsOld) {
                if (auto value = m_storageLists.authors().findValue(id); value.has_value()) {
                    authorsIdsNew.insert(id);
                }
            }

            const bool changed = authorsIdsNew != authorsIdsOld;
            if (!changed) {
                continue;
            }

            m_storageRecordsLinks.remove(link);
            link.replace().idsAuthors(authorsIdsNew);
            m_storageRecordsLinks.add(link);

            linksChanged = true;
        }

        if (linksChanged) {
            m_recordsIsValid = false;
        }
    }

    void deleteBadLinksCastingMaterials() {
        const auto links = m_storageRecordsLinks.get();
        bool linksChanged = false;
        for (auto link : links) {
            const auto castingMaterialsIdsOld = link.get().idsCastingMaterials();

            QSet<qint32> castingMaterialsIdsNew;
            for (auto id : castingMaterialsIdsOld) {
                if (auto value = m_storageLists.castingMaterials().findValue(id);
                    value.has_value()) {
                    castingMaterialsIdsNew.insert(id);
                }
            }

            const bool changed = castingMaterialsIdsNew != castingMaterialsIdsOld;
            if (!changed) {
                continue;
            }

            m_storageRecordsLinks.remove(link);
            link.replace().idsCastingMaterials(castingMaterialsIdsNew);
            m_storageRecordsLinks.add(link);

            linksChanged = true;
        }

        if (linksChanged) {
            m_recordsIsValid = false;
        }
    }

    void deleteBadLinksModelMaterials() {
        const auto links = m_storageRecordsLinks.get();
        bool linksChanged = false;
        for (auto link : links) {
            const auto modelMaterialsIdsOld = link.get().idsModelMaterials();

            QSet<qint32> modelMaterialsIdsNew;
            for (auto id : modelMaterialsIdsOld) {
                if (auto value = m_storageLists.modelMaterials().findValue(id); value.has_value()) {
                    modelMaterialsIdsNew.insert(id);
                }
            }

            const bool changed = modelMaterialsIdsNew != modelMaterialsIdsOld;
            if (!changed) {
                continue;
            }

            m_storageRecordsLinks.remove(link);
            link.replace().idsModelMaterials(modelMaterialsIdsNew);
            m_storageRecordsLinks.add(link);

            linksChanged = true;
        }

        if (linksChanged) {
            m_recordsIsValid = false;
        }
    }

    void deleteBadLinksMachines() {
        const auto links = m_storageRecordsLinks.get();
        bool linksChanged = false;
        for (auto link : links) {
            const auto machinesIdsOld = link.get().idsMachines();

            QSet<qint32> machinesIdsNew;
            for (auto id : machinesIdsOld) {
                if (auto value = m_storageLists.machines().findValue(id); value.has_value()) {
                    machinesIdsNew.insert(id);
                }
            }

            const bool changed = machinesIdsNew != machinesIdsOld;
            if (!changed) {
                continue;
            }

            m_storageRecordsLinks.remove(link);
            link.replace().idsMachines(machinesIdsNew);
            m_storageRecordsLinks.add(link);

            linksChanged = true;
        }

        if (linksChanged) {
            m_recordsIsValid = false;
        }
    }

    void deleteBadLinksNotes() {
        const auto links = m_storageRecordsLinks.get();
        bool linksChanged = false;
        for (auto link : links) {
            const auto notesIdsOld = link.get().idsNotes();

            QSet<qint32> notesIdsNew;
            for (auto id : notesIdsOld) {
                if (auto value = m_storageLists.notes().findValue(id); value.has_value()) {
                    notesIdsNew.insert(id);
                }
            }

            const bool changed = notesIdsNew != notesIdsOld;
            if (!changed) {
                continue;
            }

            m_storageRecordsLinks.remove(link);
            link.replace().idsNotes(notesIdsNew);
            m_storageRecordsLinks.add(link);

            linksChanged = true;
        }

        if (linksChanged) {
            m_recordsIsValid = false;
        }
    }
};

#endif // STORAGEMANAGER_H
