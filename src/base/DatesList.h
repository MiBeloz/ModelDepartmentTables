#ifndef DATESLIST_H
#define DATESLIST_H

#include "CustomList.h"

namespace mdt {
    namespace work {
        constexpr const int START_EXCEL_DATE = 2415019; // 30.12.1899
        constexpr const int END_EXCEL_DATE = 5373484;   // 31.12.9999

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

            std::optional<qint32> insert(const QString &date,
                                         const QString &format = QStringLiteral(u"dd.MM.yyyy"));
            std::optional<qint32> insert(qint32 exelFormat);
            bool remove(const QString &date, const QString &format = QStringLiteral(u"dd.MM.yyyy"));
            bool remove(qint32 exelFormat);
            std::optional<qint32> getId(const QString &date,
                                        const QString &format = QStringLiteral(u"dd.MM.yyyy")) const;
            std::optional<qint32> getId(qint32 exelFormat) const;
            std::optional<QString> getStrValue(
                qint32 id,
                const QString &format = QStringLiteral(u"dd.MM.yyyy")) const;
            std::optional<qint32> getValue(qint32 id) const;
            QStringList getAllStrValues(const QString &format = QStringLiteral(u"dd.MM.yyyy")) const;
            QList<qint32> getAllValues() const;

            void clear();
            void reset();
            void commit();

            qsizetype size() const;
            qsizetype sizeCommitted() const;

            void serialize(QDataStream &out) const;
            void deserialize(QDataStream &in);

            static std::optional<QString> dateToStr(
                qint32 date,
                const QString &format = QStringLiteral(u"dd.MM.yyyy"));
            static std::optional<qint32> strToDate(
                const QString &date,
                const QString &format = QStringLiteral(u"dd.MM.yyyy"));

            static bool checkDate(qint32 date);
            static bool checkDate(const QString &date,
                                  const QString &format = QStringLiteral(u"dd.MM.yyyy"));

        private:
            CustomList m_list;
        };
    } // namespace work
} // namespace mdt

#endif // DATESLIST_H
