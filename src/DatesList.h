#ifndef DATESLIST_H
#define DATESLIST_H

#include "CustomList.h"

constexpr const int START_EXCEL_DATE = 2415019; // 30.12.1899
constexpr const int END_EXCEL_DATE = 5373484;   // 31.12.9999

class DatesList final : public CustomList<qint32> {
public:
    enum ErrorType {
        NoError,
        IdError,
        DateError,
        FormatError,
    };

    std::optional<qint32> insert(const QString &date);
    std::optional<qint32> insert(const qint32 &exelFormat) override;
    bool remove(const QString &date);
    bool remove(const qint32 &exelFormat) override;
    std::optional<qint32> getId(const QString &date) const;
    std::optional<qint32> getId(const qint32 &exelFormat) const override;
    std::optional<QString> getStrValue(qint32 id) const;
    std::optional<qint32> getValue(qint32 id) const override;

    ErrorType lastError() const;

    void setDateFormat(const QString &format);
    QString getDateFormat() const;

    static std::optional<QString> dateToStr(qint32 date, const QString &format = "dd.MM.yyyy");
    static std::optional<qint32> strToDate(const QString &date,
                                           const QString &format = "dd.MM.yyyy");

    static bool checkDate(qint32 date);
    static bool checkDate(const QString &date, const QString &format = "dd.MM.yyyy");

private:
    QString m_format = "dd.MM.yyyy";
    mutable ErrorType m_error { ErrorType::NoError };

    void setError(ErrorType error) const {
        m_error = error;
    }
};

#endif // DATESLIST_H
