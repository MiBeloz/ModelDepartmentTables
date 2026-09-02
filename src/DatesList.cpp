#include "DatesList.h"
#include "Constants.h"

DatesListError::ErrorType DatesListError::lastError() const {
    return m_error;
}

void DatesListError::setError(ErrorType error) {
    m_error = error;
}

std::optional<qsizetype> DatesList::insert(const QString &date, const QString &format) {
    const QWriteLocker locker(&m_lock);
    setError(DatesListError::NoError);
    if (auto intDate = strToDate(date, format); intDate.has_value()) {
        for (auto [k, v] : m_dates.asKeyValueRange()) {
            if (v == intDate) {
                return k;
            }
        }

        qsizetype id = -1;
        if (m_emptyIDs.isEmpty()) {
            ++m_id;
            id = m_id;
        } else {
            id = m_emptyIDs.first();
            m_emptyIDs.removeFirst();
        }
        m_dates.insert(id, intDate.value());
        return id;
    }
    setError(DatesListError::FormatError);
    return std::nullopt;
}

bool DatesList::remove(const QString &date, const QString &format) {
    const QWriteLocker locker(&m_lock);
    setError(DatesListError::NoError);
    if (auto intDate = strToDate(date, format); intDate.has_value()) {
        qsizetype id = -1;
        for (auto [k, v] : m_dates.asKeyValueRange()) {
            if (v == intDate) {
                id = k;
                break;
            }
        }
        if (id != -1) {
            m_dates.remove(id);
            m_emptyIDs.append(id);
            return true;
        }
        setError(DatesListError::DateError);
        return false;
    }
    setError(DatesListError::FormatError);
    return false;
}

std::optional<qsizetype> DatesList::getID(const QString &date, const QString &format) const {
    const QReadLocker locker(&m_lock);
    setError(DatesListError::NoError);
    if (auto intDate = strToDate(date, format); intDate.has_value()) {
        for (auto [k, v] : m_dates.asKeyValueRange()) {
            if (v == intDate) {
                return k;
            }
        }
        setError(DatesListError::DateError);
        return std::nullopt;
    }
    setError(DatesListError::FormatError);
    return std::nullopt;
}

std::optional<QString> DatesList::getDate(qsizetype id, const QString &format) const {
    const QReadLocker locker(&m_lock);
    setError(DatesListError::NoError);
    qsizetype date = m_dates.value(id, -1);
    if (date != -1) {
        if (auto result = dateToStr(date, format); result.has_value()) {
            return result;
        }
        setError(DatesListError::FormatError);
        return std::nullopt;
    }
    setError(DatesListError::IdError);
    return std::nullopt;
}

qsizetype DatesList::size() const {
    const QReadLocker locker(&m_lock);
    return m_dates.size();
}

void DatesList::clear() {
    const QWriteLocker locker(&m_lock);
    setError(DatesListError::NoError);
    m_dates.clear();
    m_emptyIDs.clear();
    m_id = 0;
}

DatesListError::ErrorType DatesList::lastError() const {
    return m_error.load(std::memory_order_acquire);
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
