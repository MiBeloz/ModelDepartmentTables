#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "DatesList.h"
#include "DrawingsList.h"

template<typename T>
class CustomRepository {
public:
    CustomRepository(CustomList<T>* list = nullptr) : m_list(dynamic_cast<decltype(list)>(list)) {
        if (!list) {
            list = new CustomList<T>();
        }
    }

    virtual ~CustomRepository() {test
        delete m_list;
    }

    virtual std::optional<qsizetype> add(const T& value) {
        return m_list->insert(value);
    }

    virtual bool remove(const T& value) {
        return m_list->remove(value);
    }

    virtual std::optional<qsizetype> findId(const T& value) const {
        return m_list->getID(value);
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
    CustomList<T>* m_list;
};

class DatesRepository final : public CustomRepository<qsizetype> {
public:
    DatesRepository() {
        m_list = new DatesList();
    }
    virtual ~DatesRepository() = default;

    std::optional<qsizetype> add(const QString& date) {
        DatesList *datesListPtr = dynamic_cast<DatesList *>(m_list);
        if (datesListPtr) {
            return datesListPtr->insert(date);
        }
        return std::nullopt;
    }
    virtual std::optional<qsizetype> add(const qsizetype& exelFormat) override;
    bool remove(const QString& date) {
        return this->remove(date);
    }
    virtual bool remove(const qsizetype& exelFormat) override;
    std::optional<qsizetype> findId(const QString& date) const {
        return this->findId(date);
    }
    virtual std::optional<qsizetype> findId(const qsizetype& exelFormat) const override;
    std::optional<QString> findStrValue(qsizetype id) const {
        return this->findStrValue(id);
    }
    virtual std::optional<qsizetype> findValue(qsizetype id) const override {
        return m_list->getValue(id);
    }
    virtual QList<qsizetype> findAllValues() const override;

    virtual qsizetype count() const override;
    virtual void clear() override;

    void setDateFormat(const QString& format) {
        this->setDateFormat(format);
    }
    QString getDateFormat() const {
        return this->getDateFormat();
    }

    DatesListError::ErrorType lastError() const {
        return this->lastError();
    }
};

class DrawingsRepository final : public CustomRepository<Drawing> {
public:
    DrawingsRepository() {
        m_list = new DrawingsList();

    }
    virtual ~DrawingsRepository() = default;

    // virtual std::optional<qsizetype> add(const Drawing& drawing) override;// {
    //     // DrawingsList *drawingsListPtr = dynamic_cast<DrawingsList *>(m_list.get());
    //     // if (drawingsListPtr) {
    //     //     return drawingsListPtr->insert(drawing);
    //     // }
    //     // return std::nullopt;
    // //}
    // virtual bool remove(const Drawing& drawing) override;
    // virtual std::optional<qsizetype> findId(const Drawing& drawing) const override;
    // virtual std::optional<Drawing> findValue(qsizetype id) const override {
    //     DrawingsList *drawingsListPtr = dynamic_cast<DrawingsList *>(m_list.get());
    //     if (drawingsListPtr) {
    //         return drawingsListPtr->getValue(id);
    //     }
    //     return std::nullopt;
    // }
    // virtual QList<Drawing> findAllValues() const override;

    // virtual qsizetype count() const override;
    // virtual void clear() override;

    virtual std::optional<Drawing> findValue(qsizetype id) const override {
        return m_list->getValue(id);
    }
};

class RepositoryService final {
public:
    RepositoryService()
        : m_dateRepo(std::make_unique<DatesRepository>())
        , m_drawingRepo(std::make_unique<DrawingsRepository>())
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

    DrawingsRepository& drawings() {
        return *m_drawingRepo;
    }
    const DrawingsRepository& drawings() const {
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
    std::unique_ptr<DrawingsRepository> m_drawingRepo;
    std::unique_ptr<CustomRepository<QString>> m_executorRepo;
    std::unique_ptr<CustomRepository<QString>> m_authorRepo;
    std::unique_ptr<CustomRepository<QString>> m_castingMaterialRepo;
    std::unique_ptr<CustomRepository<QString>> m_modelMaterialRepo;
    std::unique_ptr<CustomRepository<QString>> m_machineRepo;
    std::unique_ptr<CustomRepository<QString>> m_noteRepo;
    std::unique_ptr<CustomRepository<int>> m_amountRepo;
};

#endif // REPOSITORY_H
