#include "StorageManager.h"

mdt::work::StorageManager *mdt::work::StorageManager::m_instance = nullptr;

void mdt::work::StorageManager::initInstance() {
    if (!m_instance) {
        m_instance = new StorageManager;
    }
}

void mdt::work::StorageManager::freeInstance() {
    delete m_instance;
    m_instance = nullptr;
}

mdt::work::StorageManager *mdt::work::StorageManager::instance() {
    return m_instance;
}

mdt::work::StorageManager::Adder &mdt::work::StorageManager::Adder::date(const QString &date) {
    const QWriteLocker locker(&m_instance->m_lock);
    m_instance->m_storageLists.dates().insert(date);
    return *this;
}

mdt::work::StorageManager::Adder &mdt::work::StorageManager::Adder::date(const qint32 exelFormat) {
    const QWriteLocker locker(&m_instance->m_lock);
    m_instance->m_storageLists.dates().insert(exelFormat);
    return *this;
}

mdt::work::StorageManager::Adder &mdt::work::StorageManager::Adder::drawing(const Drawing &drawing) {
    const QWriteLocker locker(&m_instance->m_lock);
    m_instance->m_storageLists.drawings().insert(drawing);
    return *this;
}

mdt::work::StorageManager::Adder &mdt::work::StorageManager::Adder::amount(const qint32 amount) {
    const QWriteLocker locker(&m_instance->m_lock);
    m_instance->m_storageLists.amounts().insert(amount);
    return *this;
}

mdt::work::StorageManager::Adder &mdt::work::StorageManager::Adder::custom(qsizetype customIndex,
                                                                           const QVariant &data) {
    const QWriteLocker locker(&m_instance->m_lock);
    m_instance->m_storageLists.customStorage(customIndex).insert(data);
    return *this;
}

mdt::work::StorageManager::Remover &mdt::work::StorageManager::Remover::date(const QString &date) {
    const QWriteLocker locker(&m_instance->m_lock);
    m_instance->m_storageLists.dates().remove(date);
    m_instance->deleteBadLinks();
    return *this;
}

mdt::work::StorageManager::Remover &mdt::work::StorageManager::Remover::date(
    const qint32 exelFormat) {
    const QWriteLocker locker(&m_instance->m_lock);
    m_instance->m_storageLists.dates().remove(exelFormat);
    m_instance->deleteBadLinks();
    return *this;
}

mdt::work::StorageManager::Remover &mdt::work::StorageManager::Remover::drawing(
    const Drawing &drawing) {
    const QWriteLocker locker(&m_instance->m_lock);
    m_instance->m_storageLists.drawings().remove(drawing);
    m_instance->deleteBadLinks();
    return *this;
}

mdt::work::StorageManager::Remover &mdt::work::StorageManager::Remover::amount(const qint32 amount) {
    const QWriteLocker locker(&m_instance->m_lock);
    m_instance->m_storageLists.amounts().remove(amount);
    m_instance->deleteBadLinks();
    return *this;
}

mdt::work::StorageManager::Remover &mdt::work::StorageManager::Remover::custom(qsizetype customIndex,
                                                                               const QVariant &data) {
    const QWriteLocker locker(&m_instance->m_lock);
    m_instance->m_storageLists.customStorage(customIndex).remove(data);
    m_instance->deleteBadLinksList(customIndex);
    return *this;
}

QVariantList mdt::work::StorageManager::Getter::dates() const {
    const QReadLocker locker(&m_instance->m_lock);
    return m_instance->m_storageLists.dates().getAllValues();
}

QStringList mdt::work::StorageManager::Getter::datesStr() const {
    const QReadLocker locker(&m_instance->m_lock);
    return m_instance->m_storageLists.dates().getAllStrValues();
}

mdt::work::DatesList::ErrorType mdt::work::StorageManager::Getter::datesError() const {
    const QReadLocker locker(&m_instance->m_lock);
    return m_instance->m_storageLists.dates().lastError();
}

QVariantList mdt::work::StorageManager::Getter::drawings() const {
    const QReadLocker locker(&m_instance->m_lock);
    return m_instance->m_storageLists.drawings().getAllValues();
}

QVariantList mdt::work::StorageManager::Getter::amounts() const {
    const QReadLocker locker(&m_instance->m_lock);
    return m_instance->m_storageLists.amounts().getAllValues();
}

QVariantList mdt::work::StorageManager::Getter::custom(qsizetype customIndex) const {
    const QReadLocker locker(&m_instance->m_lock);
    return m_instance->m_storageLists.customStorage(customIndex).getAllValues();
}

qsizetype mdt::work::StorageManager::Counter::dates() const {
    const QReadLocker locker(&m_instance->m_lock);
    return m_instance->m_storageLists.dates().size();
}

