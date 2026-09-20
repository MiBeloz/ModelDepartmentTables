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

class DatesList final : public CustomList<qint32> {
public:
    std::optional<qint32> insert(const QString &date) {
        setError(DatesListError::NoError);
        if (!checkDate(date, m_format)) {
            setError(DatesListError::FormatError);
            return std::nullopt;
        }

        if (auto intDate = strToDate(date, m_format); intDate.has_value()) {
            return CustomList::insert(intDate.value());
        }
        setError(DatesListError::FormatError);
        return std::nullopt;
    }

    std::optional<qint32> insert(const qint32 &exelFormat) override {
        setError(DatesListError::NoError);
        if (!checkDate(exelFormat)) {
            setError(DatesListError::FormatError);
            return std::nullopt;
        }
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

    bool remove(const qint32 &exelFormat) override {
        setError(DatesListError::NoError);
        if (CustomList::remove(exelFormat)) {
            return true;
        }
        setError(DatesListError::FormatError);
        return false;
    }

    std::optional<qint32> getId(const QString &date) const {
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

    std::optional<qint32> getId(const qint32 &exelFormat) const override {
        setError(DatesListError::NoError);
        if (auto id = CustomList::getId(exelFormat); id.has_value()) {
            return id;
        }
        setError(DatesListError::DateError);
        return std::nullopt;
    }

    std::optional<QString> getStrValue(qint32 id) const {
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

    std::optional<qint32> getValue(qint32 id) const override {
        setError(DatesListError::NoError);
        if (auto date = CustomList::getValue(id); date.has_value()) {
            return date;
        }
        setError(DatesListError::IdError);
        return std::nullopt;
    }

    DatesListError::ErrorType lastError() const {
        return m_error;
    }

    void setDateFormat(const QString &format) {
        m_format = format;
    }

    QString getDateFormat() const {
        return m_format;
    }

    static std::optional<QString> dateToStr(qint32 date, const QString &format = "dd.MM.yyyy") {
        QDate startDate = QDate::fromJulianDay(START_EXCEL_DATE);
        startDate = startDate.addDays(date);
        if (startDate.isValid()) {
            return startDate.toString(format);
        }
        return std::nullopt;
    }

    static std::optional<qint32> strToDate(const QString &date,
                                           const QString &format = "dd.MM.yyyy") {
        QDate startDate = QDate::fromJulianDay(START_EXCEL_DATE);
        QDate inputDate = QDate::fromString(date, format);
        if (inputDate.isValid()) {
            return inputDate.toJulianDay() - startDate.toJulianDay();
        }
        return std::nullopt;
    }

    static bool checkDate(qint32 date) {
        QDate startDate = QDate::fromJulianDay(START_EXCEL_DATE);
        QDate endDate = QDate::fromJulianDay(END_EXCEL_DATE);
        QDate inputDate = startDate.addDays(date);

        if (inputDate.isValid() && inputDate >= startDate && inputDate <= endDate) {
            return true;
        }
        return false;
    }

    static bool checkDate(const QString &date, const QString &format = "dd.MM.yyyy") {
        QDate startDate = QDate::fromJulianDay(START_EXCEL_DATE);
        QDate endDate = QDate::fromJulianDay(END_EXCEL_DATE);
        QDate inputDate = QDate::fromString(date, format);

        if (inputDate.isValid() && inputDate >= startDate && inputDate <= endDate) {
            return true;
        }
        return false;
    }

private:
    QString m_format = "dd.MM.yyyy";
    mutable DatesListError::ErrorType m_error { DatesListError::NoError };

    void setError(DatesListError::ErrorType error) const {
        m_error = error;
    }
};

#endif // DATESLIST_H
