#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "DatesList.h"
#include "Drawing.h"

template<typename T, typename U = CustomList<T>>
class CustomRepository {
public:
    CustomRepository(std::unique_ptr<U> list = std::make_unique<U>())
        : m_list(std::move(list)) { }

    virtual ~CustomRepository() { }

    virtual std::optional<qsizetype> add(const T& value) {
        return m_list->insert(value);
    }

    virtual bool remove(const T& value) {
        return m_list->remove(value);
    }

    virtual std::optional<qsizetype> findId(const T& value) const {
        return m_list->getId(value);
    }

    virtual std::optional<T> findValue(qsizetype id) const {
        return m_list->getValue(id);
    }


    virtual QList<T> findAllValues() const {
        return m_list->getAllValues();
    }

    virtual qsizetype count() const {
        return m_list->size();
    }

    virtual void clear() {
        return m_list->clear();
    }

protected:
    std::unique_ptr<U> m_list;
};

class DatesRepository final : public CustomRepository<qsizetype, DatesList> {
public:
    DatesRepository() : CustomRepository() { }

    virtual ~DatesRepository() = default;

    std::optional<qsizetype> add(const QString& date) {
        return m_list->insert(date);
    }

    bool remove(const QString& date) {
        return m_list->remove(date);
    }

    std::optional<qsizetype> findId(const QString& date) const {
        return m_list->getId(date);
    }

    std::optional<QString> findStrValue(qsizetype id) const {
        return m_list->getStrValue(id);
    }

    DatesListError::ErrorType lastError() const {
        return m_list->lastError();
    }

    void setDateFormat(const QString& format) {
        m_list->setDateFormat(format);
    }

    QString getDateFormat() const {
        return m_list->getDateFormat();
    }
};

class RepositoryService final {
public:
    RepositoryService()
        : m_dateRepo(std::make_unique<DatesRepository>())
        , m_drawingRepo(std::make_unique<CustomRepository<Drawing>>())
        , m_executorRepo(std::make_unique<CustomRepository<QString>>())
        , m_authorRepo(std::make_unique<CustomRepository<QString>>())
        , m_castingMaterialRepo(std::make_unique<CustomRepository<QString>>())
        , m_modelMaterialRepo(std::make_unique<CustomRepository<QString>>())
        , m_machineRepo(std::make_unique<CustomRepository<QString>>())
        , m_noteRepo(std::make_unique<CustomRepository<QString>>())
        , m_amountRepo(std::make_unique<CustomRepository<int>>()) { }

    DatesRepository& dates() {
        return *m_dateRepo;
    }
    const DatesRepository& dates() const {
        return *m_dateRepo;
    }

    CustomRepository<Drawing>& drawings() {
        return *m_drawingRepo;
    }
    const CustomRepository<Drawing>& drawings() const {
        return *m_drawingRepo;
    }

    CustomRepository<QString>& executors() {
        return *m_executorRepo;
    }
    const CustomRepository<QString>& executors() const {
        return *m_executorRepo;
    }

    CustomRepository<QString>& authors() {
        return *m_authorRepo;
    }
    const CustomRepository<QString>& authors() const {
        return *m_authorRepo;
    }

    CustomRepository<QString>& castingMaterials() {
        return *m_castingMaterialRepo;
    }
    const CustomRepository<QString>& castingMaterials() const {
        return *m_castingMaterialRepo;
    }

    CustomRepository<QString>& modelMaterials() {
        return *m_modelMaterialRepo;
    }
    const CustomRepository<QString>& modelMaterials() const {
        return *m_modelMaterialRepo;
    }

    CustomRepository<QString>& machines() {
        return *m_machineRepo;
    }
    const CustomRepository<QString>& machines() const {
        return *m_machineRepo;
    }

    CustomRepository<QString>& notes() {
        return *m_noteRepo;
    }
    const CustomRepository<QString>& notes() const {
        return *m_noteRepo;
    }

    CustomRepository<int>& amounts() {
        return *m_amountRepo;
    }
    const CustomRepository<int>& amounts() const {
        return *m_amountRepo;
    }

private:
    std::unique_ptr<DatesRepository> m_dateRepo;
    std::unique_ptr<CustomRepository<Drawing>> m_drawingRepo;
    std::unique_ptr<CustomRepository<QString>> m_executorRepo;
    std::unique_ptr<CustomRepository<QString>> m_authorRepo;
    std::unique_ptr<CustomRepository<QString>> m_castingMaterialRepo;
    std::unique_ptr<CustomRepository<QString>> m_modelMaterialRepo;
    std::unique_ptr<CustomRepository<QString>> m_machineRepo;
    std::unique_ptr<CustomRepository<QString>> m_noteRepo;
    std::unique_ptr<CustomRepository<int>> m_amountRepo;
};

#endif // REPOSITORY_H
