#ifndef STORAGERECORD_H
#define STORAGERECORD_H

// #include "SaverFileStorage.h"
// #include "StorageOfRecordLink.h"

// struct Record {
//     explicit Record(const QString& _date,
//                     const Drawing& _drawing,
//                     qint32 _amount,
//                     const QStringList& _executors = QStringList(),
//                     const QStringList& _authors = QStringList(),
//                     const QStringList& _castingMaterials = QStringList(),
//                     const QStringList& _modelMaterials = QStringList(),
//                     const QStringList& _machines = QStringList(),
//                     const QStringList& _notes = QStringList())
//         : date(_date)
//         , drawing(_drawing)
//         , amount(_amount)
//         , executors(_executors)
//         , authors(_authors)
//         , castingMaterials(_castingMaterials)
//         , modelMaterials(_modelMaterials)
//         , machines(_machines)
//         , notes(_notes) { }

//     QString date;
//     Drawing drawing;
//     qint32 amount = 0;
//     QStringList executors;
//     QStringList authors;
//     QStringList castingMaterials;
//     QStringList modelMaterials;
//     QStringList machines;
//     QStringList notes;
// };

// class StorageOfRecord final {
// public:
//     StorageOfRecord()
//         : m_saverFileStorage(STORAGE_SAVER_FILENAME,
//                              STORAGE_SAVER_FILENAME_TMP,
//                              STORAGE_SAVER_FILENAME_BACKUP) { }

//     ~StorageOfRecord() { }

//     StorageOfRecord(const StorageOfRecord&) = delete;
//     StorageOfRecord& operator =(const StorageOfRecord&) = delete;

//     bool add(const Record& record) {
//         if (!checkDate(record.date) || record.amount < 1) {
//             return false;
//         }

//         qint32 idDate = 0;
//         if (auto id = m_storageService.dates().add(record.date); id.has_value()) {
//             idDate = id.value();
//         } else {
//             return false;
//         }

//         qint32 idDrawing = 0;
//         if (auto id = m_storageService.drawings().add(record.drawing); id.has_value()) {
//             idDrawing = id.value();
//         } else {
//             return false;
//         }

//         qint32 idAmount = 0;
//         if (auto id = m_storageService.amounts().add(record.amount); id.has_value()) {
//             idAmount = id.value();
//         } else {
//             return false;
//         }

//         auto idExecutors = addHelper(record.executors, m_storageService.executors());
//         auto idAuthors = addHelper(record.authors, m_storageService.authors());
//         auto idCastingMaterials = addHelper(record.castingMaterials,
//                                             m_storageService.castingMaterials());
//         auto idModelMaterials = addHelper(record.modelMaterials, m_storageService.modelMaterials());
//         auto idMachines = addHelper(record.machines, m_storageService.machines());
//         auto idNotes = addHelper(record.notes, m_storageService.notes());

//         RecordLink recordLink(idDate,
//                               idDrawing,
//                               idAmount,
//                               idExecutors,
//                               idAuthors,
//                               idCastingMaterials,
//                               idModelMaterials,
//                               idMachines,
//                               idNotes);

//         if (m_saverFileStorage.save(m_storageService)) {
//             if (m_saverFileStorage.commit()) {
//                 m_storageOfRecordLink.add(recordLink);
//                 return true;
//             }
//         }

//         return false;
//     }

//     bool remove(const Record& record) {
//         if (!checkDate(record.date) || record.amount < 1) {
//             return false;
//         }

//         auto idDate = m_storageService.dates().findId(record.date);
//         auto idDrawing = m_storageService.drawings().findId(record.drawing);
//         auto idAmount = m_storageService.amounts().findId(record.amount);
//         if (!idDate.has_value() || !idDrawing.has_value() || !idAmount.has_value()) {
//             return false;
//         }

//         auto idExecutors = findIdHelper(record.executors, m_storageService.executors());
//         auto idAuthors = findIdHelper(record.authors, m_storageService.authors());
//         auto idCastingMaterials = findIdHelper(record.castingMaterials,
//                                                m_storageService.castingMaterials());
//         auto idModelMaterials = findIdHelper(record.modelMaterials,
//                                              m_storageService.modelMaterials());
//         auto idMachines = findIdHelper(record.machines, m_storageService.machines());
//         auto idNotes = findIdHelper(record.notes, m_storageService.notes());

//         RecordLink recordLink(idDate.value(),
//                               idDrawing.value(),
//                               idAmount.value(),
//                               idExecutors,
//                               idAuthors,
//                               idCastingMaterials,
//                               idModelMaterials,
//                               idMachines,
//                               idNotes);

