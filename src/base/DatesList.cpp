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

        std::optional<qint32> DatesList::insert(const QString &strDate, const QString &format) {
            if (std::optional<qint32> exelDate = strToExelDate(strDate, format);
                exelDate.has_value()) {
                return m_list.insert(exelDate.value());
            }
            return std::nullopt;
        }

        std::optional<qint32> DatesList::insert(qint32 exelDate) {
            if (!checkExelDate(exelDate)) {
                return std::nullopt;
            }
            return m_list.insert(exelDate);
        }

        bool DatesList::remove(const QString &strDate, const QString &format) {
            if (std::optional<qint32> exelDate = strToExelDate(strDate, format);
                exelDate.has_value()) {
                return m_list.remove(exelDate.value());
            }
            return false;
        }

        bool DatesList::remove(qint32 exelDate) {
            if (!checkExelDate(exelDate)) {
                return false;
            }
            return m_list.remove(exelDate);
        }

        std::optional<qint32> DatesList::getId(const QString &strDate, const QString &format) const {
            if (std::optional<qint32> exelDate = strToExelDate(strDate, format);
                exelDate.has_value()) {
                return m_list.getId(exelDate.value());
            }
            return std::nullopt;
        }

        std::optional<qint32> DatesList::getId(qint32 exelDate) const {
            if (!checkExelDate(exelDate)) {
                return std::nullopt;
            }
            return m_list.getId(exelDate);
        }

        std::optional<QString> DatesList::getStrDate(qint32 id, const QString &format) const {
            std::optional<qint32> exelDate = getExelDate(id);
            if (exelDate.has_value()) {
                return exelDateToStr(exelDate.value(), format);
            }
            return std::nullopt;
        }

        std::optional<qint32> DatesList::getExelDate(qint32 id) const {
            if (std::optional<QVariant> exelDate = m_list.getValue(id); exelDate.has_value()) {
                if (exelDate->userType() != QMetaType::Int) {
                    return std::nullopt;
                }
                return exelDate.value().toInt();
            }
            return std::nullopt;
        }

        QStringList DatesList::getAllStrDates(const QString &format) const {
            QStringList strDates;
            QList<qint32> exelDates = getAllExelDates();
            for (qint32 exelDate : std::as_const(exelDates)) {
                if (auto str = exelDateToStr(exelDate, format); str.has_value()) {
                    strDates.append(str.value());
                }
            }
            return strDates;
        }

        QList<qint32> DatesList::getAllExelDates() const {
            QList<qint32> exelDates;
            QVariantList allValues = m_list.getAllValues();
            for (const QVariant &v : std::as_const(allValues)) {
                if (v.userType() == QMetaType::Int) {
                    exelDates.append(v.toInt());
                }
            }
            return exelDates;
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
            CustomList tmp;
            tmp.deserialize(in);
            QVariantList allValues = tmp.getAllValues();
            for (const QVariant &v : std::as_const(allValues)) {
                if (v.userType() != QMetaType::Int || !checkExelDate(v.toInt())) {
                    throw RuntimeError(QObject::tr("DatesList: invalid date in stream"));
                }
            }
            m_list.swap(tmp);
        }

        std::optional<QString> DatesList::exelDateToStr(qint32 exelDate, const QString &format) {
            if (!checkExelDate(exelDate)) {
                return std::nullopt;
            }
            QDate date = QDate::fromJulianDay(EXCEL_EPOCH_JULIAN_DAY + exelDate);
            if (date.isValid()) {
                return date.toString(format);
            }
            return std::nullopt;
        }

        std::optional<qint32> DatesList::strToExelDate(const QString &strDate,
                                                       const QString &format) {
            QDate inputDate = QDate::fromString(strDate, format);
            if (inputDate.isValid()) {
                qint32 days = inputDate.toJulianDay() - EXCEL_EPOCH_JULIAN_DAY;
                if (days < EXCEL_MIN_DAY || days > EXCEL_MAX_DAY) {
                    return std::nullopt;
                }
                return days;
            }
            return std::nullopt;
        }

        bool DatesList::checkExelDate(qint32 exelDate) {
            return exelDate >= EXCEL_MIN_DAY && exelDate <= EXCEL_MAX_DAY;
        }

        bool DatesList::checkExelDate(const QString &strDate, const QString &format) {
            return strToExelDate(strDate, format).has_value();
        }
    } // namespace work
} // namespace mdt
