#ifndef DATESLIST_H
#define DATESLIST_H

#include "CustomList.h"

using namespace Qt::Literals::StringLiterals;

namespace mdt {
    namespace work {
        constexpr const int START_EXCEL_DATE = 2415019; // 30.12.1899
        constexpr const int END_EXCEL_DATE = 5373484;   // 31.12.9999

        class DatesList final {
        public:
            enum ErrorType {
                NoError,
                IdError,
                DateError,
                FormatError,
            };

            DatesList() = default;
            ~DatesList() = default;

            DatesList(const DatesList &) = default;
            DatesList(DatesList &&) noexcept = default;
            DatesList &operator=(const DatesList &) = default;
            DatesList &operator=(DatesList &&) noexcept = default;

            bool operator==(const DatesList &other) const;
            bool operator!=(const DatesList &other) const;

            void swap(DatesList &other);

            std::optional<qint32> insert(const QString &date);
            std::optional<qint32> insert(const qint32 &exelFormat);
            bool remove(const QString &date);
            bool remove(const qint32 &exelFormat);
            std::optional<qint32> getId(const QString &date) const;
            std::optional<qint32> getId(const qint32 &exelFormat) const;
            std::optional<QString> getStrValue(qint32 id) const;
            std::optional<qint32> getValue(qint32 id) const;
            QStringList getAllStrValues() const;
            QList<qint32> getAllValues() const;

            void clear();
            void reset();
            void commit();

            qsizetype size() const;
            qsizetype sizeCommitted() const;

            void serialize(QDataStream &out) const;
            void deserialize(QDataStream &in);

            ErrorType lastError() const;

            static void setDateFormat(const QString &format);
            static QString getDateFormat();

            static std::optional<QString> dateToStr(qint32 date,
                                                    const QString &format = u"dd.MM.yyyy"_s);
            static std::optional<qint32> strToDate(const QString &date,
                                                   const QString &format = u"dd.MM.yyyy"_s);

            static bool checkDate(qint32 date);
            static bool checkDate(const QString &date, const QString &format = u"dd.MM.yyyy"_s);

        private:
            CustomList m_list;
            static inline QString m_format = u"dd.MM.yyyy"_s;
            mutable ErrorType m_error { ErrorType::NoError };

            void setError(ErrorType error) const {
                m_error = error;
            }
        };
    } // namespace work
} // namespace mdt

#endif // DATESLIST_H
