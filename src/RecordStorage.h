#ifndef record_H
#define record_H

#include "FileStorageSaver.h"

struct Record {
    explicit Record(const QString& _date,
                    const Drawing& _drawing,
                    qint64 _amount,
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
    qint64 amount = 0;
    QStringList executors;
    QStringList authors;
    QStringList castingMaterials;
    QStringList modelMaterials;
    QStringList machines;
    QStringList notes;
};

class RecordLink {
public:
    explicit RecordLink(qint64 idDate,
                        qint64 idDrawing,
                        qint64 idAmount,
                        const QSet<qint64>& idExecutors,
                        const QSet<qint64>& idAuthors,
                        const QSet<qint64>& idCastingMaterials,
                        const QSet<qint64>& idModelMaterials,
                        const QSet<qint64>& idMachines,
                        const QSet<qint64>& idNotes)
        : m_idDate(idDate)
        , m_idDrawing(idDrawing)
        , m_idAmount(idAmount)
        , m_idExecutors(idExecutors)
        , m_idAuthors(idAuthors)
        , m_idCastingMaterials(idCastingMaterials)
        , m_idModelMaterials(idModelMaterials)
        , m_idMachines(idMachines)
        , m_idNotes(idNotes) { }

    RecordLink(const RecordLink& other) {
        m_idDate = other.m_idDate;
        m_idDrawing = other.m_idDrawing;
        m_idAmount = other.m_idAmount;
        m_idExecutors = other.m_idExecutors;
        m_idAuthors = other.m_idAuthors;
        m_idCastingMaterials = other.m_idCastingMaterials;
        m_idModelMaterials = other.m_idModelMaterials;
        m_idMachines = other.m_idMachines;
        m_idNotes = other.m_idNotes;
    }

    bool operator ==(const RecordLink& other) const {
        QReadLocker locker(&m_lock);
        QReadLocker otherLocker(&other.m_lock);
        return m_idDate == other.m_idDate && m_idDrawing == other.m_idDrawing &&
               m_idAmount == other.m_idAmount && m_idExecutors == other.m_idExecutors &&
               m_idAuthors == other.m_idAuthors &&
               m_idCastingMaterials == other.m_idCastingMaterials &&
               m_idModelMaterials == other.m_idModelMaterials &&
               m_idMachines == other.m_idMachines && m_idNotes == other.m_idNotes;
    }

    void setIdDate(qint64 idDate) {
        QWriteLocker locker(&m_lock);
        m_idDate = idDate;
    }

    void setIdDrawing(qint64 idDrawing) {
        QWriteLocker locker(&m_lock);
        m_idDrawing = idDrawing;
    }

    void setIdAmount(qint64 idAmount) {
        QWriteLocker locker(&m_lock);
        m_idAmount = idAmount;
    }

    void addExecutors(const QSet<qint64>& idExecutors) {
        QWriteLocker locker(&m_lock);
        m_idExecutors.unite(idExecutors);
    }

    void addAuthors(const QSet<qint64>& idAuthors) {
        QWriteLocker locker(&m_lock);
        m_idAuthors.unite(idAuthors);
    }

    void addCastingMaterials(const QSet<qint64>& idCastingMaterials) {
        QWriteLocker locker(&m_lock);
        m_idCastingMaterials.unite(idCastingMaterials);
    }

    void addModelMaterials(const QSet<qint64>& idModelMaterials) {
        QWriteLocker locker(&m_lock);
        m_idModelMaterials.unite(idModelMaterials);
    }

    void addMachines(const QSet<qint64>& idMachines) {
        QWriteLocker locker(&m_lock);
        m_idMachines.unite(idMachines);
    }

    void addNotes(const QSet<qint64>& idNotes) {
        QWriteLocker locker(&m_lock);
        m_idNotes.unite(idNotes);
    }

    void removeExecutors(const QSet<qint64>& idExecutors) {
        QWriteLocker locker(&m_lock);
        m_idExecutors.subtract(idExecutors);
    }

    void removeAuthors(const QSet<qint64>& idAuthors) {
        QWriteLocker locker(&m_lock);
        m_idAuthors.subtract(idAuthors);
    }

    void removeCastingMaterials(const QSet<qint64>& idCastingMaterials) {
        QWriteLocker locker(&m_lock);
        m_idCastingMaterials.subtract(idCastingMaterials);
    }

    void removeModelMaterials(const QSet<qint64>& idModelMaterials) {
        QWriteLocker locker(&m_lock);
        m_idModelMaterials.subtract(idModelMaterials);
    }

