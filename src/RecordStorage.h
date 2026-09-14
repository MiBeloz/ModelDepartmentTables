#ifndef record_H
#define record_H

#include <QSet>

#include "Constants.h"
#include "FileStorageSaver.h"
#include "StorageService.h"

struct Record {
    explicit Record(const QString& _date,
                    const Drawing& _drawing,
                    qsizetype _amount,
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
    qsizetype amount = 0;
    QStringList executors;
    QStringList authors;
    QStringList castingMaterials;
    QStringList modelMaterials;
    QStringList machines;
    QStringList notes;
};

class LinkRecord {
public:
    explicit LinkRecord(qsizetype idDate,
                        qsizetype idDrawing,
                        qsizetype idAmount,
                        const QSet<qsizetype>& idExecutors,
                        const QSet<qsizetype>& idAuthors,
                        const QSet<qsizetype>& idCastingMaterials,
                        const QSet<qsizetype>& idModelMaterials,
                        const QSet<qsizetype>& idMachines,
                        const QSet<qsizetype>& idNotes)
        : m_idDate(idDate)
        , m_idDrawing(idDrawing)
        , m_idAmount(idAmount)
        , m_idExecutors(idExecutors)
        , m_idAuthors(idAuthors)
        , m_idCastingMaterials(idCastingMaterials)
        , m_idModelMaterials(idModelMaterials)
        , m_idMachines(idMachines)
        , m_idNotes(idNotes) { }

    bool operator ==(const LinkRecord& other) const {
        return m_idDate && m_idDrawing && m_idAmount;
    }

    void setIdDate(qsizetype idDate) {
        m_idDate = idDate;
    }

    void setIdDrawing(qsizetype idDrawing) {
        m_idDrawing = idDrawing;
    }

    void setIdAmount(qsizetype idAmount) {
        m_idAmount = idAmount;
    }

    void addExecutors(const QSet<qsizetype>& idExecutors) {
        m_idExecutors.unite(idExecutors);
    }

    void addAuthors(const QSet<qsizetype>& idAuthors) {
        m_idAuthors.unite(idAuthors);
    }

    void addCastingMaterials(const QSet<qsizetype>& idCastingMaterials) {
        m_idCastingMaterials.unite(idCastingMaterials);
    }

    void addModelMaterials(const QSet<qsizetype>& idModelMaterials) {
        m_idModelMaterials.unite(idModelMaterials);
    }

    void addMachines(const QSet<qsizetype>& idMachines) {
        m_idMachines.unite(idMachines);
    }

    void addNotes(const QSet<qsizetype>& idNotes) {
        m_idNotes.unite(idNotes);
    }

    void removeExecutors(const QSet<qsizetype>& idExecutors) {
        m_idExecutors.subtract(idExecutors);
    }

    void removeAuthors(const QSet<qsizetype>& idAuthors) {
        m_idAuthors.subtract(idAuthors);
    }

    void removeCastingMaterials(const QSet<qsizetype>& idCastingMaterials) {
        m_idCastingMaterials.subtract(idCastingMaterials);
    }

    void removeModelMaterials(const QSet<qsizetype>& idModelMaterials) {
        m_idModelMaterials.subtract(idModelMaterials);
    }

    void removeMachines(const QSet<qsizetype>& idMachines) {
        m_idMachines.subtract(idMachines);
    }

    void removeNotes(const QSet<qsizetype>& idNotes) {
        m_idNotes.subtract(idNotes);
    }

    qsizetype getIdDate() const {
        return m_idDate;
    }

    qsizetype getIdDrawing() const {
        return m_idDrawing;
    }

    qsizetype getIdAmount() const {
        return m_idAmount;
    }

    QSet<qsizetype> getIdExecutors() const {
        return m_idExecutors;
    }

    QSet<qsizetype> getIdAuthors() const {
        return m_idAuthors;
    }

    QSet<qsizetype> getIdCastingMaterials() const {
        return m_idCastingMaterials;
    }

    QSet<qsizetype> getIdModelMaterials() const {
        return m_idModelMaterials;
    }

    QSet<qsizetype> getIdMachines() const {
        return m_idMachines;
    }

