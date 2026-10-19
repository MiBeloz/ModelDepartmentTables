#ifndef RECORD_H
#define RECORD_H

#include <QDate>

#include "Drawing.h"

namespace mdt {
    namespace work {
        class Record {
        public:
            Record() = default;

            explicit Record(const QString& date, const Drawing& drawing, qint32 amount);

            explicit Record(const QString& date,
                            const Drawing& drawing,
                            qint32 amount,
                            const QStringList& executors,
                            const QStringList& authors,
                            const QStringList& castingMaterials,
                            const QStringList& modelMaterials,
                            const QStringList& machines,
                            const QStringList& notes);

            bool operator ==(const Record& other) const;
            bool operator !=(const Record& other) const;
            bool operator <(const Record& other) const;
            bool operator >(const Record& other) const;
            bool operator <=(const Record& other) const;
            bool operator >=(const Record& other) const;

            bool isValid() const;

            const QString& date() const;
            const Drawing& drawing() const;
            qint32 amount() const;
            const QStringList& executors() const;
            const QStringList& authors() const;
            const QStringList& castingMaterials() const;
            const QStringList& modelMaterials() const;
            const QStringList& machines() const;
            const QStringList& notes() const;

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
    } // namespace work
} // namespace mdt

#endif // RECORD_H
