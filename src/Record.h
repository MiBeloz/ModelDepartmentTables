#ifndef RECORD_H
#define RECORD_H

#include <QDate>

#include "DatesList.h"
#include "Drawing.h"

struct Record {
    Record() = default;

    explicit Record(const QString& _date,
                    const Drawing& _drawing,
                    qint32 _amount,
                    const QStringList& _executors = QStringList(),
                    const QStringList& _authors = QStringList(),
                    const QStringList& _castingMaterials = QStringList(),
                    const QStringList& _modelMaterials = QStringList(),
                    const QStringList& _machines = QStringList(),
                    const QStringList& _notes = QStringList())
        : date(_date)
        , drawing(_drawing)
        , amount(_amount)
        , executors(_executors)
        , authors(_authors)
        , castingMaterials(_castingMaterials)
        , modelMaterials(_modelMaterials)
        , machines(_machines)
        , notes(_notes) { }

    bool operator ==(const Record& other) const {
        return date == other.date && drawing == other.drawing && amount == other.amount &&
               executors == other.executors && authors == other.authors &&
               castingMaterials == other.castingMaterials &&
               modelMaterials == other.modelMaterials && machines == other.machines &&
               notes == other.notes;
    }

    bool operator !=(const Record& other) const {
        return !(*this == other);
    }

    bool operator <(const Record& other) const {
        QDate leftDate = QDate::fromString(date, DatesList::getDateFormat());
        QDate rightDate = QDate::fromString(other.date, DatesList::getDateFormat());

        return leftDate < rightDate && drawing < other.drawing && amount < other.amount &&
               executors < other.executors && authors < other.authors &&
               castingMaterials < other.castingMaterials && modelMaterials < other.modelMaterials &&
               machines < other.machines && notes < other.notes;
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
        if (!DatesList::checkDate(date) || !drawing.isValid() || amount < 1) {
            return false;
        }
        return true;
    }

    static const Record Null;

    QString date;
    Drawing drawing = Drawing::Null;
    qint32 amount = 0;
    QStringList executors;
    QStringList authors;
    QStringList castingMaterials;
    QStringList modelMaterials;
    QStringList machines;
    QStringList notes;
};

inline const Record Record::Null { };

#endif // RECORD_H
