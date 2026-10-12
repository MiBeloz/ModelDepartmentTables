#ifndef RECORD_H
#define RECORD_H

#include <QDate>

#include "DatesList.h"
#include "Drawing.h"

class Record {
public:
    Record() = default;

    explicit Record(const QString& date, const Drawing& drawing, qint32 amount)
        : m_date(date)
        , m_drawing(drawing)
        , m_amount(amount) { }

    explicit Record(const QString& date,
                    const Drawing& drawing,
                    qint32 amount,
                    const QStringList& executors,
                    const QStringList& authors,
                    const QStringList& castingMaterials,
                    const QStringList& modelMaterials,
                    const QStringList& machines,
                    const QStringList& notes)
        : m_date(date)
        , m_drawing(drawing)
        , m_amount(amount)
        , m_executors(executors)
        , m_authors(authors)
        , m_castingMaterials(castingMaterials)
        , m_modelMaterials(modelMaterials)
        , m_machines(machines)
        , m_notes(notes) { }

    bool operator ==(const Record& other) const {
        return m_date == other.m_date && m_drawing == other.m_drawing &&
               m_amount == other.m_amount && m_executors == other.m_executors &&
               m_authors == other.m_authors && m_castingMaterials == other.m_castingMaterials &&
               m_modelMaterials == other.m_modelMaterials && m_machines == other.m_machines &&
               m_notes == other.m_notes;
    }

    bool operator !=(const Record& other) const {
        return !(*this == other);
    }

    bool operator <(const Record& other) const {
        QDate leftDate = QDate::fromString(m_date, DatesList::getDateFormat());
        QDate rightDate = QDate::fromString(other.m_date, DatesList::getDateFormat());

        return leftDate < rightDate && m_drawing < other.m_drawing && m_amount < other.m_amount &&
               m_executors < other.m_executors && m_authors < other.m_authors &&
               m_castingMaterials < other.m_castingMaterials &&
               m_modelMaterials < other.m_modelMaterials && m_machines < other.m_machines &&
               m_notes < other.m_notes;
    }

    bool operator >(const Record& other) const {
        return other < *this;
    }

    bool operator <=(const Record& other) const {
        return !(other < *this);
    }

    bool operator >=(const Record& other) const {
        return !(*this < other);
    }

    bool isValid() const {
        if (!DatesList::checkDate(m_date) || !m_drawing.isValid() || m_amount < 1) {
            return false;
        }
        return true;
    }

    const QString& date() const {
        return m_date;
    }
    const Drawing& drawing() const {
        return m_drawing;
    }
    qint32 amount() const {
        return m_amount;
    }
    const QStringList& executors() const {
        return m_executors;
    }
    const QStringList& authors() const {
        return m_authors;
    }
    const QStringList& castingMaterials() const {
        return m_castingMaterials;
    }
    const QStringList& modelMaterials() const {
        return m_modelMaterials;
    }
    const QStringList& machines() const {
        return m_machines;
    }
    const QStringList& notes() const {
        return m_notes;
    }

    static const Record Null;

private:
    QString m_date;
    Drawing m_drawing = Drawing::Null;
    qint32 m_amount = 0;
    QStringList m_executors;
    QStringList m_authors;
    QStringList m_castingMaterials;
    QStringList m_modelMaterials;
    QStringList m_machines;
    QStringList m_notes;
};

inline const Record Record::Null { };

#endif // RECORD_H
