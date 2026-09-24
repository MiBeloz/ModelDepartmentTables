#include "DatesList.h"

#include <QDate>

std::optional<qint32> DatesList::insert(const QString &date) {
    setError(ErrorType::NoError);
    if (!checkDate(date, m_format)) {
        setError(ErrorType::FormatError);
        return std::nullopt;
    }

    if (auto intDate = strToDate(date, m_format); intDate.has_value()) {
        return CustomList::insert(intDate.value());
    }
    setError(ErrorType::FormatError);
    return std::nullopt;
}

std::optional<qint32> DatesList::insert(const qint32 &exelFormat) {
    setError(ErrorType::NoError);
    if (!checkDate(exelFormat)) {
        setError(ErrorType::FormatError);
        return std::nullopt;
    }
    return CustomList::insert(exelFormat);
}

bool DatesList::remove(const QString &date) {
    setError(ErrorType::NoError);
    if (auto exelDate = strToDate(date, m_format); exelDate.has_value()) {
        if (CustomList::remove(exelDate.value())) {
            return true;
        }
        setError(ErrorType::DateError);
        return false;
    }
    setError(ErrorType::FormatError);
    return false;
}

bool DatesList::remove(const qint32 &exelFormat) {
    setError(ErrorType::NoError);
    if (CustomList::remove(exelFormat)) {
        return true;
    }
    setError(ErrorType::FormatError);
    return false;
}

std::optional<qint32> DatesList::getId(const QString &date) const {
    setError(ErrorType::NoError);
    if (auto exelDate = strToDate(date, m_format); exelDate.has_value()) {
        if (auto id = CustomList::getId(exelDate.value()); id.has_value()) {
            return id;
        }
        setError(ErrorType::DateError);
        return std::nullopt;
    }
    setError(ErrorType::FormatError);
    return std::nullopt;
}

std::optional<qint32> DatesList::getId(const qint32 &exelFormat) const {
    setError(ErrorType::NoError);
    if (auto id = CustomList::getId(exelFormat); id.has_value()) {
        return id;
    }
    setError(ErrorType::DateError);
    return std::nullopt;
}

std::optional<QString> DatesList::getStrValue(qint32 id) const {
    setError(ErrorType::NoError);
    if (auto date = CustomList::getValue(id); date.has_value()) {
        if (auto strDate = dateToStr(date.value(), m_format); strDate.has_value()) {
            return strDate;
        }
        setError(ErrorType::FormatError);
        return std::nullopt;
    }
    setError(ErrorType::IdError);
    return std::nullopt;
}

std::optional<qint32> DatesList::getValue(qint32 id) const {
    setError(ErrorType::NoError);
    if (auto date = CustomList::getValue(id); date.has_value()) {
        return date;
    }
    setError(ErrorType::IdError);
    return std::nullopt;
}

void DatesList::setDateFormat(const QString &format) {
    m_format = format;
}

QString DatesList::getDateFormat() const {
    return m_format;
}

std::optional<QString> DatesList::dateToStr(qint32 date, const QString &format) {
    QDate startDate = QDate::fromJulianDay(START_EXCEL_DATE);
    startDate = startDate.addDays(date);
    if (startDate.isValid()) {
        return startDate.toString(format);
    }
    return std::nullopt;
}

std::optional<qint32> DatesList::strToDate(const QString &date, const QString &format) {
    QDate startDate = QDate::fromJulianDay(START_EXCEL_DATE);
    QDate inputDate = QDate::fromString(date, format);
    if (inputDate.isValid()) {
        return inputDate.toJulianDay() - startDate.toJulianDay();
    }
    return std::nullopt;
}

bool DatesList::checkDate(qint32 date) {
    QDate startDate = QDate::fromJulianDay(START_EXCEL_DATE);
    QDate endDate = QDate::fromJulianDay(END_EXCEL_DATE);
    QDate inputDate = startDate.addDays(date);

    if (inputDate.isValid() && inputDate >= startDate && inputDate <= endDate) {
        return true;
    }
    return false;
}

bool DatesList::checkDate(const QString &date, const QString &format) {
    QDate startDate = QDate::fromJulianDay(START_EXCEL_DATE);
    QDate endDate = QDate::fromJulianDay(END_EXCEL_DATE);
    QDate inputDate = QDate::fromString(date, format);

    if (inputDate.isValid() && inputDate >= startDate && inputDate <= endDate) {
        return true;
    }
    return false;
}

DatesList::ErrorType DatesList::lastError() const {
    return m_error;
}
