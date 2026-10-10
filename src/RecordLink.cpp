#include "RecordLink.h"

#include "Exceptions.h"

RecordLink::Adder::Adder(RecordLink &link) : m_link(link) { }

RecordLink::Adder &RecordLink::Adder::idsExecutors(const QSet<qint32> &ids) {
    m_link.addExecutors(ids);
    return *this;
}

RecordLink::Adder &RecordLink::Adder::idsAuthors(const QSet<qint32> &ids) {
    m_link.addAuthors(ids);
    return *this;
}

RecordLink::Adder &RecordLink::Adder::idsCastingMaterials(const QSet<qint32> &ids) {
    m_link.addCastingMaterials(ids);
    return *this;
}

RecordLink::Adder &RecordLink::Adder::idsModelMaterials(const QSet<qint32> &ids) {
    m_link.addModelMaterials(ids);
    return *this;
}

RecordLink::Adder &RecordLink::Adder::idsMachines(const QSet<qint32> &ids) {
    m_link.addMachines(ids);
    return *this;
}

RecordLink::Adder &RecordLink::Adder::idsNotes(const QSet<qint32> &ids) {
    m_link.addNotes(ids);
    return *this;
}

RecordLink::Remover::Remover(RecordLink &link) : m_link(link) { }

RecordLink::Remover &RecordLink::Remover::idsExecutors(const QSet<qint32> &ids) {
    m_link.removeExecutors(ids);
    return *this;
}

RecordLink::Remover &RecordLink::Remover::idsAuthors(const QSet<qint32> &ids) {
    m_link.removeAuthors(ids);
    return *this;
}

RecordLink::Remover &RecordLink::Remover::idsCastingMaterials(const QSet<qint32> &ids) {
    m_link.removeCastingMaterials(ids);
    return *this;
}

RecordLink::Remover &RecordLink::Remover::idsModelMaterials(const QSet<qint32> &ids) {
    m_link.removeModelMaterials(ids);
    return *this;
}

RecordLink::Remover &RecordLink::Remover::idsMachines(const QSet<qint32> &ids) {
    m_link.removeMachines(ids);
    return *this;
}

