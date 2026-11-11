#include "Record.h"

#include "DatesList.h"

namespace mdt {
    namespace work {
        Record::Record(const QString &date, const Drawing &drawing, qint32 amount)
            : m_date(date)
            , m_drawing(drawing)
            , m_amount(amount) {}

        Record::Record(const QString &date,
                       const Drawing &drawing,
                       qint32 amount,
                       const QList<QStringList> &custom)
            : m_date(date)
            , m_drawing(drawing)
            , m_amount(amount)
            , m_custom(custom) {}

        bool Record::operator ==(const Record &other) const {
            return m_date == other.m_date && m_drawing == other.m_drawing &&
                   m_amount == other.m_amount && m_custom == other.m_custom;
        }

        bool Record::operator !=(const Record &other) const {
            return !(*this == other);
        }

        bool Record::operator <(const Record &other) const {
            const QDate l = QDate::fromString(m_date, DatesList::getDateFormat());
            const QDate r = QDate::fromString(other.m_date, DatesList::getDateFormat());

            if (l != r) {
                return l < r;
            }
            if (m_drawing != other.m_drawing) {
                return m_drawing < other.m_drawing;
            }
            if (m_amount != other.m_amount) {
                return m_amount < other.m_amount;
            }
            return m_custom < other.m_custom;
        }

        bool Record::operator >(const Record &other) const {
            return other < *this;
        }

        bool Record::operator <=(const Record &other) const {
            return !(other < *this);
        }

        bool Record::operator >=(const Record &other) const {
            return !(*this < other);
        }

        bool Record::isValid() const {
            if (!DatesList::checkDate(m_date) || !m_drawing.isValid() || m_amount < 1) {
                return false;
            }
            return true;
        }

        const QString &Record::date() const {
            return m_date;
        }

        const Drawing &Record::drawing() const {
            return m_drawing;
        }

        qint32 Record::amount() const {
            return m_amount;
        }

        const QStringList &Record::custom(qsizetype customIndex) const {
            return m_custom[customIndex];
        }
    } // namespace work
} // namespace mdt
