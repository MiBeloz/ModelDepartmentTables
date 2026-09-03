#ifndef REPOSITORY_H
#define REPOSITORY_H

#include "CustomList.h"
#include "DatesList.h"
#include "DrawingsList.h"

template<typename T>
class IRepository {
public:
    virtual ~IRepository() = default;

    virtual std::optional<qsizetype> add(const T& value) = 0;
    virtual bool remove(const T& value) = 0;
    virtual std::optional<qsizetype> findId(const T& value) const = 0;
    virtual std::optional<T> findValue(qsizetype id) const = 0;
    virtual qsizetype count() const = 0;
    virtual void clear() = 0;
};

class StringRepository final : public IRepository<QString> {
public:
    StringRepository() = default;

    std::optional<qsizetype> add(const QString& value) override;
    bool remove(const QString& value) override;
    std::optional<qsizetype> findId(const QString& value) const override;
    std::optional<QString> findValue(qsizetype id) const override;

    qsizetype count() const override;
    void clear() override;

private:
    CustomList<QString> m_list;
};

class NumericRepository final : public IRepository<int> {
public:
    NumericRepository() = default;

    std::optional<qsizetype> add(const int& value) override;
    bool remove(const int& value) override;
    std::optional<qsizetype> findId(const int& value) const override;
    std::optional<int> findValue(qsizetype id) const override;

    qsizetype count() const override;
    void clear() override;

private:
    CustomList<int> m_list;
};

class DateRepository final : public IRepository<QString> {
public:
    DateRepository() = default;

    std::optional<qsizetype> add(const QString& date) override;
    bool remove(const QString& date) override;
    std::optional<qsizetype> findId(const QString& date) const override;
    std::optional<QString> findValue(qsizetype id) const override;

    qsizetype count() const override;
    void clear() override;

    void setDateFormat(const QString& format);
    QString getDateFormat() const;

    DatesListError::ErrorType lastError() const;

private:
    DatesList m_dates;
};

class DrawingRepository final : public IRepository<Drawing> {
public:
    DrawingRepository() = default;

    std::optional<qsizetype> add(const Drawing& drawing) override;
    std::optional<qsizetype> add(const QString& number, const QString& title);
    bool remove(const Drawing& drawing) override;
    bool remove(const QString& number, const QString& title);
    std::optional<qsizetype> findId(const Drawing& drawing) const override;
    std::optional<Drawing> findValue(qsizetype id) const override;
    QList<Drawing> getAllValues() const;

    qsizetype count() const override;
    void clear() override;

private:
    DrawingsList m_drawings;
};

class RepositoryService final {
public:
    RepositoryService()
        : m_dateRepo(std::make_unique<DateRepository>())
        , m_drawingRepo(std::make_unique<DrawingRepository>())
        , m_executorRepo(std::make_unique<StringRepository>())
        , m_authorRepo(std::make_unique<StringRepository>())
        , m_castingMaterialRepo(std::make_unique<StringRepository>())
        , m_modelMaterialRepo(std::make_unique<StringRepository>())
        , m_machineRepo(std::make_unique<StringRepository>())
        , m_noteRepo(std::make_unique<StringRepository>())
        , m_amountRepo(std::make_unique<NumericRepository>()) { }

    DateRepository& dates();
    const DateRepository& dates() const;

    DrawingRepository& drawings();
    const DrawingRepository& drawings() const;

    StringRepository& executors();
    const StringRepository& executors() const;

    StringRepository& authors();
    const StringRepository& authors() const;

    StringRepository& castingMaterials();
    const StringRepository& castingMaterials() const;

    StringRepository& modelMaterials();
    const StringRepository& modelMaterials() const;

    StringRepository& machines();
    const StringRepository& machines() const;

    StringRepository& notes();
    const StringRepository& notes() const;

    NumericRepository& amounts();
    const NumericRepository& amounts() const;

private:
    std::unique_ptr<DateRepository> m_dateRepo;
    std::unique_ptr<DrawingRepository> m_drawingRepo;
    std::unique_ptr<StringRepository> m_executorRepo;
    std::unique_ptr<StringRepository> m_authorRepo;
    std::unique_ptr<StringRepository> m_castingMaterialRepo;
    std::unique_ptr<StringRepository> m_modelMaterialRepo;
    std::unique_ptr<StringRepository> m_machineRepo;
    std::unique_ptr<StringRepository> m_noteRepo;
    std::unique_ptr<NumericRepository> m_amountRepo;
};

#endif // REPOSITORY_H
