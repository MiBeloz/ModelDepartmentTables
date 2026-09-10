#ifndef record_H
#define record_H

#include <QSet>

#include "StorageService.h"

struct Record {
    explicit Record(const QString& _date,
                    const Drawing& _drawing,
                    int _amount,
                    const QStringList& _executors,
                    const QStringList& _authors,
                    const QStringList& _castingMaterials,
                    const QStringList& _modelMaterials,
                    const QStringList& _machines,
                    const QStringList& _notes)
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
    int amount = 0;
    QStringList executors;
    QStringList authors;
    QStringList castingMaterials;
    QStringList modelMaterials;
    QStringList machines;
    QStringList notes;
};

class LinkRecord {
public:
    explicit LinkRecord(qsizetype idDate, qsizetype idDrawing, qsizetype idAmount)
        : m_idDate(idDate)
        , m_idDrawing(idDrawing)
        , m_idAmount(idAmount) { }

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
        return m_idDate == other.m_idDate && m_idDrawing == other.m_idDrawing &&
               m_idAmount == other.m_idAmount;
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
};

class RecordStorage {
public:
    RecordStorage() = default;

    bool add(const Record& record) {
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
            m_service.dates().remove(record.date);
            return false;
        }

        qsizetype idAmount = 0;
        if (auto id = m_service.amounts().add(record.amount); id.has_value()) {
            idAmount = id.value();
        } else {
            m_service.dates().remove(record.date);
            m_service.drawings().remove(record.drawing);
            return false;
        }

        QSet<qsizetype> idExecutors;
        for (auto it = record.executors.begin(); it != record.executors.end(); ++it) {
            if (auto id = m_service.executors().add(*it); id.has_value()) {
                idExecutors.insert(id.value());
            }
        }

        QSet<qsizetype> idAuthors;
        for (auto it = record.authors.begin(); it != record.authors.end(); ++it) {
            if (auto id = m_service.authors().add(*it); id.has_value()) {
                idAuthors.insert(id.value());
            }
        }

        QSet<qsizetype> idCastingMaterials;
        for (auto it = record.castingMaterials.begin(); it != record.castingMaterials.end(); ++it) {
            if (auto id = m_service.castingMaterials().add(*it); id.has_value()) {
                idCastingMaterials.insert(id.value());
            }
        }

        QSet<qsizetype> idModelMaterials;
        for (auto it = record.modelMaterials.begin(); it != record.modelMaterials.end(); ++it) {
            if (auto id = m_service.modelMaterials().add(*it); id.has_value()) {
                idModelMaterials.insert(id.value());
            }
        }

        QSet<qsizetype> idMachines;
        for (auto it = record.machines.begin(); it != record.machines.end(); ++it) {
            if (auto id = m_service.machines().add(*it); id.has_value()) {
                idMachines.insert(id.value());
            }
        }

        QSet<qsizetype> idNotes;
        for (auto it = record.notes.begin(); it != record.notes.end(); ++it) {
            if (auto id = m_service.notes().add(*it); id.has_value()) {
                idNotes.insert(id.value());
            }
        }

        m_records.insert(LinkRecord(idDate,
                                    idDrawing,
                                    idAmount,
                                    idExecutors,
                                    idAuthors,
                                    idCastingMaterials,
                                    idModelMaterials,
                                    idMachines,
                                    idNotes));

        return true;
    }

    void remove(const Record& record) {
        //m_records.remove(record);
    }

    QSet<Record> get() const {
        return QSet<Record>();
    }

    qsizetype count() const {
        return m_records.count();
    }

    void clear() {
        m_records.clear();
    }

private:
    QSet<LinkRecord> m_records;
    StorageService m_service;
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