RecordLink::Remover &RecordLink::Remover::idsNotes(const QSet<qint32> &ids) {
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

QSet<qint32> RecordLink::Getter::idsExecutors() const {
    return m_link.getIdsExecutors();
}

QSet<qint32> RecordLink::Getter::idsAuthors() const {
    return m_link.getIdsAuthors();
}

QSet<qint32> RecordLink::Getter::idsCastingMaterials() const {
    return m_link.getIdsCastingMaterials();
}

QSet<qint32> RecordLink::Getter::idsModelMaterials() const {
    return m_link.getIdsModelMaterials();
}

QSet<qint32> RecordLink::Getter::idsMachines() const {
    return m_link.getIdsMachines();
}

QSet<qint32> RecordLink::Getter::idsNotes() const {
    return m_link.getIdsNotes();
}

RecordLink::Replacer::Replacer(RecordLink &link) : m_link(link) { }

RecordLink::Replacer &RecordLink::Replacer::idDate(qint32 newIdDate) {
    m_link.setIdDate(newIdDate);
    return *this;
}

RecordLink::Replacer &RecordLink::Replacer::idDrawing(qint32 newIdDrawing) {
    m_link.setIdDrawing(newIdDrawing);
    return *this;
}

RecordLink::Replacer &RecordLink::Replacer::idAmount(qint32 newIdAmount) {
    m_link.setIdAmount(newIdAmount);
    return *this;
}

RecordLink::Replacer &RecordLink::Replacer::idsExecutors(const QSet<qint32> &ids) {
    m_link.replaceExecutors(ids);
    return *this;
}

RecordLink::Replacer &RecordLink::Replacer::idsAuthors(const QSet<qint32> &ids) {
    m_link.replaceAuthors(ids);
    return *this;
}

RecordLink::Replacer &RecordLink::Replacer::idsCastingMaterials(const QSet<qint32> &ids) {
    m_link.replaceCastingMaterials(ids);
    return *this;
}

RecordLink::Replacer &RecordLink::Replacer::idsModelMaterials(const QSet<qint32> &ids) {
    m_link.replaceModelMaterials(ids);
    return *this;
}

RecordLink::Replacer &RecordLink::Replacer::idsMachines(const QSet<qint32> &ids) {
    m_link.replaceMachines(ids);
    return *this;
}

RecordLink::Replacer &RecordLink::Replacer::idsNotes(const QSet<qint32> &ids) {
    m_link.replaceNotes(ids);
    return *this;
}

RecordLink::RecordLink(qint32 idDate,
                       qint32 idDrawing,
                       qint32 idAmount,
                       const QSet<qint32> &idsExecutors,
                       const QSet<qint32> &idsAuthors,
                       const QSet<qint32> &idsCastingMaterials,
                       const QSet<qint32> &idsModelMaterials,
                       const QSet<qint32> &idsMachines,
                       const QSet<qint32> &idsNotes)
    : m_idDate(idDate)
    , m_idDrawing(idDrawing)
    , m_idAmount(idAmount)
    , m_idsExecutors(idsExecutors)
    , m_idsAuthors(idsAuthors)
    , m_idsCastingMaterials(idsCastingMaterials)
    , m_idsModelMaterials(idsModelMaterials)
    , m_idsMachines(idsMachines)
    , m_idsNotes(idsNotes) { }

RecordLink::RecordLink(const RecordLink &other) {
    m_idDate = other.m_idDate;
    m_idDrawing = other.m_idDrawing;
    m_idAmount = other.m_idAmount;
    m_idsExecutors = other.m_idsExecutors;
    m_idsAuthors = other.m_idsAuthors;
    m_idsCastingMaterials = other.m_idsCastingMaterials;
    m_idsModelMaterials = other.m_idsModelMaterials;
    m_idsMachines = other.m_idsMachines;
    m_idsNotes = other.m_idsNotes;
}

RecordLink::RecordLink(RecordLink &&other) noexcept {
    m_idDate = other.m_idDate;
    m_idDrawing = other.m_idDrawing;
    m_idAmount = other.m_idAmount;
    m_idsExecutors = std::move(other.m_idsExecutors);
    m_idsAuthors = std::move(other.m_idsAuthors);
    m_idsCastingMaterials = std::move(other.m_idsCastingMaterials);
    m_idsModelMaterials = std::move(other.m_idsModelMaterials);
    m_idsMachines = std::move(other.m_idsMachines);
    m_idsNotes = std::move(other.m_idsNotes);

    other.m_idDate = 0;
    other.m_idDrawing = 0;
    other.m_idAmount = 0;
}

RecordLink &RecordLink::operator =(const RecordLink &other) {
    if (this == &other) {
        return *this;
    }

    m_idDate = other.m_idDate;
    m_idDrawing = other.m_idDrawing;
    m_idAmount = other.m_idAmount;
    m_idsExecutors = other.m_idsExecutors;
    m_idsAuthors = other.m_idsAuthors;
    m_idsCastingMaterials = other.m_idsCastingMaterials;
    m_idsModelMaterials = other.m_idsModelMaterials;
    m_idsMachines = other.m_idsMachines;
    m_idsNotes = other.m_idsNotes;
    return *this;
}

RecordLink &RecordLink::operator =(RecordLink &&other) noexcept {
    if (this == &other) {
        return *this;
    }

    m_idDate = other.m_idDate;
    m_idDrawing = other.m_idDrawing;
    m_idAmount = other.m_idAmount;
    m_idsExecutors = std::move(other.m_idsExecutors);
    m_idsAuthors = std::move(other.m_idsAuthors);
    m_idsCastingMaterials = std::move(other.m_idsCastingMaterials);
    m_idsModelMaterials = std::move(other.m_idsModelMaterials);
    m_idsMachines = std::move(other.m_idsMachines);
    m_idsNotes = std::move(other.m_idsNotes);

    other.m_idDate = 0;
    other.m_idDrawing = 0;
    other.m_idAmount = 0;

    return *this;
}

bool RecordLink::operator ==(const RecordLink &other) const {
    if (this == &other) {
        return true;
    }

    return m_idDate == other.m_idDate && m_idDrawing == other.m_idDrawing &&
           m_idAmount == other.m_idAmount && m_idsExecutors == other.m_idsExecutors &&
           m_idsAuthors == other.m_idsAuthors &&
           m_idsCastingMaterials == other.m_idsCastingMaterials &&
           m_idsModelMaterials == other.m_idsModelMaterials &&
           m_idsMachines == other.m_idsMachines && m_idsNotes == other.m_idsNotes;
}

bool RecordLink::operator !=(const RecordLink &other) const {
    return !(*this == other);
}

void RecordLink::swap(RecordLink &other) {
    if (this == &other) {
        return;
    }

    std::swap(m_idDate, other.m_idDate);
    std::swap(m_idDrawing, other.m_idDrawing);
    std::swap(m_idAmount, other.m_idAmount);
    m_idsExecutors.swap(other.m_idsExecutors);
    m_idsAuthors.swap(other.m_idsAuthors);
    m_idsCastingMaterials.swap(other.m_idsCastingMaterials);
    m_idsModelMaterials.swap(other.m_idsModelMaterials);
    m_idsMachines.swap(other.m_idsMachines);
    m_idsNotes.swap(other.m_idsNotes);
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

RecordLink::Replacer RecordLink::replace() {
    return Replacer(*this);
}

void RecordLink::serialize(QDataStream &out) const {
    out << out.version();

    out << m_idDate;
    out << m_idDrawing;
    out << m_idAmount;

    out << static_cast<qint32>(m_idsExecutors.size());
    for (auto &v : std::as_const(m_idsExecutors)) {
        out << v;
    }

    out << static_cast<qint32>(m_idsAuthors.size());
    for (auto &v : std::as_const(m_idsAuthors)) {
        out << v;
    }

    out << static_cast<qint32>(m_idsCastingMaterials.size());
    for (auto &v : std::as_const(m_idsCastingMaterials)) {
        out << v;
    }

    out << static_cast<qint32>(m_idsModelMaterials.size());
    for (auto &v : std::as_const(m_idsModelMaterials)) {
        out << v;
    }

    out << static_cast<qint32>(m_idsMachines.size());
    for (auto &v : std::as_const(m_idsMachines)) {
        out << v;
    }

    out << static_cast<qint32>(m_idsNotes.size());
    for (auto &v : std::as_const(m_idsNotes)) {
        out << v;
    }

    if (out.status() != QDataStream::Ok) {
        throwStreamError(out.status());
    }
}

void RecordLink::deserialize(QDataStream &in) {
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
    m_idsExecutors = tmpExecutors;
    m_idsAuthors = tmpAuthors;
    m_idsCastingMaterials = tmpCastingMaterials;
    m_idsModelMaterials = tmpModelMaterials;
    m_idsMachines = tmpMachines;
    m_idsNotes = tmpNotes;
}

size_t RecordLink::hash(size_t seed) const {
    return qHash(m_idDate, seed) ^ qHash(m_idDrawing, seed) ^ qHash(m_idAmount, seed) ^
           qHash(m_idsExecutors, seed) ^ qHash(m_idsAuthors, seed) ^
           qHash(m_idsCastingMaterials, seed) ^ qHash(m_idsModelMaterials, seed) ^
           qHash(m_idsMachines, seed) ^ qHash(m_idsNotes, seed);
}

RecordLink::RecordLink() = default;

void RecordLink::setIdDate(qint32 idDate) {
    m_idDate = idDate;
}

void RecordLink::setIdDrawing(qint32 idDrawing) {
    m_idDrawing = idDrawing;
}

void RecordLink::setIdAmount(qint32 idAmount) {
    m_idAmount = idAmount;
}

void RecordLink::addExecutors(const QSet<qint32> &idExecutors) {
    m_idsExecutors.unite(idExecutors);
}

void RecordLink::addAuthors(const QSet<qint32> &idAuthors) {
    m_idsAuthors.unite(idAuthors);
}

void RecordLink::addCastingMaterials(const QSet<qint32> &idCastingMaterials) {
    m_idsCastingMaterials.unite(idCastingMaterials);
}

void RecordLink::addModelMaterials(const QSet<qint32> &idModelMaterials) {
    m_idsModelMaterials.unite(idModelMaterials);
}

void RecordLink::addMachines(const QSet<qint32> &idMachines) {
    m_idsMachines.unite(idMachines);
}

void RecordLink::addNotes(const QSet<qint32> &idNotes) {
    m_idsNotes.unite(idNotes);
}

void RecordLink::removeExecutors(const QSet<qint32> &idExecutors) {
    m_idsExecutors.subtract(idExecutors);
}

void RecordLink::removeAuthors(const QSet<qint32> &idAuthors) {
    m_idsAuthors.subtract(idAuthors);
}

void RecordLink::removeCastingMaterials(const QSet<qint32> &idCastingMaterials) {
    m_idsCastingMaterials.subtract(idCastingMaterials);
}

void RecordLink::removeModelMaterials(const QSet<qint32> &idModelMaterials) {
    m_idsModelMaterials.subtract(idModelMaterials);
}

void RecordLink::removeMachines(const QSet<qint32> &idMachines) {
    m_idsMachines.subtract(idMachines);
}

void RecordLink::removeNotes(const QSet<qint32> &idNotes) {
    m_idsNotes.subtract(idNotes);
}

void RecordLink::replaceExecutors(const QSet<qint32> &idExecutors) {
    m_idsExecutors = idExecutors;
}

void RecordLink::replaceAuthors(const QSet<qint32> &idAuthors) {
    m_idsAuthors = idAuthors;
}

void RecordLink::replaceCastingMaterials(const QSet<qint32> &idCastingMaterials) {
    m_idsCastingMaterials = idCastingMaterials;
}

void RecordLink::replaceModelMaterials(const QSet<qint32> &idModelMaterials) {
    m_idsModelMaterials = idModelMaterials;
}

void RecordLink::replaceMachines(const QSet<qint32> &idMachines) {
    m_idsMachines = idMachines;
}

void RecordLink::replaceNotes(const QSet<qint32> &idNotes) {
    m_idsNotes = idNotes;
}

qint32 RecordLink::getIdDate() const {
    return m_idDate;
}

qint32 RecordLink::getIdDrawing() const {
    return m_idDrawing;
}

qint32 RecordLink::getIdAmount() const {
    return m_idAmount;
}

QSet<qint32> RecordLink::getIdsExecutors() const {
    return m_idsExecutors;
}

QSet<qint32> RecordLink::getIdsAuthors() const {
    return m_idsAuthors;
}

QSet<qint32> RecordLink::getIdsCastingMaterials() const {
    return m_idsCastingMaterials;
}

QSet<qint32> RecordLink::getIdsModelMaterials() const {
    return m_idsModelMaterials;
}

QSet<qint32> RecordLink::getIdsMachines() const {
    return m_idsMachines;
}

QSet<qint32> RecordLink::getIdsNotes() const {
    return m_idsNotes;
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
