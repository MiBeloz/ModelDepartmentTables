#ifndef record_H
#define record_H

#include "FileStorageSaver.h"

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

class RecordLink {
public:
    explicit RecordLink(qint32 idDate,
                        qint32 idDrawing,
                        qint32 idAmount,
                        const QSet<qint32>& idExecutors,
                        const QSet<qint32>& idAuthors,
                        const QSet<qint32>& idCastingMaterials,
                        const QSet<qint32>& idModelMaterials,
                        const QSet<qint32>& idMachines,
                        const QSet<qint32>& idNotes)
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

    void setIdDate(qint32 idDate) {
        QWriteLocker locker(&m_lock);
        m_idDate = idDate;
    }

    void setIdDrawing(qint32 idDrawing) {
        QWriteLocker locker(&m_lock);
        m_idDrawing = idDrawing;
    }

    void setIdAmount(qint32 idAmount) {
        QWriteLocker locker(&m_lock);
        m_idAmount = idAmount;
    }

    void addExecutors(const QSet<qint32>& idExecutors) {
        QWriteLocker locker(&m_lock);
        m_idExecutors.unite(idExecutors);
    }

    void addAuthors(const QSet<qint32>& idAuthors) {
        QWriteLocker locker(&m_lock);
        m_idAuthors.unite(idAuthors);
    }

    void addCastingMaterials(const QSet<qint32>& idCastingMaterials) {
        QWriteLocker locker(&m_lock);
        m_idCastingMaterials.unite(idCastingMaterials);
    }

    void addModelMaterials(const QSet<qint32>& idModelMaterials) {
        QWriteLocker locker(&m_lock);
        m_idModelMaterials.unite(idModelMaterials);
    }

    void addMachines(const QSet<qint32>& idMachines) {
        QWriteLocker locker(&m_lock);
        m_idMachines.unite(idMachines);
    }

    void addNotes(const QSet<qint32>& idNotes) {
        QWriteLocker locker(&m_lock);
        m_idNotes.unite(idNotes);
    }

    void removeExecutors(const QSet<qint32>& idExecutors) {
        QWriteLocker locker(&m_lock);
        m_idExecutors.subtract(idExecutors);
    }

    void removeAuthors(const QSet<qint32>& idAuthors) {
        QWriteLocker locker(&m_lock);
        m_idAuthors.subtract(idAuthors);
    }

    void removeCastingMaterials(const QSet<qint32>& idCastingMaterials) {
        QWriteLocker locker(&m_lock);
        m_idCastingMaterials.subtract(idCastingMaterials);
    }

    void removeModelMaterials(const QSet<qint32>& idModelMaterials) {
        QWriteLocker locker(&m_lock);
        m_idModelMaterials.subtract(idModelMaterials);
    }

    void removeMachines(const QSet<qint32>& idMachines) {
        QWriteLocker locker(&m_lock);
        m_idMachines.subtract(idMachines);
    }

    void removeNotes(const QSet<qint32>& idNotes) {
        QWriteLocker locker(&m_lock);
        m_idNotes.subtract(idNotes);
    }

    qint32 getIdDate() const {
        QReadLocker locker(&m_lock);
        return m_idDate;
    }

    qint32 getIdDrawing() const {
        QReadLocker locker(&m_lock);
        return m_idDrawing;
    }

    qint32 getIdAmount() const {
        QReadLocker locker(&m_lock);
        return m_idAmount;
    }

    QSet<qint32> getIdExecutors() const {
        QReadLocker locker(&m_lock);
        return m_idExecutors;
    }

    QSet<qint32> getIdAuthors() const {
        QReadLocker locker(&m_lock);
        return m_idAuthors;
    }

    QSet<qint32> getIdCastingMaterials() const {
        QReadLocker locker(&m_lock);
        return m_idCastingMaterials;
    }

    QSet<qint32> getIdModelMaterials() const {
        QReadLocker locker(&m_lock);
        return m_idModelMaterials;
    }

    QSet<qint32> getIdMachines() const {
        QReadLocker locker(&m_lock);
        return m_idMachines;
    }

    QSet<qint32> getIdNotes() const {
        QReadLocker locker(&m_lock);
        return m_idNotes;
    }

    void serialize(QDataStream& out) const {
        QReadLocker locker(&m_lock);

        out << REQUIRED_SERIALIZATION_VERSION;

        out << m_idDate;
        out << m_idDrawing;
        out << m_idAmount;

        out << static_cast<qint32>(m_idExecutors.size());
        for (auto it = m_idExecutors.begin(); it != m_idExecutors.end(); ++it) {
            out << *it;
        }

        out << static_cast<qint32>(m_idAuthors.size());
        for (auto it = m_idAuthors.begin(); it != m_idAuthors.end(); ++it) {
            out << *it;
        }

        out << static_cast<qint32>(m_idCastingMaterials.size());
        for (auto it = m_idCastingMaterials.begin(); it != m_idCastingMaterials.end(); ++it) {
            out << *it;
        }

        out << static_cast<qint32>(m_idModelMaterials.size());
        for (auto it = m_idModelMaterials.begin(); it != m_idModelMaterials.end(); ++it) {
            out << *it;
        }

        out << static_cast<qint32>(m_idMachines.size());
        for (auto it = m_idMachines.begin(); it != m_idMachines.end(); ++it) {
            out << *it;
        }

        out << static_cast<qint32>(m_idNotes.size());
        for (auto it = m_idNotes.begin(); it != m_idNotes.end(); ++it) {
            out << *it;
        }
    }

