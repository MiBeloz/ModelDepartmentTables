#ifndef RECORD_H
#define RECORD_H

#include "Drawing.h"

namespace mdt {
    namespace work {
        class Record {
        public:
            Record() = default;
            ~Record() = default;

            explicit Record(qint32 exelDate,
                            const Drawing& drawing,
                            qint32 amount,
                            qsizetype customCount);

            explicit Record(qint32 exelDate,
                            const Drawing& drawing,
                            qint32 amount,
                            const QList<QStringList>& custom = {});

            bool operator ==(const Record& other) const;
            bool operator !=(const Record& other) const;
            bool operator <(const Record& other) const;
            bool operator >(const Record& other) const;
            bool operator <=(const Record& other) const;
            bool operator >=(const Record& other) const;

            bool isValid() const;

            qint32 date() const;
            const Drawing& drawing() const;
            qint32 amount() const;
            const QStringList& custom(qsizetype customIndex) const;

            qsizetype customCount() const;

            static const Record Null;

        private:
            qint32 m_date;
            Drawing m_drawing = Drawing::Null;
            qint32 m_amount = 0;
            QList<QStringList> m_custom;
        };

        inline const Record Record::Null {};
    } // namespace work
} // namespace mdt

#endif // RECORD_H
