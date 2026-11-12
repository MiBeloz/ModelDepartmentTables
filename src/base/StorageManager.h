#ifndef STORAGEMANAGER_H
#define STORAGEMANAGER_H

#include <QReadWriteLock>
#include <QSet>
#include <QString>

#include "Record.h"
#include "Storage.h"
#include "StorageLists.h"
#include "StorageRecordsLinks.h"

namespace mdt {
    namespace work {
        class StorageManager final : public Storage {
            Q_DISABLE_COPY_MOVE(StorageManager)

        public:
            class Adder {
            public:
                Adder& date(const QString& date);
                Adder& date(const qint32 exelFormat);
                Adder& drawing(const Drawing& drawing);
                Adder& amount(const qint32 amount);
                Adder& custom(qsizetype customIndex, const QVariant& data);
            };

            class Remover {
            public:
                Remover& date(const QString& date);
                Remover& date(const qint32 exelFormat);
                Remover& drawing(const Drawing& drawing);
                Remover& amount(const qint32 amount);
                Remover& custom(qsizetype customIndex, const QVariant& data);
            };

            class Getter {
            public:
                QVariantList dates() const;
                QStringList datesStr() const;
                DatesList::ErrorType datesError() const;
                QVariantList drawings() const;
                QVariantList amounts() const;
                QVariantList custom(qsizetype customIndex) const;
            };

            class Counter {
            public:
                qsizetype dates() const;
                qsizetype drawings() const;
                qsizetype amounts() const;
                qsizetype custom(qsizetype customIndex) const;
            };

            static void initInstance();
            static void freeInstance();
            static StorageManager* instance();

            Adder add();
            Remover remove();
            Getter get() const;
            Counter count() const;

            void setCustomListsCount(qint32 count);

            bool addRecord(const Record& record);
            bool removeRecord(const Record& record);

            const QList<Record>& getRecords() const;

            void reset();
            void commit();
            void clear();

            void serialize(QDataStream& out) const override;
            void deserialize(QDataStream& in) override;

            qsizetype countRecords() const;

        private:
            StorageManager();
            static StorageManager* m_instance;

            StorageRecordsLinks m_storageRecordsLinks;
            StorageLists m_storageLists;
            mutable QList<Record> m_records;
            mutable bool m_recordsIsValid = true;
            mutable QReadWriteLock m_lock;
            mutable QReadWriteLock m_cacheLock;

            QSet<qint32> addHelper(const QStringList& values, CustomList& storage) {
                QSet<qint32> result;
                for (auto it = values.begin(); it != values.end(); ++it) {
                    if (auto id = storage.insert(*it); id.has_value()) {
                        result.insert(id.value());
                    }
                }
                return result;
            }

            QSet<qint32> findIdHelper(const QStringList& values, const CustomList& storage) const {
                QSet<qint32> result;
                for (auto it = values.begin(); it != values.end(); ++it) {
                    if (auto id = storage.getId(*it); id.has_value()) {
                        result.insert(id.value());
                    }
                }
                return result;
            }

            void deleteBadLinks();

            void deleteBadLinksList(qsizetype customIndex);
        };
    } // namespace work
} // namespace mdt

#endif // STORAGEMANAGER_H
