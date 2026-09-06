#ifndef DATESLIST_H
#define DATESLIST_H

#include <QDate>

#include "Constants.h"
#include "CustomList.h"

class DatesList;
class DatesListError final {
public:
    enum ErrorType {
        NoError,
        IdError,
        DateError,
        FormatError,
    };

    DatesListError(ErrorType error = NoError) : m_error(error) { }

    ErrorType lastError() const;

private:
    ErrorType m_error;

    void setError(ErrorType error) {
        m_error = error;
    }
};

class DatesList final : public CustomList<qsizetype> {
public:
    std::optional<qsizetype> insert(const QString &date) {
        setError(DatesListError::NoError);
        if (auto intDate = strToDate(date, m_format); intDate.has_value()) {
            return CustomList::insert(intDate.value());
        }
        setError(DatesListError::FormatError);
        return std::nullopt;
    }
    std::optional<qsizetype> insert(const qsizetype &exelFormat) override {
        setError(DatesListError::NoError);
        return CustomList::insert(exelFormat);
    }
    bool remove(const QString &date) {
        setError(DatesListError::NoError);
        if (auto exelDate = strToDate(date, m_format); exelDate.has_value()) {
            if (CustomList::remove(exelDate.value())) {
                return true;
            }
            setError(DatesListError::DateError);
            return false;
        }
        setError(DatesListError::FormatError);
        return false;
    }
    bool remove(const qsizetype &exelFormat) override {
        setError(DatesListError::NoError);
        if (CustomList::remove(exelFormat)) {
            return true;
        }
        setError(DatesListError::FormatError);
        return false;
    }
    std::optional<qsizetype> getId(const QString &date) const {
        setError(DatesListError::NoError);
        if (auto exelDate = strToDate(date, m_format); exelDate.has_value()) {
            if (auto id = CustomList::getId(exelDate.value()); id.has_value()) {
                return id;
            }
            setError(DatesListError::DateError);
            return std::nullopt;
        }
        setError(DatesListError::FormatError);
        return std::nullopt;
    }
    std::optional<qsizetype> getId(const qsizetype &exelFormat) const override {
        setError(DatesListError::NoError);
        if (auto id = CustomList::getId(exelFormat); id.has_value()) {
            return id;
        }
        setError(DatesListError::DateError);
        return std::nullopt;
    }
    std::optional<QString> getStrValue(qsizetype id) const {
        setError(DatesListError::NoError);
        if (auto date = CustomList::getValue(id); date.has_value()) {
            if (auto strDate = dateToStr(date.value(), m_format); strDate.has_value()) {
                return strDate;
            }
            setError(DatesListError::FormatError);
            return std::nullopt;
        }
        setError(DatesListError::IdError);
        return std::nullopt;
    }
    std::optional<qsizetype> getValue(qsizetype id) const override {
        setError(DatesListError::NoError);
        if (auto date = CustomList::getValue(id); date.has_value()) {
            return date;
        }
        setError(DatesListError::IdError);
        return std::nullopt;
    }

    DatesListError::ErrorType lastError() const {
        return m_error.load(std::memory_order_acquire);
    }

    void setDateFormat(const QString &format) {
        m_format = format;
    }
    QString getDateFormat() const {
        return m_format;
    }

    static std::optional<QString> dateToStr(qsizetype date, const QString &format = "dd.MM.yyyy") {
        QDate baseDate = QDate::fromJulianDay(startDateExcel);
        baseDate = baseDate.addDays(date);
        if (baseDate.isValid()) {
            return baseDate.toString(format);
        }
        return std::nullopt;
    }
    static std::optional<qsizetype> strToDate(const QString &date,
                                              const QString &format = "dd.MM.yyyy") {
        QDate baseDate = QDate::fromJulianDay(startDateExcel);
        QDate inputDate = QDate::fromString(date, format);
        if (inputDate.isValid()) {
            return inputDate.toJulianDay() - baseDate.toJulianDay();
        }
        return std::nullopt;
    }

private:
    QString m_format = "dd.MM.yyyy";
    mutable std::atomic<DatesListError::ErrorType> m_error { DatesListError::NoError };

    void setError(DatesListError::ErrorType error) const {
        m_error.store(error, std::memory_order_release);
    }
};

#endif // DATESLIST_H
