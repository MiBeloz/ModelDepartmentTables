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
                explicit Adder(StorageManager& storageManager);

                Adder& date(const QString& date);
                Adder& date(const qint32 exelFormat);
                Adder& drawing(const Drawing& drawing);
                Adder& amount(const qint32 amount);
                Adder& executor(const QString& executor);
                Adder& author(const QString& author);
                Adder& castingMaterial(const QString& castingMaterial);
                Adder& modelMaterial(const QString& modelMaterial);
                Adder& machine(const QString& machine);
                Adder& note(const QString& note);

            private:
                StorageManager& m_storageManager;
            };

            class Remover {
            public:
                explicit Remover(StorageManager& storageManager);

                Remover& date(const QString& date);
                Remover& date(const qint32 exelFormat);
                Remover& drawing(const Drawing& drawing);
                Remover& amount(const qint32 amount);
                Remover& executor(const QString& executor);
                Remover& author(const QString& author);
                Remover& castingMaterial(const QString& castingMaterial);
                Remover& modelMaterial(const QString& modelMaterial);
                Remover& machine(const QString& machine);
                Remover& note(const QString& note);

            private:
                StorageManager& m_storageManager;
            };

            class Getter {
            public:
                explicit Getter(const StorageManager& storageManager);

                auto dates() const;
                auto datesStr() const;
                auto datesError() const;
                auto drawings() const;
                auto amounts() const;
                auto executors() const;
                auto authors() const;
                auto castingMaterials() const;
                auto modelMaterials() const;
                auto machines() const;
                auto notes() const;

            private:
                const StorageManager& m_storageManager;
            };

            class Counter {
            public:
                explicit Counter(const StorageManager& storageManager);

                auto dates() const;
                auto drawings() const;
                auto amounts() const;
                auto executors() const;
                auto authors() const;
                auto castingMaterials() const;
                auto modelMaterials() const;
                auto machines() const;
                auto notes() const;

            private:
                const StorageManager& m_storageManager;
            };

            static void initInstance();
            static void freeInstance();
            static StorageManager* instance();

            Adder add();
            Remover remove();
            Getter get() const;
            Counter count() const;

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
            QSet<qint32> findIdHelper(const QStringList& values,
                                      const ServiceCustomList<T>& storage) const {
                QSet<qint32> result;
                for (auto it = values.begin(); it != values.end(); ++it) {
                    if (auto id = storage.findId(*it); id.has_value()) {
                        result.insert(id.value());
                    }
                }
                return result;
            }

            void deleteBadLinks();

            void deleteBadLinksList(IdsList idsList, CustomStorage storage);
        };
    } // namespace work
} // namespace mdt

#endif // STORAGEMANAGER_H
