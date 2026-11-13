#include "DatesList.h"

#include <QDate>

namespace mdt {
    namespace work {
        bool DatesList::operator==(const DatesList &other) const {
            if (this == &other) {
                return true;
            }

            return m_list == other.m_list && m_error == other.m_error;
        }

        bool DatesList::operator!=(const DatesList &other) const {
            return !(*this == other);
        }

        void DatesList::swap(DatesList &other) {
            if (this == &other) {
                return;
            }

            m_list.swap(other.m_list);
            std::swap(m_error, other.m_error);
        }

        std::optional<qint32> DatesList::insert(const QString &date) {
            setError(ErrorType::NoError);
            if (!checkDate(date, m_format)) {
                setError(ErrorType::FormatError);
                return std::nullopt;
            }

            if (std::optional<qint32> intDate = strToDate(date, m_format); intDate.has_value()) {
                return m_list.insert(intDate.value());
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
            return m_list.insert(exelFormat);
        }

        bool DatesList::remove(const QString &date) {
            setError(ErrorType::NoError);
            if (std::optional<qint32> exelDate = strToDate(date, m_format); exelDate.has_value()) {
                if (m_list.remove(exelDate.value())) {
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
            if (m_list.remove(exelFormat)) {
                return true;
            }
            setError(ErrorType::DateError);
            return false;
        }

        std::optional<qint32> DatesList::getId(const QString &date) const {
            setError(ErrorType::NoError);
            if (std::optional<qint32> exelDate = strToDate(date, m_format); exelDate.has_value()) {
                if (std::optional<qint32> id = m_list.getId(exelDate.value()); id.has_value()) {
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
            if (std::optional<qint32> id = m_list.getId(exelFormat); id.has_value()) {
                return id;
            }
            setError(ErrorType::DateError);
            return std::nullopt;
        }

        std::optional<QString> DatesList::getStrValue(qint32 id) const {
            setError(ErrorType::NoError);
            if (std::optional<qint32> date = m_list.getValue(id)->toInt(); date.has_value()) {
                if (std::optional<QString> strDate = dateToStr(date.value(), m_format);
                    strDate.has_value()) {
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
            if (std::optional<qint32> date = m_list.getValue(id)->toInt(); date.has_value()) {
                return date;
            }
            setError(ErrorType::IdError);
            return std::nullopt;
        }

        QList<qint32> DatesList::getAllValues() const {
            QList<qint32> list;
            for (QVariant &v : m_list.getAllValues()) {
                list.append(v.toInt());
            }
            return list;
        }

        QStringList DatesList::getAllStrValues() const {
            QStringList strValues;
            QList<qint32> values = getAllValues();
            for (qint32 &value : values) {
                strValues.append(DatesList::dateToStr(value, m_format).value());
            }
            return strValues;
        }

        void DatesList::clear() {
            m_list.clear();
        }

        void DatesList::reset() {
            m_list.reset();
        }

        void DatesList::commit() {
            m_list.commit();
        }

        qsizetype DatesList::size() const {
            return m_list.size();
        }

        qsizetype DatesList::sizeCommitted() const {
            return m_list.sizeCommitted();
        }

        void DatesList::serialize(QDataStream &out) const {
            m_list.serialize(out);
        }

        void DatesList::deserialize(QDataStream &in) {
            m_list.deserialize(in);
        }

        DatesList::ErrorType DatesList::lastError() const {
            return m_error;
        }

        void DatesList::setDateFormat(const QString &format) {
            m_format = format;
        }

        QString DatesList::getDateFormat() {
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
    } // namespace work
} // namespace mdt
