#ifndef DATESLIST_H
#define DATESLIST_H

#include "CustomList.h"

#include <QStringList>

namespace mdt {
    namespace work {
        constexpr int EXCEL_EPOCH_JULIAN_DAY = 2415019;                               // 30.12.1899
        constexpr int EXCEL_END_JULIAN_DAY = 5373484;                                 // 31.12.9999
        constexpr int EXCEL_MAX_DAYS = EXCEL_END_JULIAN_DAY - EXCEL_EPOCH_JULIAN_DAY; // 2958465

        class DatesList final {
        public:
            DatesList() = default;
            ~DatesList() = default;

            DatesList(const DatesList &) = default;
            DatesList(DatesList &&) noexcept = default;
            DatesList &operator=(const DatesList &) = default;
            DatesList &operator=(DatesList &&) noexcept = default;

            bool operator==(const DatesList &other) const;
            bool operator!=(const DatesList &other) const;

            void swap(DatesList &other);

            std::optional<qint32> insert(const QString &strDate,
                                         const QString &format = QStringLiteral(u"dd.MM.yyyy"));
            std::optional<qint32> insert(qint32 exelDate);
            bool remove(const QString &strDate,
                        const QString &format = QStringLiteral(u"dd.MM.yyyy"));
            bool remove(qint32 exelDate);
            std::optional<qint32> getId(const QString &strDate,
                                        const QString &format = QStringLiteral(u"dd.MM.yyyy")) const;
            std::optional<qint32> getId(qint32 exelDate) const;
            std::optional<QString> getStrDate(
                qint32 id,
                const QString &format = QStringLiteral(u"dd.MM.yyyy")) const;
            std::optional<qint32> getExelDate(qint32 id) const;
            QStringList getAllStrDates(const QString &format = QStringLiteral(u"dd.MM.yyyy")) const;
            QList<qint32> getAllExelDates() const;

            void clear();
            void reset();
            void commit();

            qsizetype size() const;
            qsizetype sizeCommitted() const;

            void serialize(QDataStream &out) const;
            void deserialize(QDataStream &in);

            static std::optional<QString> exelDateToStr(
                qint32 exelDate,
                const QString &format = QStringLiteral(u"dd.MM.yyyy"));
            static std::optional<qint32> strToExelDate(
                const QString &strDate,
                const QString &format = QStringLiteral(u"dd.MM.yyyy"));

            static bool checkExelDate(qint32 exelDate);
            static bool checkExelDate(const QString &strDate,
                                      const QString &format = QStringLiteral(u"dd.MM.yyyy"));

        private:
            CustomList m_list;
        };
    } // namespace work
} // namespace mdt

#endif // DATESLIST_H
