#ifndef DATESLIST_H
#define DATESLIST_H

#include "CustomList.h"

using namespace Qt::Literals::StringLiterals;

namespace mdt {
    namespace work {
        constexpr const int START_EXCEL_DATE = 2415019; // 30.12.1899
        constexpr const int END_EXCEL_DATE = 5373484;   // 31.12.9999

        class DatesList final : public CustomList {
        public:
            enum ErrorType {
                NoError,
                IdError,
                DateError,
                FormatError,
            };

            std::optional<qint32> insert(const QString &date);
            std::optional<qint32> insert(const QVariant &exelFormat) override;
            bool remove(const QString &date);
            bool remove(const QVariant &exelFormat) override;
            std::optional<qint32> getId(const QString &date) const;
            std::optional<qint32> getId(const QVariant &exelFormat) const override;
            std::optional<QString> getStrValue(qint32 id) const;
            std::optional<QVariant> getValue(qint32 id) const override;
            QStringList getAllStrValues() const;
            QVariantList getAllValues() const override;

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
            static inline QString m_format = u"dd.MM.yyyy"_s;
            mutable ErrorType m_error { ErrorType::NoError };

            void setError(ErrorType error) const {
                m_error = error;
            }
        };
    } // namespace work
} // namespace mdt

#endif // DATESLIST_H