    QSet<qsizetype> getIdNotes() const {
        return m_idNotes;
    }

private:
    qsizetype m_idDate = 0;
    qsizetype m_idDrawing = 0;
    qsizetype m_idAmount = 0;
    QSet<qsizetype> m_idExecutors;
    QSet<qsizetype> m_idAuthors;
    QSet<qsizetype> m_idCastingMaterials;
    QSet<qsizetype> m_idModelMaterials;
    QSet<qsizetype> m_idMachines;
    QSet<qsizetype> m_idNotes;

    friend uint qHash(const LinkRecord& linkRecord, uint seed);
};

inline uint qHash(const LinkRecord& linkRecord, uint seed = 0) {
    return qHash(linkRecord.m_idDate, seed) ^ qHash(linkRecord.m_idDrawing, seed) ^
           qHash(linkRecord.m_idAmount, seed) ^ qHash(linkRecord.m_idExecutors, seed) ^
           qHash(linkRecord.m_idAuthors, seed) ^ qHash(linkRecord.m_idCastingMaterials, seed) ^
           qHash(linkRecord.m_idModelMaterials, seed) ^ qHash(linkRecord.m_idMachines, seed) ^
           qHash(linkRecord.m_idNotes, seed);
}

class RecordStorage final {
public:
    RecordStorage()
        : m_fileServiceSaver(StorageSaverFilename, StorageSaverFilenameTmp, StorageSaverFilenameBackup) { }

    ~RecordStorage() { }

    bool add(const Record& record) {
        if (!checkDate(record.date) || record.amount < 1) {
            return false;
        }

        qsizetype idDate = 0;
        if (auto id = m_service.dates().add(record.date); id.has_value()) {
            idDate = id.value();
        } else {
            return false;
        }

        qsizetype idDrawing = 0;
        if (auto id = m_service.drawings().add(record.drawing); id.has_value()) {
            idDrawing = id.value();
        } else {
            return false;
        }

        qsizetype idAmount = 0;
        if (auto id = m_service.amounts().add(record.amount); id.has_value()) {
            idAmount = id.value();
        } else {
            return false;
        }

        auto idExecutors = addHelper(record.executors, m_service.executors());
        auto idAuthors = addHelper(record.authors, m_service.authors());
        auto idCastingMaterials = addHelper(record.castingMaterials, m_service.castingMaterials());
        auto idModelMaterials = addHelper(record.modelMaterials, m_service.modelMaterials());
        auto idMachines = addHelper(record.machines, m_service.machines());
        auto idNotes = addHelper(record.notes, m_service.notes());

        LinkRecord linkRecord(idDate,
                              idDrawing,
                              idAmount,
                              idExecutors,
                              idAuthors,
                              idCastingMaterials,
                              idModelMaterials,
                              idMachines,
                              idNotes);

        m_linkRecords.insert(linkRecord);

        m_fileServiceSaver.save(m_service);
        // if (m_fileServiceSaver.save(m_service) && m_fileServiceSaver.commit()) {
        //     return true;
        // }
        // m_fileLinkRecordSaverTmp.save(m_linkRecords);

        return true;
    }

    bool remove(const Record& record) {
        if (!checkDate(record.date) || record.amount < 1) {
            return false;
        }

        auto idDate = m_service.dates().findId(record.date);
        auto idDrawing = m_service.drawings().findId(record.drawing);
        auto idAmount = m_service.amounts().findId(record.amount);
        if (!idDate.has_value() || !idDrawing.has_value() || !idAmount.has_value()) {
            return false;
        }

        auto idExecutors = findIdHelper(record.executors, m_service.executors());
        auto idAuthors = findIdHelper(record.authors, m_service.authors());
        auto idCastingMaterials = findIdHelper(record.castingMaterials,
                                               m_service.castingMaterials());
        auto idModelMaterials = findIdHelper(record.modelMaterials, m_service.modelMaterials());
        auto idMachines = findIdHelper(record.machines, m_service.machines());
        auto idNotes = findIdHelper(record.notes, m_service.notes());

        LinkRecord linkRecord(idDate.value(),
                              idDrawing.value(),
                              idAmount.value(),
                              idExecutors,
                              idAuthors,
                              idCastingMaterials,
                              idModelMaterials,
                              idMachines,
                              idNotes);

        if (m_linkRecords.remove(linkRecord)) {
            // TODO
            // Check other linkRecords and remove dead links.
        }
        return false;
    }

    QSet<Record> get() const {
        return QSet<Record>();
    }

    qsizetype count() const {
        return m_linkRecords.count();
    }