qsizetype mdt::work::StorageManager::Counter::drawings() const {
    const QReadLocker locker(&m_instance->m_lock);
    return m_instance->m_storageLists.drawings().size();
}

qsizetype mdt::work::StorageManager::Counter::amounts() const {
    const QReadLocker locker(&m_instance->m_lock);
    return m_instance->m_storageLists.amounts().size();
}

qsizetype mdt::work::StorageManager::Counter::custom(qsizetype customIndex) const {
    const QReadLocker locker(&m_instance->m_lock);
    return m_instance->m_storageLists.customStorage(customIndex).size();
}

mdt::work::StorageManager::Adder mdt::work::StorageManager::add() {
    return Adder();
}

mdt::work::StorageManager::Remover mdt::work::StorageManager::remove() {
    return Remover();
}

mdt::work::StorageManager::Getter mdt::work::StorageManager::get() const {
    return Getter();
}

mdt::work::StorageManager::Counter mdt::work::StorageManager::count() const {
    return Counter();
}

bool mdt::work::StorageManager::addRecord(const Record &record) {
    if (!record.isValid()) {
        return false;
    }

    const QWriteLocker locker(&m_lock);

    auto idDate = m_storageLists.dates().insert(record.date());
    auto idDrawing = m_storageLists.drawings().insert(record.drawing());
    auto idAmount = m_storageLists.amounts().insert(record.amount());
    if (!idDate.has_value() || !idDrawing.has_value() || !idAmount.has_value()) {
        return false;
    }

    auto idExecutors = addHelper(record.executors(),
                                 m_storageLists.customStorage(CustomStorage::Executors));
    auto idAuthors = addHelper(record.authors(),
                               m_storageLists.customStorage(CustomStorage::Authors));
    auto idCastingMaterials = addHelper(record.castingMaterials(),
                                        m_storageLists.customStorage(
                                            CustomStorage::CastingMaterials));
    auto idModelMaterials = addHelper(record.modelMaterials(),
                                      m_storageLists.customStorage(CustomStorage::ModelMaterials));
    auto idMachines = addHelper(record.machines(),
                                m_storageLists.customStorage(CustomStorage::Machines));
    auto idNotes = addHelper(record.notes(), m_storageLists.customStorage(CustomStorage::Notes));

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

bool mdt::work::StorageManager::removeRecord(const Record &record) {
    if (!record.isValid()) {
        return false;
    }

    const QWriteLocker locker(&m_lock);

    auto idDate = m_storageLists.dates().findId(record.date());
    auto idDrawing = m_storageLists.drawings().findId(record.drawing());
    auto idAmount = m_storageLists.amounts().findId(record.amount());
    if (!idDate.has_value() || !idDrawing.has_value() || !idAmount.has_value()) {
        return false;
    }

    auto idExecutors = findIdHelper(record.executors(),
                                    m_storageLists.customStorage(CustomStorage::Executors));
    auto idAuthors = findIdHelper(record.authors(),
                                  m_storageLists.customStorage(CustomStorage::Authors));
    auto idCastingMaterials = findIdHelper(record.castingMaterials(),
                                           m_storageLists.customStorage(
                                               CustomStorage::CastingMaterials));
    auto idModelMaterials = findIdHelper(record.modelMaterials(),
                                         m_storageLists.customStorage(
                                             CustomStorage::ModelMaterials));
    auto idMachines = findIdHelper(record.machines(),
                                   m_storageLists.customStorage(CustomStorage::Machines));
    auto idNotes = findIdHelper(record.notes(), m_storageLists.customStorage(CustomStorage::Notes));

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

const QList<mdt::work::Record> &mdt::work::StorageManager::getRecords() const {
    {
        const QReadLocker readLocker(&m_lock);
        if (m_recordsIsValid) {
            return m_records;
        }
    }

    const QWriteLocker writeLocker(&m_lock);
    if (m_recordsIsValid) {
        return m_records;
    }

    m_records.clear();
    const auto recordsLinks = m_storageRecordsLinks.get();
    for (const auto &it : recordsLinks) {
        QString date;
        if (auto v = m_storageLists.dates().findStrValue(it.getIdDate()); v.has_value()) {
            date = v.value();
        } else {
            continue;
        }

        Drawing drawing(Drawing::Null);
        if (auto v = m_storageLists.drawings().findValue(it.getIdDrawing()); v.has_value()) {
            drawing = v.value();
        } else {
            continue;
        }

        qint32 amount {};
        if (auto v = m_storageLists.amounts().findValue(it.getIdAmount()); v.has_value()) {
            amount = v.value();
        } else {
            continue;
        }

        QStringList executors;
        QSet executorsIds = it.getIds(IdsList::Executors);
        for (auto id = executorsIds.begin(); id != executorsIds.end(); ++id) {
            if (auto v = m_storageLists.customStorage(CustomStorage::Executors).findValue(*id);
                v.has_value()) {
                executors << v.value();
            }
        }

        QStringList authors;
        QSet authorsIds = it.getIds(IdsList::Authors);
        for (auto id = authorsIds.begin(); id != authorsIds.end(); ++id) {
            if (auto v = m_storageLists.customStorage(CustomStorage::Authors).findValue(*id);
                v.has_value()) {
                authors << v.value();
            }
        }

        QStringList castingMaterials;
        QSet castingMaterialsIds = it.getIds(IdsList::CastingMaterials);
        for (auto id = castingMaterialsIds.begin(); id != castingMaterialsIds.end(); ++id) {
            if (auto v = m_storageLists.customStorage(CustomStorage::CastingMaterials).findValue(*id);
                v.has_value()) {
                castingMaterials << v.value();
            }
        }

        QStringList modelMaterials;
        QSet modelMaterialsIds = it.getIds(IdsList::ModelMaterials);
        for (auto id = modelMaterialsIds.begin(); id != modelMaterialsIds.end(); ++id) {
            if (auto v = m_storageLists.customStorage(CustomStorage::ModelMaterials).findValue(*id);
                v.has_value()) {
                modelMaterials << v.value();
            }
        }

        QStringList machines;
        QSet machinesIds = it.getIds(IdsList::Machines);
        for (auto id = machinesIds.begin(); id != machinesIds.end(); ++id) {
            if (auto v = m_storageLists.customStorage(CustomStorage::Machines).findValue(*id);
                v.has_value()) {
                machines << v.value();
            }
        }

        QStringList notes;
        QSet notesIds = it.getIds(IdsList::Notes);
        for (auto id = notesIds.begin(); id != notesIds.end(); ++id) {
            if (auto v = m_storageLists.customStorage(CustomStorage::Notes).findValue(*id);
                v.has_value()) {
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

void mdt::work::StorageManager::reset() {
    const QWriteLocker locker(&m_lock);
    m_storageRecordsLinks.reset();
    m_storageLists.reset();
    m_recordsIsValid = false;
}

void mdt::work::StorageManager::commit() {
    const QWriteLocker locker(&m_lock);
    m_storageRecordsLinks.commit();
    m_storageLists.commit();
}

void mdt::work::StorageManager::clear() {
    const QWriteLocker locker(&m_lock);
    m_storageRecordsLinks.clear();
    m_storageLists.clear();
    m_recordsIsValid = false;
}

void mdt::work::StorageManager::serialize(QDataStream &out) const {
    const QReadLocker locker(&m_lock);
    m_storageRecordsLinks.serialize(out);
    m_storageLists.serialize(out);
}

void mdt::work::StorageManager::deserialize(QDataStream &in) {
    const QWriteLocker locker(&m_lock);
    m_storageRecordsLinks.deserialize(in);
    m_storageLists.deserialize(in);
    m_recordsIsValid = false;
}

qsizetype mdt::work::StorageManager::countRecords() const {
    const QReadLocker locker(&m_lock);
    return m_storageRecordsLinks.count();
}

mdt::work::StorageManager::StorageManager() = default;

void mdt::work::StorageManager::deleteBadLinks() {
    const auto links = m_storageRecordsLinks.get();
    for (auto &link : links) {
        if (!m_storageLists.dates().findValue(link.getIdDate()).has_value() ||
            !m_storageLists.drawings().findValue(link.getIdDrawing()).has_value() ||
            !m_storageLists.amounts().findValue(link.getIdAmount()).has_value()) {
            m_storageRecordsLinks.remove(link);
            m_recordsIsValid = false;
        }
    }
}

void mdt::work::StorageManager::deleteBadLinksList(qsizetype customIndex) {
    const auto links = m_storageRecordsLinks.get();
    bool linksChanged = false;
    for (auto link : links) {
        const auto idsOld = link.getIdsCustom(customIndex);

        QSet<qint32> idsNew;
        for (auto id : idsOld) {
            if (auto value = m_storageLists.customStorage(customIndex).findValue(id);
                value.has_value()) {
                idsNew.insert(id);
            }
        }

        const bool changed = idsNew != idsOld;
        if (!changed) {
            continue;
        }

        m_storageRecordsLinks.remove(link);
        link.replaceIdsCustom(customIndex, idsNew);
        m_storageRecordsLinks.add(link);

        linksChanged = true;
    }

    if (linksChanged) {
        m_recordsIsValid = false;
    }
}