//         m_storageOfRecordLink.remove(recordLink);
//         // TODO
//         // Check other recordsLinks and remove dead links.
//         // Check m_storageService and remove dead values.

//         return true;
//     }

//     QSet<Record> get() const {
//         // TODO
//         // Return all records
//         return QSet<Record>();
//     }

//     qsizetype count() const {
//         return m_storageOfRecordLink.count();
//     }

//     void clear() {
//         m_storageOfRecordLink.clear();
//     }

//     void print() {
//         qDebug() << "LinkRecords:";
//         qint32 i = 1;
//         auto recordsLinks = m_storageOfRecordLink.get();
//         for (auto& it : recordsLinks) {
//             qDebug() << "\tLinkRecord" << i++;
//             qDebug() << "\t\tDate =" << it.getIdDate() << "-"
//                      << m_storageService.dates().findStrValue(it.getIdDate()).value();
//             qDebug() << "\t\tDrawing =" << it.getIdDrawing() << "-"
//                      << m_storageService.drawings().findValue(it.getIdDrawing()).value().getNumber()
//                      << "-"
//                      << m_storageService.drawings().findValue(it.getIdDrawing()).value().getTitle();
//             qDebug() << "\t\tAmount =" << it.getIdAmount() << "-"
//                      << m_storageService.amounts().findValue(it.getIdAmount()).value();

//             QSet executors = it.getIdExecutors();
//             qDebug() << "\t\tExecutors =" << it.getIdExecutors() << "-";
//             for (auto it2 = executors.begin(); it2 != executors.end(); ++it2) {
//                 qDebug() << "\t\t\t" << m_storageService.executors().findValue(*it2).value();
//             }

//             QSet authors = it.getIdAuthors();
//             qDebug() << "\t\tAuthors =" << it.getIdAuthors() << "-";
//             for (auto it2 = authors.begin(); it2 != authors.end(); ++it2) {
//                 qDebug() << "\t\t\t" << m_storageService.authors().findValue(*it2).value();
//             }

//             QSet castingMaterials = it.getIdCastingMaterials();
//             qDebug() << "\t\tCastingMaterials =" << it.getIdCastingMaterials() << "-";
//             for (auto it2 = castingMaterials.begin(); it2 != castingMaterials.end(); ++it2) {
//                 qDebug() << "\t\t\t" << m_storageService.castingMaterials().findValue(*it2).value();
//             }

//             QSet modelMaterials = it.getIdModelMaterials();
//             qDebug() << "\t\tModelMaterials =" << it.getIdModelMaterials() << "-";
//             for (auto it2 = modelMaterials.begin(); it2 != modelMaterials.end(); ++it2) {
//                 qDebug() << "\t\t\t" << m_storageService.modelMaterials().findValue(*it2).value();
//             }

//             QSet machines = it.getIdMachines();
//             qDebug() << "\t\tMachines =" << it.getIdMachines() << "-";
//             for (auto it2 = machines.begin(); it2 != machines.end(); ++it2) {
//                 qDebug() << "\t\t\t" << m_storageService.machines().findValue(*it2).value();
//             }

//             QSet notes = it.getIdNotes();
//             qDebug() << "\t\tNotes =" << it.getIdNotes() << "-";
//             for (auto it2 = notes.begin(); it2 != notes.end(); ++it2) {
//                 qDebug() << "\t\t\t" << m_storageService.notes().findValue(*it2).value();
//             }
//         }
//     }

// private:
//     StorageOfRecordLink m_storageOfRecordLink;
//     StorageService m_storageService;
//     SaverFileStorage m_saverFileStorage;

//     bool checkDate(const QString& date) const {
//         if (auto d = DatesList::strToDate(date); d.has_value()) {
//             return true;
//         }
//         return false;
//     }

//     template<typename T>
//     QSet<qint32> addHelper(const QStringList& values, CustomStorage<T>& storage) {
//         QSet<qint32> result;
//         for (auto it = values.begin(); it != values.end(); ++it) {
//             if (auto id = storage.add(*it); id.has_value()) {
//                 result.insert(id.value());
//             }
//         }
//         return result;
//     }

//     template<typename T>
//     QSet<qint32> findIdHelper(const QStringList& values, const CustomStorage<T>& storage) const {
//         QSet<qint32> result;
//         for (auto it = values.begin(); it != values.end(); ++it) {
//             if (auto id = storage.findId(*it); id.has_value()) {
//                 result.insert(id.value());
//             }
//         }
//         return result;
//     }
// };

#endif // STORAGERECORD_H
