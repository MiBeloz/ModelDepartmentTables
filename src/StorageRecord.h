#ifndef STORAGERECORD_H
#define STORAGERECORD_H

#include "Constants.h"
#include "SaverFileRecordLink.h"
#include "SaverFileStorage.h"
#include "StorageRecordLink.h"

struct Record {
    explicit Record(const QString& _date,
                    const Drawing& _drawing,
                    qint32 _amount,
                    const QStringList& _executors = QStringList(),
                    const QStringList& _authors = QStringList(),
                    const QStringList& _castingMaterials = QStringList(),
                    const QStringList& _modelMaterials = QStringList(),
                    const QStringList& _machines = QStringList(),
                    const QStringList& _notes = QStringList())
        : date(_date)
        , drawing(_drawing)
        , amount(_amount)
        , executors(_executors)
        , authors(_authors)
        , castingMaterials(_castingMaterials)
        , modelMaterials(_modelMaterials)
        , machines(_machines)
        , notes(_notes) { }

    QString date;
    Drawing drawing;
    qint32 amount = 0;
    QStringList executors;
    QStringList authors;
    QStringList castingMaterials;
    QStringList modelMaterials;
    QStringList machines;
    QStringList notes;

    bool operator ==(const Record& other) const {
        return date == other.date && drawing == other.drawing && amount == other.amount &&
               executors == other.executors && authors == other.authors &&
               castingMaterials == other.castingMaterials &&
               modelMaterials == other.modelMaterials && machines == other.machines &&
               notes == other.notes;
    }
};

class StorageRecord final {
public:
    StorageRecord()
        : m_saverFileStorage(SAVER_STORAGE_FILENAME,
                             SAVER_STORAGE_FILENAME_TMP,
                             SAVER_STORAGE_FILENAME_BACKUP)
        , m_saverFileRecordLink(SAVER_RECORDLINK_FILENAME,
                                SAVER_RECORDLINK_FILENAME_TMP,
                                SAVER_RECORDLINK_FILENAME_BACKUP) { }

    ~StorageRecord() { }

    StorageRecord(const StorageRecord&) = delete;
    StorageRecord& operator =(const StorageRecord&) = delete;

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
        m_storageRecordLink.add(recordLink);

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

        auto oldSize = m_storageRecordLink.count();
        m_storageRecordLink.remove(recordLink);

        if (oldSize > m_storageRecordLink.count()) {
            return true;
        }
        return false;
    }

    QList<Record> get() const {
        QList<Record> records;
        const auto recordsLinks = m_storageRecordLink.get();
        for (const auto& it : recordsLinks) {
            auto date = m_storageLists.dates().findStrValue(it.get().idDate()).value();
            Drawing drawing(
                m_storageLists.drawings().findValue(it.get().idDrawing()).value().getNumber(),
                m_storageLists.drawings().findValue(it.get().idDrawing()).value().getTitle());
            auto amount = m_storageLists.amounts().findValue(it.get().idAmount()).value();

            QStringList executors;
            QSet executorsIds = it.get().idExecutors();
            for (auto it = executorsIds.begin(); it != executorsIds.end(); ++it) {
                executors << m_storageLists.executors().findValue(*it).value();
            }

            QStringList authors;
            QSet authorsIds = it.get().idAuthors();
            for (auto it2 = authorsIds.begin(); it2 != authorsIds.end(); ++it2) {
                authors << m_storageLists.authors().findValue(*it2).value();
            }

            QStringList castingMaterials;
            QSet castingMaterialsIds = it.get().idCastingMaterials();
            for (auto it2 = castingMaterialsIds.begin(); it2 != castingMaterialsIds.end(); ++it2) {
                castingMaterials << m_storageLists.castingMaterials().findValue(*it2).value();
            }

            QStringList modelMaterials;
            QSet modelMaterialsIds = it.get().idModelMaterials();
            for (auto it2 = modelMaterialsIds.begin(); it2 != modelMaterialsIds.end(); ++it2) {
                modelMaterials << m_storageLists.modelMaterials().findValue(*it2).value();
            }

            QStringList machines;
            QSet machinesIds = it.get().idMachines();
            for (auto it2 = machinesIds.begin(); it2 != machinesIds.end(); ++it2) {
                machines << m_storageLists.machines().findValue(*it2).value();
            }

            QStringList notes;
            QSet notesIds = it.get().idNotes();
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

            records.append(r);
        }
        return records;
    }

    bool reset() {
        m_storageRecordLink.reset();

        m_storageLists.dates().reset();
        m_storageLists.drawings().reset();
        m_storageLists.amounts().reset();
        m_storageLists.executors().reset();
        m_storageLists.authors().reset();
        m_storageLists.castingMaterials().reset();
        m_storageLists.modelMaterials().reset();
        m_storageLists.machines().reset();
        m_storageLists.notes().reset();

        if (m_saverFileRecordLink.write(m_storageRecordLink) &&
            m_saverFileStorage.write(m_storageLists)) {
            return true;
        }
        return false;
    }

    bool commit() {
        m_storageRecordLink.commit();

        m_storageLists.dates().commit();
        m_storageLists.drawings().commit();
        m_storageLists.amounts().commit();
        m_storageLists.executors().commit();
        m_storageLists.authors().commit();
        m_storageLists.castingMaterials().commit();
        m_storageLists.modelMaterials().commit();
        m_storageLists.machines().commit();
        m_storageLists.notes().commit();

        if (m_saverFileRecordLink.write(m_storageRecordLink) &&
            m_saverFileStorage.write(m_storageLists)) {
            return true;
        }
        return false;
    }

    bool save() {
        if (m_saverFileRecordLink.save() && m_saverFileStorage.save()) {
            return true;
        }
        return false;
    }

    bool load() {
        if (m_saverFileRecordLink.read(m_storageRecordLink) &&
            m_saverFileStorage.read(m_storageLists)) {
            return true;
        }
        return false;
    }

    qsizetype count() const {
        return m_storageRecordLink.count();
    }

    void clear() {
        m_storageRecordLink.clear();

        m_storageLists.dates().clear();
        m_storageLists.drawings().clear();
        m_storageLists.amounts().clear();
        m_storageLists.executors().clear();
        m_storageLists.authors().clear();
        m_storageLists.castingMaterials().clear();
        m_storageLists.modelMaterials().clear();
        m_storageLists.machines().clear();
        m_storageLists.notes().clear();
    }

    void deleteBadLinks() {
        const auto links = m_storageRecordLink.get();
        for (const auto& link : links) {
            {
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
                    m_storageRecordLink.remove(link);
                }
            }

            {
                bool bad = false;
                for (const auto executorId : link.get().idExecutors()) {
                    if (auto executor = m_storageLists.executors().findValue(executorId);
                        !executor.has_value()) {
                        // TODO
                    }
                }
            }
        }
    }

private:
    StorageRecordLink m_storageRecordLink;
    StorageLists m_storageLists;
    SaverFileStorage m_saverFileStorage;
    SaverFileRecordLink m_saverFileRecordLink;

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

#endif // STORAGERECORD_H
