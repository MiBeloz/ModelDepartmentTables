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
        if (checkRecord(record)) {
            return false;
        }

        auto idDate = m_storageService.dates().add(record.date);
        auto idDrawing = m_storageService.drawings().add(record.drawing);
        auto idAmount = m_storageService.amounts().add(record.amount);
        if (!idDate.has_value() || !idDrawing.has_value() || !idAmount.has_value()) {
            return false;
        }

        auto idExecutors = addHelper(record.executors, m_storageService.executors());
        auto idAuthors = addHelper(record.authors, m_storageService.authors());
        auto idCastingMaterials = addHelper(record.castingMaterials,
                                            m_storageService.castingMaterials());
        auto idModelMaterials = addHelper(record.modelMaterials, m_storageService.modelMaterials());
        auto idMachines = addHelper(record.machines, m_storageService.machines());
        auto idNotes = addHelper(record.notes, m_storageService.notes());

        RecordLink recordLink(*idDate,
                              *idDrawing,
                              *idAmount,
                              idExecutors,
                              idAuthors,
                              idCastingMaterials,
                              idModelMaterials,
                              idMachines,
                              idNotes);
        m_StorageRecordLink.add(recordLink);

        m_saverFileStorage.save(m_storageService);
        return true;
    }

    bool remove(const Record& record) {
        if (checkRecord(record)) {
            return false;
        }

        auto idDate = m_storageService.dates().findId(record.date);
        auto idDrawing = m_storageService.drawings().findId(record.drawing);
        auto idAmount = m_storageService.amounts().findId(record.amount);
        if (!idDate.has_value() || !idDrawing.has_value() || !idAmount.has_value()) {
            return false;
        }

        auto idExecutors = findIdHelper(record.executors, m_storageService.executors());
        auto idAuthors = findIdHelper(record.authors, m_storageService.authors());
        auto idCastingMaterials = findIdHelper(record.castingMaterials,
                                               m_storageService.castingMaterials());
        auto idModelMaterials = findIdHelper(record.modelMaterials,
                                             m_storageService.modelMaterials());
        auto idMachines = findIdHelper(record.machines, m_storageService.machines());
        auto idNotes = findIdHelper(record.notes, m_storageService.notes());

        RecordLink recordLink(idDate.value(),
                              idDrawing.value(),
                              idAmount.value(),
                              idExecutors,
                              idAuthors,
                              idCastingMaterials,
                              idModelMaterials,
                              idMachines,
                              idNotes);

        auto oldSize = m_StorageRecordLink.count();
        m_StorageRecordLink.remove(recordLink);

        if (oldSize > m_StorageRecordLink.count()) {
            m_saverFileStorage.save(m_storageService);
            return true;
        }
        return false;
    }

    QSet<Record> get() const {
        // TODO
        // Return all records
        return QSet<Record>();
    }

    void reset() {
        m_StorageRecordLink.reset();
        m_storageService.dates().reset();
        m_storageService.drawings().reset();
        m_storageService.amounts().reset();
        m_storageService.executors().reset();
        m_storageService.authors().reset();
        m_storageService.castingMaterials().reset();
        m_storageService.modelMaterials().reset();
        m_storageService.machines().reset();
        m_storageService.notes().reset();
    }

    void commit() {
        m_StorageRecordLink.commit();
        m_storageService.dates().commit();
        m_storageService.drawings().commit();
        m_storageService.amounts().commit();
        m_storageService.executors().commit();
        m_storageService.authors().commit();
        m_storageService.castingMaterials().commit();
        m_storageService.modelMaterials().commit();
        m_storageService.machines().commit();
        m_storageService.notes().commit();
    }

    bool save() {
        if (m_saverFileStorage.save(m_storageService) &&
            m_saverFileRecordLink.save(m_StorageRecordLink) && m_saverFileStorage.commit() &&
            m_saverFileRecordLink.commit()) {
            return true;
        }
        return false;
    }

    qsizetype count() const {
        return m_StorageRecordLink.count();
    }

    void clear() {
        m_StorageRecordLink.clear();
    }

    void deleteBadLinks() {
        // TODO
        // Ddelete all bad links
    }

    void print() {
        qDebug() << "LinkRecords:";
        qint32 i = 1;
        auto recordsLinks = m_StorageRecordLink.get();
        for (auto& it : recordsLinks) {
            qDebug() << "\tLinkRecord" << i++;
            qDebug() << "\t\tDate =" << it.get().idDate() << "-"
                     << m_storageService.dates().findStrValue(it.get().idDate()).value();
            qDebug()
                << "\t\tDrawing =" << it.get().idDrawing() << "-"
                << m_storageService.drawings().findValue(it.get().idDrawing()).value().getNumber()
                << "-"
                << m_storageService.drawings().findValue(it.get().idDrawing()).value().getTitle();
            qDebug() << "\t\tAmount =" << it.get().idAmount() << "-"
                     << m_storageService.amounts().findValue(it.get().idAmount()).value();

            QSet executors = it.get().idExecutors();
            qDebug() << "\t\tExecutors =" << it.get().idExecutors() << "-";
            for (auto it2 = executors.begin(); it2 != executors.end(); ++it2) {
                qDebug() << "\t\t\t" << m_storageService.executors().findValue(*it2).value();
            }

            QSet authors = it.get().idAuthors();
            qDebug() << "\t\tAuthors =" << it.get().idAuthors() << "-";
            for (auto it2 = authors.begin(); it2 != authors.end(); ++it2) {
                qDebug() << "\t\t\t" << m_storageService.authors().findValue(*it2).value();
            }

            QSet castingMaterials = it.get().idCastingMaterials();
            qDebug() << "\t\tCastingMaterials =" << it.get().idCastingMaterials() << "-";
            for (auto it2 = castingMaterials.begin(); it2 != castingMaterials.end(); ++it2) {
                qDebug() << "\t\t\t" << m_storageService.castingMaterials().findValue(*it2).value();
            }

            QSet modelMaterials = it.get().idModelMaterials();
            qDebug() << "\t\tModelMaterials =" << it.get().idModelMaterials() << "-";
            for (auto it2 = modelMaterials.begin(); it2 != modelMaterials.end(); ++it2) {
                qDebug() << "\t\t\t" << m_storageService.modelMaterials().findValue(*it2).value();
            }

            QSet machines = it.get().idMachines();
            qDebug() << "\t\tMachines =" << it.get().idMachines() << "-";
            for (auto it2 = machines.begin(); it2 != machines.end(); ++it2) {
                qDebug() << "\t\t\t" << m_storageService.machines().findValue(*it2).value();
            }

            QSet notes = it.get().idNotes();
            qDebug() << "\t\tNotes =" << it.get().idNotes() << "-";
            for (auto it2 = notes.begin(); it2 != notes.end(); ++it2) {
                qDebug() << "\t\t\t" << m_storageService.notes().findValue(*it2).value();
            }
        }
    }

private:
    StorageRecordLink m_StorageRecordLink;
    StorageService m_storageService;
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
