#ifndef DATESLIST_H
#define DATESLIST_H

#include "IList.h"

class DatesList;
class DatesListError final {
public:
    enum ErrorType {
        NoError,
        IdError,
        DateError,
        FormatError,
    };

    DatesListError(ErrorType error = NoError) : m_error(error) { }

    ErrorType lastError() const;

private:
    QString m_format = "dd.MM.yyyy";
    ErrorType m_error;

    void setError(ErrorType error);
};

class DatesList final : public IList<qsizetype> {
public:
    DatesList() { }

    std::optional<qsizetype> insert(const QString &date);
    std::optional<qsizetype> insert(const qsizetype &exelFormat) override;
    bool remove(const QString &date);
    bool remove(const qsizetype &exelFormat) override;
    std::optional<qsizetype> getID(const QString &date) const;
    std::optional<qsizetype> getID(const qsizetype &exelFormat) const override;
    std::optional<QString> getStrValue(qsizetype id) const;
    std::optional<qsizetype> getValue(qsizetype id) const override;

    DatesListError::ErrorType lastError() const;

    void setDateFormat(const QString &format);
    QString getDateFormat() const;

    static std::optional<QString> dateToStr(qsizetype date, const QString &format = "dd.MM.yyyy");
    static std::optional<qsizetype> strToDate(const QString &date,
                                              const QString &format = "dd.MM.yyyy");

private:
    QString m_format;
    mutable std::atomic<DatesListError::ErrorType> m_error { DatesListError::NoError };

    void setError(DatesListError::ErrorType error) const;
};

#endif // DATESLIST_H
