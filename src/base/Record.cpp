#include "Record.h"

#include "DatesList.h"

namespace mdt {
    namespace work {
        Record::Record(const QString &date, const Drawing &drawing, qint32 amount)
            : m_date(date)
            , m_drawing(drawing)
            , m_amount(amount) { }

        Record::Record(const QString &date,
                       const Drawing &drawing,
                       qint32 amount,
                       const QStringList &executors,
                       const QStringList &authors,
                       const QStringList &castingMaterials,
                       const QStringList &modelMaterials,
                       const QStringList &machines,
                       const QStringList &notes)
            : m_date(date)
            , m_drawing(drawing)
            , m_amount(amount)
            , m_executors(executors)
            , m_authors(authors)
            , m_castingMaterials(castingMaterials)
            , m_modelMaterials(modelMaterials)
            , m_machines(machines)
            , m_notes(notes) { }

        bool Record::operator ==(const Record &other) const {
            return m_date == other.m_date && m_drawing == other.m_drawing &&
                   m_amount == other.m_amount && m_executors == other.m_executors &&
                   m_authors == other.m_authors && m_castingMaterials == other.m_castingMaterials &&
                   m_modelMaterials == other.m_modelMaterials && m_machines == other.m_machines &&
                   m_notes == other.m_notes;
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
            if (m_executors != other.m_executors) {
                return m_executors < other.m_executors;
            }
            if (m_authors != other.m_authors) {
                return m_authors < other.m_authors;
            }
            if (m_castingMaterials != other.m_castingMaterials) {
                return m_castingMaterials < other.m_castingMaterials;
            }
            if (m_modelMaterials != other.m_modelMaterials) {
                return m_modelMaterials < other.m_modelMaterials;
            }
            if (m_machines != other.m_machines) {
                return m_machines < other.m_machines;
            }
            return m_notes < other.m_notes;
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

        const QStringList &Record::executors() const {
            return m_executors;
        }

        const QStringList &Record::authors() const {
            return m_authors;
        }

        const QStringList &Record::castingMaterials() const {
            return m_castingMaterials;
        }

        const QStringList &Record::modelMaterials() const {
            return m_modelMaterials;
        }

        const QStringList &Record::machines() const {
            return m_machines;
        }

        const QStringList &Record::notes() const {
            return m_notes;
        }
    } // namespace work
} // namespace mdt
