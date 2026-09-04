#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "DatesList.h"
#include "DrawingsList.h"

template<typename T>
class CustomRepository {
public:
    CustomRepository(std::unique_ptr<CustomList<T>> list = std::make_unique<CustomList<T>>())
        : m_list(std::move(list)) { }

    virtual ~CustomRepository() = default;

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
    std::unique_ptr<CustomList<T>> m_list;
};

class DatesRepository final : public CustomRepository<qsizetype> {
public:
    DatesRepository() : CustomRepository(std::make_unique<DatesList>()) { }
    virtual ~DatesRepository() = default;

    std::optional<qsizetype> add(const QString& date);
    virtual std::optional<qsizetype> add(const qsizetype& exelFormat) override;
    bool remove(const QString& date);
    virtual bool remove(const qsizetype& exelFormat) override;
    std::optional<qsizetype> findId(const QString& date) const;
    virtual std::optional<qsizetype> findId(const qsizetype& exelFormat) const override;
    std::optional<QString> findStrValue(qsizetype id) const;
    virtual std::optional<qsizetype> findValue(qsizetype id) const override;
    virtual QList<qsizetype> findAllValues() const override;

    virtual qsizetype count() const override;
    virtual void clear() override;

    void setDateFormat(const QString& format);
    QString getDateFormat() const;

    DatesListError::ErrorType lastError() const;
};

class DrawingsRepository final : public CustomRepository<Drawing> {
public:
    DrawingsRepository() : CustomRepository(std::make_unique<DrawingsList>()) { }
    virtual ~DrawingsRepository() = default;

    virtual std::optional<qsizetype> add(const Drawing& drawing) override;
    virtual bool remove(const Drawing& drawing) override;
    virtual std::optional<qsizetype> findId(const Drawing& drawing) const override;
    virtual std::optional<Drawing> findValue(qsizetype id) const override;
    virtual QList<Drawing> findAllValues() const override;

    virtual qsizetype count() const override;
    virtual void clear() override;
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

    DatesRepository& dates();
    const DatesRepository& dates() const;

    DrawingsRepository& drawings();
    const DrawingsRepository& drawings() const;

    CustomRepository<QString>& executors();
    const CustomRepository<QString>& executors() const;

    CustomRepository<QString>& authors();
    const CustomRepository<QString>& authors() const;

    CustomRepository<QString>& castingMaterials();
    const CustomRepository<QString>& castingMaterials() const;

    CustomRepository<QString>& modelMaterials();
    const CustomRepository<QString>& modelMaterials() const;

    CustomRepository<QString>& machines();
    const CustomRepository<QString>& machines() const;

    CustomRepository<QString>& notes();
    const CustomRepository<QString>& notes() const;

    CustomRepository<int>& amounts();
    const CustomRepository<int>& amounts() const;

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