    void removeMachines(const QSet<qint64>& idMachines) {
        QWriteLocker locker(&m_lock);
        m_idMachines.subtract(idMachines);
    }

    void removeNotes(const QSet<qint64>& idNotes) {
        QWriteLocker locker(&m_lock);
        m_idNotes.subtract(idNotes);
    }

    qint64 getIdDate() const {
        QReadLocker locker(&m_lock);
        return m_idDate;
    }

    qint64 getIdDrawing() const {
        QReadLocker locker(&m_lock);
        return m_idDrawing;
    }

    qint64 getIdAmount() const {
        QReadLocker locker(&m_lock);
        return m_idAmount;
    }

    QSet<qint64> getIdExecutors() const {
        QReadLocker locker(&m_lock);
        return m_idExecutors;
    }

    QSet<qint64> getIdAuthors() const {
        QReadLocker locker(&m_lock);
        return m_idAuthors;
    }

    QSet<qint64> getIdCastingMaterials() const {
        QReadLocker locker(&m_lock);
        return m_idCastingMaterials;
    }

    QSet<qint64> getIdModelMaterials() const {
        QReadLocker locker(&m_lock);
        return m_idModelMaterials;
    }

    QSet<qint64> getIdMachines() const {
        QReadLocker locker(&m_lock);
        return m_idMachines;
    }

    QSet<qint64> getIdNotes() const {
        QReadLocker locker(&m_lock);
        return m_idNotes;
    }

    void serialize(QDataStream& out) const {
        QReadLocker locker(&m_lock);

        out << SERIALIZATION_VERSION;

        out << m_idDate;
        out << m_idDrawing;
        out << m_idAmount;

        out << static_cast<qint64>(m_idExecutors.size());
        for (auto it = m_idExecutors.begin(); it != m_idExecutors.end(); ++it) {
            out << *it;
        }

        out << static_cast<qint64>(m_idAuthors.size());
        for (auto it = m_idAuthors.begin(); it != m_idAuthors.end(); ++it) {
            out << *it;
        }

        out << static_cast<qint64>(m_idCastingMaterials.size());
        for (auto it = m_idCastingMaterials.begin(); it != m_idCastingMaterials.end(); ++it) {
            out << *it;
        }

        out << static_cast<qint64>(m_idModelMaterials.size());
        for (auto it = m_idModelMaterials.begin(); it != m_idModelMaterials.end(); ++it) {
            out << *it;
        }

        out << static_cast<qint64>(m_idMachines.size());
        for (auto it = m_idMachines.begin(); it != m_idMachines.end(); ++it) {
            out << *it;
        }

        out << static_cast<qint64>(m_idNotes.size());
        for (auto it = m_idNotes.begin(); it != m_idNotes.end(); ++it) {
            out << *it;
        }
    }

    void deserialize(QDataStream& in) {
        QWriteLocker locker(&m_lock);

        quint32 version;
        in >> version;
        // TODO
        // Check version

        m_idDate = 0;
        m_idDrawing = 0;
        m_idAmount = 0;
        m_idExecutors.clear();
        m_idAuthors.clear();
        m_idCastingMaterials.clear();
        m_idModelMaterials.clear();
        m_idMachines.clear();
        m_idNotes.clear();

        in >> m_idDate;
        in >> m_idDrawing;
        in >> m_idAmount;

        qint64 executorsSize;
        in >> executorsSize;
        for (qint64 i = 0; i < executorsSize; ++i) {
            qint64 id;
            in >> id;
            m_idExecutors.insert(id);
        }

        qint64 authorsSize;
        in >> authorsSize;
        for (qint64 i = 0; i < authorsSize; ++i) {
            qint64 id;
            in >> id;
            m_idAuthors.insert(id);
        }

        qint64 castingMaterialsSize;
        in >> castingMaterialsSize;
        for (qint64 i = 0; i < castingMaterialsSize; ++i) {
            qint64 id;
            in >> id;
            m_idCastingMaterials.insert(id);
        }

        qint64 modelMaterialsSize;
        in >> modelMaterialsSize;
        for (qint64 i = 0; i < modelMaterialsSize; ++i) {
            qint64 id;
            in >> id;
            m_idModelMaterials.insert(id);
        }

        qint64 machinesSize;
        in >> machinesSize;
        for (qint64 i = 0; i < machinesSize; ++i) {
            qint64 id;
            in >> id;
            m_idMachines.insert(id);
        }

        qint64 notesSize;
        in >> notesSize;
        for (qint64 i = 0; i < notesSize; ++i) {
            qint64 id;
            in >> id;
            m_idNotes.insert(id);
        }
    }

private:
    qint64 m_idDate = 0;
    qint64 m_idDrawing = 0;
    qint64 m_idAmount = 0;
    QSet<qint64> m_idExecutors;
    QSet<qint64> m_idAuthors;
    QSet<qint64> m_idCastingMaterials;
    QSet<qint64> m_idModelMaterials;
    QSet<qint64> m_idMachines;
    QSet<qint64> m_idNotes;
    static constexpr quint32 SERIALIZATION_VERSION = 1;
    mutable QReadWriteLock m_lock;

