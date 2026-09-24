#include "RecordLink.h"

#include "Exceptions.h"

RecordLink::Setter::Setter(RecordLink &link) : m_link(link) { }

RecordLink::Setter &RecordLink::Setter::idDate(qint32 id) {
    m_link.setIdDate(id);
    return *this;
}

RecordLink::Setter &RecordLink::Setter::idDrawing(qint32 id) {
    m_link.setIdDrawing(id);
    return *this;
}

RecordLink::Setter &RecordLink::Setter::idAmount(qint32 id) {
    m_link.setIdAmount(id);
    return *this;
}

RecordLink::Adder::Adder(RecordLink &link) : m_link(link) { }

RecordLink::Adder &RecordLink::Adder::idExecutors(const QSet<qint32> &ids) {
    m_link.addExecutors(ids);
    return *this;
}

RecordLink::Adder &RecordLink::Adder::idAuthors(const QSet<qint32> &ids) {
    m_link.addAuthors(ids);
    return *this;
}

RecordLink::Adder &RecordLink::Adder::idCastingMaterials(const QSet<qint32> &ids) {
    m_link.addCastingMaterials(ids);
    return *this;
}

RecordLink::Adder &RecordLink::Adder::idModelMaterials(const QSet<qint32> &ids) {
    m_link.addModelMaterials(ids);
    return *this;
}

RecordLink::Adder &RecordLink::Adder::idMachines(const QSet<qint32> &ids) {
    m_link.addMachines(ids);
    return *this;
}

RecordLink::Adder &RecordLink::Adder::idNotes(const QSet<qint32> &ids) {
    m_link.addNotes(ids);
    return *this;
}

RecordLink::Remover::Remover(RecordLink &link) : m_link(link) { }

RecordLink::Remover &RecordLink::Remover::removeExecutors(const QSet<qint32> &ids) {
    m_link.removeExecutors(ids);
    return *this;
}

RecordLink::Remover &RecordLink::Remover::removeAuthors(const QSet<qint32> &ids) {
    m_link.removeAuthors(ids);
    return *this;
}

RecordLink::Remover &RecordLink::Remover::removeCastingMaterials(const QSet<qint32> &ids) {
    m_link.removeCastingMaterials(ids);
    return *this;
}

RecordLink::Remover &RecordLink::Remover::removeModelMaterials(const QSet<qint32> &ids) {
    m_link.removeModelMaterials(ids);
    return *this;
}

RecordLink::Remover &RecordLink::Remover::removeMachines(const QSet<qint32> &ids) {
    m_link.removeMachines(ids);
    return *this;
}

RecordLink::Remover &RecordLink::Remover::removeNotes(const QSet<qint32> &ids) {
    m_link.removeNotes(ids);
    return *this;
}

RecordLink::Getter::Getter(const RecordLink &link) : m_link(link) { }

qint32 RecordLink::Getter::idDate() const {
    return m_link.getIdDate();
}

qint32 RecordLink::Getter::idDrawing() const {
    return m_link.getIdDrawing();
}

qint32 RecordLink::Getter::idAmount() const {
    return m_link.getIdAmount();
}

QSet<qint32> RecordLink::Getter::idExecutors() const {
    return m_link.getIdExecutors();
}

QSet<qint32> RecordLink::Getter::idAuthors() const {
    return m_link.getIdAuthors();
}

QSet<qint32> RecordLink::Getter::idCastingMaterials() const {
    return m_link.getIdCastingMaterials();
}

QSet<qint32> RecordLink::Getter::idModelMaterials() const {
    return m_link.getIdModelMaterials();
}

QSet<qint32> RecordLink::Getter::idMachines() const {
    return m_link.getIdMachines();
}

QSet<qint32> RecordLink::Getter::idNotes() const {
    return m_link.getIdNotes();
}

RecordLink::RecordLink(qint32 idDate,
                       qint32 idDrawing,
                       qint32 idAmount,
                       const QSet<qint32> &idExecutors,
                       const QSet<qint32> &idAuthors,
                       const QSet<qint32> &idCastingMaterials,
                       const QSet<qint32> &idModelMaterials,
                       const QSet<qint32> &idMachines,
                       const QSet<qint32> &idNotes)
    : m_idDate(idDate)
    , m_idDrawing(idDrawing)
    , m_idAmount(idAmount)
    , m_idExecutors(idExecutors)
    , m_idAuthors(idAuthors)
    , m_idCastingMaterials(idCastingMaterials)
    , m_idModelMaterials(idModelMaterials)
    , m_idMachines(idMachines)
    , m_idNotes(idNotes) { }