    void deserialize(QDataStream& in) {
        QWriteLocker locker(&m_lock);

        deserializeVersion(in);

        qint32 tmpDate = deserializeId(in);
        qint32 tmpDrawing = deserializeId(in);
        qint32 tmpAmount = deserializeId(in);
        QSet<qint32> tmpExecutors = deserializeIds(in);
        QSet<qint32> tmpAuthors = deserializeIds(in);
        QSet<qint32> tmpCastingMaterials = deserializeIds(in);
        QSet<qint32> tmpModelMaterials = deserializeIds(in);
        QSet<qint32> tmpMachines = deserializeIds(in);
        QSet<qint32> tmpNotes = deserializeIds(in);

        m_idDate = tmpDate;
        m_idDrawing = tmpDrawing;
        m_idAmount = tmpAmount;
        m_idExecutors = tmpExecutors;
        m_idAuthors = tmpAuthors;
        m_idCastingMaterials = tmpCastingMaterials;
        m_idModelMaterials = tmpModelMaterials;
        m_idMachines = tmpMachines;
        m_idNotes = tmpNotes;
    }

private:
    qint32 m_idDate = 0;
    qint32 m_idDrawing = 0;
    qint32 m_idAmount = 0;
    QSet<qint32> m_idExecutors;
    QSet<qint32> m_idAuthors;
    QSet<qint32> m_idCastingMaterials;
    QSet<qint32> m_idModelMaterials;
    QSet<qint32> m_idMachines;
    QSet<qint32> m_idNotes;
    mutable QReadWriteLock m_lock;

    friend uint qHash(const RecordLink& recordLink, uint seed);

    void throwStreamError(QDataStream::Status status) const {
        throw RuntimeError(
            QObject::tr("QDataStream error. Error code: '%1'.").arg(static_cast<int>(status)));
    }

    void deserializeVersion(QDataStream& in) const {
        quint32 version { };
        in >> version;
        if (in.status() != QDataStream::Ok) {
            throwStreamError(in.status());
        }
        if (version != REQUIRED_SERIALIZATION_VERSION) {
            in.setStatus(QDataStream::Status::ReadCorruptData);
            throw RuntimeError(
                QObject::tr("Version error. Required version: '%1', Current version: '%2'")
                    .arg(QString::number(REQUIRED_SERIALIZATION_VERSION), QString::number(version)));
        }
    }

    qint32 deserializeId(QDataStream& in) const {
        qint32 id { };
        in >> id;
        if (in.status() != QDataStream::Ok) {
            throwStreamError(in.status());
        }
        if (id < 0) {
            in.setStatus(QDataStream::Status::ReadCorruptData);
            throwStreamError(in.status());
        }
        return id;
    }

    QSet<qint32> deserializeIds(QDataStream& in) const {
        qint32 size;
        in >> size;
        if (in.status() != QDataStream::Ok) {
            throwStreamError(in.status());
        }
        if (size < 0) {
            in.setStatus(QDataStream::Status::ReadCorruptData);
            throwStreamError(in.status());
        }

        QSet<qint32> ids;
        for (qint32 i = 0; i < size; ++i) {
            qint32 id;
            in >> id;
            if (in.status() != QDataStream::Ok) {
                throwStreamError(in.status());
            }
            ids.insert(id);
        }
        return ids;
    }
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

        qint32 idDate = 0;
        if (auto id = m_service.dates().add(record.date); id.has_value()) {
            idDate = id.value();
        } else {
            return false;
        }

        qint32 idDrawing = 0;
        if (auto id = m_service.drawings().add(record.drawing); id.has_value()) {
            idDrawing = id.value();
        } else {
            return false;
        }

        qint32 idAmount = 0;
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

    qsizetype count() const {
        return m_recordsLinks.count();
    }

    void clear() {
        m_recordsLinks.clear();
    }

    void print() {
        qDebug() << "LinkRecords:";
        qint32 i = 1;
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
    QSet<qint32> addHelper(const QStringList& values, CustomStorage<T>& storage) {
        QSet<qint32> result;
        for (auto it = values.begin(); it != values.end(); ++it) {
            if (auto id = storage.add(*it); id.has_value()) {
                result.insert(id.value());
            }
        }
        return result;
    }

    template<typename T>
    QSet<qint32> findIdHelper(const QStringList& values, const CustomStorage<T>& storage) const {
        QSet<qint32> result;
        for (auto it = values.begin(); it != values.end(); ++it) {
            if (auto id = storage.findId(*it); id.has_value()) {
                result.insert(id.value());
            }
        }
        return result;
    }
};

#endif // record_H