    void clear() {
        m_linkRecords.clear();
    }

    void print() {
        qDebug() << "LinkRecords:";
        qsizetype i = 1;
        for (auto it : m_linkRecords) {
            qDebug() << "\tLinkRecord" << i++;
            qDebug() << "\t\tDate =" << it.getIdDate() << "-"
                     << m_service.dates().findStrValue(it.getIdDate()).value();
            qDebug() << "\t\tDrawing =" << it.getIdDrawing() << "-"
                     << m_service.drawings().findValue(it.getIdDrawing()).value().getNumber() << "-"
                     << m_service.drawings().findValue(it.getIdDrawing()).value().getTitle();
            qDebug() << "\t\tAmount =" << it.getIdAmount() << "-"
                     << m_service.amounts().findValue(it.getIdAmount()).value();

            QSet executors = it.getIdExecutors();
            qDebug() << "\t\tExecutors =" << it.getIdExecutors() << "-";
            for (auto it2 = executors.begin(); it2 != executors.end(); ++it2) {
                qDebug() << "\t\t\t" << m_service.executors().findValue(*it2).value();
            }

            QSet authors = it.getIdAuthors();
            qDebug() << "\t\tAuthors =" << it.getIdAuthors() << "-";
            for (auto it2 = authors.begin(); it2 != authors.end(); ++it2) {
                qDebug() << "\t\t\t" << m_service.authors().findValue(*it2).value();
            }

            QSet castingMaterials = it.getIdCastingMaterials();
            qDebug() << "\t\tCastingMaterials =" << it.getIdCastingMaterials() << "-";
            for (auto it2 = castingMaterials.begin(); it2 != castingMaterials.end(); ++it2) {
                qDebug() << "\t\t\t" << m_service.castingMaterials().findValue(*it2).value();
            }

            QSet modelMaterials = it.getIdModelMaterials();
            qDebug() << "\t\tModelMaterials =" << it.getIdModelMaterials() << "-";
            for (auto it2 = modelMaterials.begin(); it2 != modelMaterials.end(); ++it2) {
                qDebug() << "\t\t\t" << m_service.modelMaterials().findValue(*it2).value();
            }

            QSet machines = it.getIdMachines();
            qDebug() << "\t\tMachines =" << it.getIdMachines() << "-";
            for (auto it2 = machines.begin(); it2 != machines.end(); ++it2) {
                qDebug() << "\t\t\t" << m_service.machines().findValue(*it2).value();
            }

            QSet notes = it.getIdNotes();
            qDebug() << "\t\tNotes =" << it.getIdNotes() << "-";
            for (auto it2 = notes.begin(); it2 != notes.end(); ++it2) {
                qDebug() << "\t\t\t" << m_service.notes().findValue(*it2).value();
            }
        }
    }

private:
    QSet<LinkRecord> m_linkRecords;
    StorageService m_service;
    FileStorageSaver m_fileServiceSaver;

    bool checkDate(const QString& date) {
        if (auto d = DatesList::strToDate(date); d.has_value()) {
            return true;
        }
        return false;
    }

    template<typename T>
    QSet<qsizetype> addHelper(const QStringList& values, CustomStorage<T>& storage) const {
        QSet<qsizetype> result;
        for (auto it = values.begin(); it != values.end(); ++it) {
            if (auto id = storage.add(*it); id.has_value()) {
                result.insert(id.value());
            }
        }
        return result;
    }

    template<typename T>
    QSet<qsizetype> findIdHelper(const QStringList& values, CustomStorage<T>& storage) const {
        QSet<qsizetype> result;
        for (auto it = values.begin(); it != values.end(); ++it) {
            if (auto id = storage.findId(*it); id.has_value()) {
                result.insert(id.value());
            }
        }
        return result;
    }
};

// class RecordData {
// public:
//     RecordData(const RecordStorage& recordStorage) : m_storage(recordStorage) { }

// private:
//     RecordStorage m_storage;
//     StorageService m_service;
// };

// inline QTextStream& operator <<(QTextStream& out, const Drawing& drawing) {
//     out << "[Name: " << drawing.getNumber() << ", Title: " << drawing.getTitle() << "]";
//     return out;
// }

// inline QDataStream& operator <<(QDataStream& out, const Drawing* data) {
//     out << data->getNumber();
//     out << data->getTitle();
//     return out;
// }

#endif // record_H