RecordLink::RecordLink(const RecordLink &other) {
    const QReadLocker otherLocker(&other.m_lock);

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

RecordLink::RecordLink(RecordLink &&other) noexcept {
    const QWriteLocker otherLocker(&other.m_lock);

    m_idDate = other.m_idDate;
    m_idDrawing = other.m_idDrawing;
    m_idAmount = other.m_idAmount;
    m_idExecutors = std::move(other.m_idExecutors);
    m_idAuthors = std::move(other.m_idAuthors);
    m_idCastingMaterials = std::move(other.m_idCastingMaterials);
    m_idModelMaterials = std::move(other.m_idModelMaterials);
    m_idMachines = std::move(other.m_idMachines);
    m_idNotes = std::move(other.m_idNotes);

    other.m_idDate = 0;
    other.m_idDrawing = 0;
    other.m_idAmount = 0;
}

RecordLink &RecordLink::operator =(const RecordLink &other) {
    if (this == &other) {
        return *this;
    }

    QReadWriteLock *first = &m_lock;
    QReadWriteLock *second = &other.m_lock;
    if (second < first) {
        std::swap(first, second);
    }

    const QWriteLocker l1(first);
    const QWriteLocker l2(second);

    m_idDate = other.m_idDate;
    m_idDrawing = other.m_idDrawing;
    m_idAmount = other.m_idAmount;
    m_idExecutors = other.m_idExecutors;
    m_idAuthors = other.m_idAuthors;
    m_idCastingMaterials = other.m_idCastingMaterials;
    m_idModelMaterials = other.m_idModelMaterials;
    m_idMachines = other.m_idMachines;
    m_idNotes = other.m_idNotes;
    return *this;
}

RecordLink &RecordLink::operator =(RecordLink &&other) noexcept {
    if (this == &other) {
        return *this;
    }

    QReadWriteLock *first = &m_lock;
    QReadWriteLock *second = &other.m_lock;
    if (second < first) {
        std::swap(first, second);
    }

    const QWriteLocker l1(first);
    const QWriteLocker l2(second);

    m_idDate = other.m_idDate;
    m_idDrawing = other.m_idDrawing;
    m_idAmount = other.m_idAmount;
    m_idExecutors = std::move(other.m_idExecutors);
    m_idAuthors = std::move(other.m_idAuthors);
    m_idCastingMaterials = std::move(other.m_idCastingMaterials);
    m_idModelMaterials = std::move(other.m_idModelMaterials);
    m_idMachines = std::move(other.m_idMachines);
    m_idNotes = std::move(other.m_idNotes);

    other.m_idDate = 0;
    other.m_idDrawing = 0;
    other.m_idAmount = 0;

    return *this;
}

bool RecordLink::operator ==(const RecordLink &other) const {
    if (this == &other) {
        return true;
    }

    QReadWriteLock *first = &m_lock;
    QReadWriteLock *second = &other.m_lock;
    if (second < first) {
        std::swap(first, second);
    }

    const QReadLocker l1(first);
    const QReadLocker l2(second);

    return m_idDate == other.m_idDate && m_idDrawing == other.m_idDrawing &&
           m_idAmount == other.m_idAmount && m_idExecutors == other.m_idExecutors &&
           m_idAuthors == other.m_idAuthors && m_idCastingMaterials == other.m_idCastingMaterials &&
           m_idModelMaterials == other.m_idModelMaterials && m_idMachines == other.m_idMachines &&
           m_idNotes == other.m_idNotes;
}

bool RecordLink::operator !=(const RecordLink &other) const {
    return !(*this == other);
}

void RecordLink::swap(RecordLink &other) {
    if (this == &other) {
        return;
    }

    QReadWriteLock *first = &m_lock;
    QReadWriteLock *second = &other.m_lock;
    if (second < first) {
        std::swap(first, second);
    }

    const QWriteLocker l1(first);
    const QWriteLocker l2(second);

    std::swap(m_idDate, other.m_idDate);
    std::swap(m_idDrawing, other.m_idDrawing);
    std::swap(m_idAmount, other.m_idAmount);
    m_idExecutors.swap(other.m_idExecutors);
    m_idAuthors.swap(other.m_idAuthors);
    m_idCastingMaterials.swap(other.m_idCastingMaterials);
    m_idModelMaterials.swap(other.m_idModelMaterials);
    m_idMachines.swap(other.m_idMachines);
    m_idNotes.swap(other.m_idNotes);
}

RecordLink::Setter RecordLink::set() {
    return Setter(*this);
}

RecordLink::Adder RecordLink::add() {
    return Adder(*this);
}

RecordLink::Remover RecordLink::remove() {
    return Remover(*this);
}

RecordLink::Getter RecordLink::get() const {
    return Getter(*this);
}

void RecordLink::serialize(QDataStream &out) const {
    const QReadLocker locker(&m_lock);

    out << out.version();

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

    if (out.status() != QDataStream::Ok) {
        throwStreamError(out.status());
    }
}

void RecordLink::deserialize(QDataStream &in) {
    const QWriteLocker locker(&m_lock);

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

size_t RecordLink::hash(size_t seed) const {
    const QReadLocker locker(&m_lock);
    return qHash(m_idDate, seed) ^ qHash(m_idDrawing, seed) ^ qHash(m_idAmount, seed) ^
           qHash(m_idExecutors, seed) ^ qHash(m_idAuthors, seed) ^
           qHash(m_idCastingMaterials, seed) ^ qHash(m_idModelMaterials, seed) ^
           qHash(m_idMachines, seed) ^ qHash(m_idNotes, seed);
}

RecordLink::RecordLink() = default;

void RecordLink::setIdDate(qint32 idDate) {
    const QWriteLocker locker(&m_lock);
    m_idDate = idDate;
}

void RecordLink::setIdDrawing(qint32 idDrawing) {
    const QWriteLocker locker(&m_lock);
    m_idDrawing = idDrawing;
}

void RecordLink::setIdAmount(qint32 idAmount) {
    const QWriteLocker locker(&m_lock);
    m_idAmount = idAmount;
}

void RecordLink::addExecutors(const QSet<qint32> &idExecutors) {
    const QWriteLocker locker(&m_lock);
    m_idExecutors.unite(idExecutors);
}

void RecordLink::addAuthors(const QSet<qint32> &idAuthors) {
    const QWriteLocker locker(&m_lock);
    m_idAuthors.unite(idAuthors);
}

void RecordLink::addCastingMaterials(const QSet<qint32> &idCastingMaterials) {
    const QWriteLocker locker(&m_lock);
    m_idCastingMaterials.unite(idCastingMaterials);
}

void RecordLink::addModelMaterials(const QSet<qint32> &idModelMaterials) {
    const QWriteLocker locker(&m_lock);
    m_idModelMaterials.unite(idModelMaterials);
}

void RecordLink::addMachines(const QSet<qint32> &idMachines) {
    const QWriteLocker locker(&m_lock);
    m_idMachines.unite(idMachines);
}

void RecordLink::addNotes(const QSet<qint32> &idNotes) {
    const QWriteLocker locker(&m_lock);
    m_idNotes.unite(idNotes);
}

void RecordLink::removeExecutors(const QSet<qint32> &idExecutors) {
    const QWriteLocker locker(&m_lock);
    m_idExecutors.subtract(idExecutors);
}

void RecordLink::removeAuthors(const QSet<qint32> &idAuthors) {
    const QWriteLocker locker(&m_lock);
    m_idAuthors.subtract(idAuthors);
}

void RecordLink::removeCastingMaterials(const QSet<qint32> &idCastingMaterials) {
    const QWriteLocker locker(&m_lock);
    m_idCastingMaterials.subtract(idCastingMaterials);
}

void RecordLink::removeModelMaterials(const QSet<qint32> &idModelMaterials) {
    const QWriteLocker locker(&m_lock);
    m_idModelMaterials.subtract(idModelMaterials);
}

void RecordLink::removeMachines(const QSet<qint32> &idMachines) {
    const QWriteLocker locker(&m_lock);
    m_idMachines.subtract(idMachines);
}

void RecordLink::removeNotes(const QSet<qint32> &idNotes) {
    const QWriteLocker locker(&m_lock);
    m_idNotes.subtract(idNotes);
}

qint32 RecordLink::getIdDate() const {
    const QReadLocker locker(&m_lock);
    return m_idDate;
}

qint32 RecordLink::getIdDrawing() const {
    const QReadLocker locker(&m_lock);
    return m_idDrawing;
}

qint32 RecordLink::getIdAmount() const {
    const QReadLocker locker(&m_lock);
    return m_idAmount;
}

QSet<qint32> RecordLink::getIdExecutors() const {
    const QReadLocker locker(&m_lock);
    return m_idExecutors;
}

QSet<qint32> RecordLink::getIdAuthors() const {
    const QReadLocker locker(&m_lock);
    return m_idAuthors;
}

QSet<qint32> RecordLink::getIdCastingMaterials() const {
    const QReadLocker locker(&m_lock);
    return m_idCastingMaterials;
}

QSet<qint32> RecordLink::getIdModelMaterials() const {
    const QReadLocker locker(&m_lock);
    return m_idModelMaterials;
}

QSet<qint32> RecordLink::getIdMachines() const {
    const QReadLocker locker(&m_lock);
    return m_idMachines;
}

QSet<qint32> RecordLink::getIdNotes() const {
    const QReadLocker locker(&m_lock);
    return m_idNotes;
}

void RecordLink::throwStreamError(QDataStream::Status status) const {
    throw RuntimeError(
        QObject::tr("QDataStream error. Error code: '%1'.").arg(static_cast<int>(status)));
}

void RecordLink::deserializeVersion(QDataStream &in) const {
    int version { };
    in >> version;
    if (in.status() != QDataStream::Ok) {
        throwStreamError(in.status());
    }
    if (version != in.version()) {
        in.setStatus(QDataStream::Status::ReadCorruptData);
        throw RuntimeError(
            QObject::tr("Version error. Required version: '%1', Current version: '%2'")
                .arg(QString::number(in.version()), QString::number(version)));
    }
}

qint32 RecordLink::deserializeId(QDataStream &in) const {
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

QSet<qint32> RecordLink::deserializeIds(QDataStream &in) const {
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