    friend uint qHash(const RecordLink& recordLink, uint seed);
};

inline uint qHash(const RecordLink& recordLink, uint seed = 0) {
    return qHash(recordLink.m_idDate, seed) ^ qHash(recordLink.m_idDrawing, seed) ^
           qHash(recordLink.m_idAmount, seed) ^ qHash(recordLink.m_idExecutors, seed) ^
           qHash(recordLink.m_idAuthors, seed) ^ qHash(recordLink.m_idCastingMaterials, seed) ^
           qHash(recordLink.m_idModelMaterials, seed) ^ qHash(recordLink.m_idMachines, seed) ^
           qHash(recordLink.m_idNotes, seed);
}

class RecordStorage final {
public:
    RecordStorage()
        : m_fileStorageSaver(STORAGE_SAVER_FILENAME,
                             STORAGE_SAVER_FILENAME_TMP,
                             STORAGE_SAVER_FILENAME_BACKUP) { }

    ~RecordStorage() { }

    RecordStorage(const RecordStorage&) = delete;
    RecordStorage& operator =(const RecordStorage&) = delete;

    bool add(const Record& record) {
        if (!checkDate(record.date) || record.amount < 1) {
            return false;
        }

        qint64 idDate = 0;
        if (auto id = m_service.dates().add(record.date); id.has_value()) {
            idDate = id.value();
        } else {
            return false;
        }

        qint64 idDrawing = 0;
        if (auto id = m_service.drawings().add(record.drawing); id.has_value()) {
            idDrawing = id.value();
        } else {
            return false;
        }

        qint64 idAmount = 0;
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

        RecordLink recordLink(idDate,
                              idDrawing,
                              idAmount,
                              idExecutors,
                              idAuthors,
                              idCastingMaterials,
                              idModelMaterials,
                              idMachines,
                              idNotes);

        if (m_fileStorageSaver.save(m_service)) {
            if (m_fileStorageSaver.commit()) {
                m_recordsLinks.insert(recordLink);
                return true;
            }
        }

        return false;
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

        RecordLink recordLink(idDate.value(),
                              idDrawing.value(),
                              idAmount.value(),
                              idExecutors,
                              idAuthors,
                              idCastingMaterials,
                              idModelMaterials,
                              idMachines,
                              idNotes);

        if (m_recordsLinks.remove(recordLink)) {
            // TODO
            // Check other recordsLinks and remove dead links.
            // Check m_service and remove dead values.

            return true;
        }
        return false;
    }

    QSet<Record> get() const {
        // TODO
        // Return all records
        return QSet<Record>();
    }

    qint64 count() const {
        return static_cast<qint64>(m_recordsLinks.count());
    }

    void clear() {
        m_recordsLinks.clear();
    }

    void print() {
        qDebug() << "LinkRecords:";
        qint64 i = 1;
        for (auto& it : std::as_const(m_recordsLinks)) {
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
    QSet<RecordLink> m_recordsLinks;
    StorageService m_service;
    FileStorageSaver m_fileStorageSaver;

    bool checkDate(const QString& date) const {
        if (auto d = DatesList::strToDate(date); d.has_value()) {
            return true;
        }
        return false;
    }

    template<typename T>
    QSet<qint64> addHelper(const QStringList& values, CustomStorage<T>& storage) {
        QSet<qint64> result;
        for (auto it = values.begin(); it != values.end(); ++it) {
            if (auto id = storage.add(*it); id.has_value()) {
                result.insert(id.value());
            }
        }
        return result;
    }

    template<typename T>
    QSet<qint64> findIdHelper(const QStringList& values, const CustomStorage<T>& storage) const {
        QSet<qint64> result;
        for (auto it = values.begin(); it != values.end(); ++it) {
            if (auto id = storage.findId(*it); id.has_value()) {
                result.insert(id.value());
            }
        }
        return result;
    }
};

#endif // record_H
