#include "DatesList.h"

#include <QDate>

#include "Constants.h"

DatesListError::ErrorType DatesListError::lastError() const {
    return m_error;
}

void DatesListError::setError(ErrorType error) {
    m_error = error;
}

std::optional<qsizetype> DatesList::insert(const QString &date) {
    setError(DatesListError::NoError);
    if (auto intDate = strToDate(date, m_format); intDate.has_value()) {
        return CustomList::insert(intDate.value());
    }
    setError(DatesListError::FormatError);
    return std::nullopt;
}

std::optional<qsizetype> DatesList::insert(const qsizetype &exelFormat) {
    setError(DatesListError::NoError);
    return CustomList::insert(exelFormat);
}

bool DatesList::remove(const QString &date) {
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

bool DatesList::remove(const qsizetype &exelFormat) {
    setError(DatesListError::NoError);
    if (CustomList::remove(exelFormat)) {
        return true;
    }
    setError(DatesListError::FormatError);
    return false;
}

std::optional<qsizetype> DatesList::getID(const QString &date) const {
    setError(DatesListError::NoError);
    if (auto exelDate = strToDate(date, m_format); exelDate.has_value()) {
        if (auto id = CustomList::getID(exelDate.value()); id.has_value()) {
            return id;
        }
        setError(DatesListError::DateError);
        return std::nullopt;
    }
    setError(DatesListError::FormatError);
    return std::nullopt;
}

std::optional<qsizetype> DatesList::getID(const qsizetype &exelFormat) const {
    setError(DatesListError::NoError);
    if (auto id = CustomList::getID(exelFormat); id.has_value()) {
        return id;
    }
    setError(DatesListError::DateError);
    return std::nullopt;
}

std::optional<QString> DatesList::getStrValue(qsizetype id) const {
    const QReadLocker locker(&this->m_lock);
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

std::optional<qsizetype> DatesList::getValue(qsizetype id) const {
    setError(DatesListError::NoError);
    if (auto date = CustomList::getValue(id); date.has_value()) {
        return date;
    }
    setError(DatesListError::IdError);
    return std::nullopt;
}

DatesListError::ErrorType DatesList::lastError() const {
    return m_error.load(std::memory_order_acquire);
}

void DatesList::setDateFormat(const QString &format) {
    m_format = format;
}

QString DatesList::getDateFormat() const {
    return m_format;
}

std::optional<QString> DatesList::dateToStr(qsizetype date, const QString &format) {
    QDate baseDate = QDate::fromJulianDay(startDateExcel);
    baseDate = baseDate.addDays(date);
    if (baseDate.isValid()) {
        return baseDate.toString(format);
    }
    return std::nullopt;
}

std::optional<qsizetype> DatesList::strToDate(const QString &date, const QString &format) {
    QDate baseDate = QDate::fromJulianDay(startDateExcel);
    QDate inputDate = QDate::fromString(date, format);
    if (inputDate.isValid()) {
        return inputDate.toJulianDay() - baseDate.toJulianDay();
        ;
    }
    return std::nullopt;
}

void DatesList::setError(DatesListError::ErrorType error) const {
    m_error.store(error, std::memory_order_release);
}
