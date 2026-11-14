#include "DatesList.h"

#include "Exceptions.h"

#include <QDate>
#include <QObject>

namespace mdt {
    namespace work {
        bool DatesList::operator==(const DatesList &other) const {
            if (this == &other) {
                return true;
            }
            return m_list == other.m_list;
        }

        bool DatesList::operator!=(const DatesList &other) const {
            return !(*this == other);
        }

        void DatesList::swap(DatesList &other) {
            if (this == &other) {
                return;
            }
            m_list.swap(other.m_list);
        }

        std::optional<qint32> DatesList::insert(const QString &date, const QString &format) {
            if (!checkDate(date, format)) {
                return std::nullopt;
            }

            if (std::optional<qint32> intDate = strToDate(date, format); intDate.has_value()) {
                return m_list.insert(intDate.value());
            }
            return std::nullopt;
        }

        std::optional<qint32> DatesList::insert(qint32 exelFormat) {
            if (!checkDate(exelFormat)) {
                return std::nullopt;
            }
            return m_list.insert(exelFormat);
        }

        bool DatesList::remove(const QString &date, const QString &format) {
            if (std::optional<qint32> exelDate = strToDate(date, format); exelDate.has_value()) {
                if (m_list.remove(exelDate.value())) {
                    return true;
                }
                return false;
            }
            return false;
        }

        bool DatesList::remove(qint32 exelFormat) {
            if (!checkDate(exelFormat)) {
                return false;
            }
            return m_list.remove(exelFormat);
        }

        std::optional<qint32> DatesList::getId(const QString &date, const QString &format) const {
            if (std::optional<qint32> exelDate = strToDate(date, format); exelDate.has_value()) {
                if (std::optional<qint32> id = m_list.getId(exelDate.value()); id.has_value()) {
                    return id;
                }
                return std::nullopt;
            }
            return std::nullopt;
        }

        std::optional<qint32> DatesList::getId(qint32 exelFormat) const {
            if (!checkDate(exelFormat)) {
                return std::nullopt;
            }
            return m_list.getId(exelFormat);
        }

        std::optional<QString> DatesList::getStrValue(qint32 id, const QString &format) const {
            if (std::optional<QVariant> date = m_list.getValue(id); date.has_value()) {
                if (std::optional<QString> strDate = dateToStr(date.value().toInt(), format);
                    strDate.has_value()) {
                    return strDate;
                }
                return std::nullopt;
            }
            return std::nullopt;
        }

        std::optional<qint32> DatesList::getValue(qint32 id) const {
            if (std::optional<QVariant> date = m_list.getValue(id); date.has_value()) {
                return date.value().toInt();
            }
            return std::nullopt;
        }

        QStringList DatesList::getAllStrValues(const QString &format) const {
            QStringList strValues;
            QList<qint32> values = getAllValues();
            for (qint32 &value : values) {
                strValues.append(DatesList::dateToStr(value, format).value());
            }
            return strValues;
        }

        QList<qint32> DatesList::getAllValues() const {
            QList<qint32> list;
            for (QVariant &v : m_list.getAllValues()) {
                list.append(v.toInt());
            }
            return list;
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
            for (const QVariant &v : m_list.getAllValues()) {
                if (v.userType() != QMetaType::Int || !checkDate(v.toInt())) {
                    throw RuntimeError(QObject::tr("DatesList: invalid date in stream"));
                }
            }
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
            if (!checkDate(date, format)) {
                return std::nullopt;
            }
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
