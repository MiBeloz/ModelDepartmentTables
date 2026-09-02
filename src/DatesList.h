#ifndef DATESLIST_H
#define DATESLIST_H

#include <QDate>
#include <QHash>
#include <QQueue>
#include <QReadWriteLock>

class DatesList;
class DatesListError final {
public:
    enum ErrorType {
        NoError,
        IdError,
        DateError,
        FormatError,
    };

    DatesListError(ErrorType error = NoError)
        : m_error(error) {}

    ErrorType lastError() const;

private:
    ErrorType m_error;
    void setError(ErrorType error);
    friend DatesList;
};

class DatesList final {
public:
    DatesList() {}

    std::optional<qsizetype> insert(const QString &date, const QString &format = "dd.MM.yyyy");
    bool remove(const QString &date, const QString &format = "dd.MM.yyyy");
    std::optional<qsizetype> getID(const QString &date, const QString &format = "dd.MM.yyyy") const;
    std::optional<QString> getDate(qsizetype id, const QString &format = "dd.MM.yyyy") const;

    qsizetype size() const;
    void clear();

    DatesListError::ErrorType lastError() const;

    static std::optional<QString> dateToStr(qsizetype date, const QString &format = "dd.MM.yyyy");
    static std::optional<qsizetype> strToDate(const QString &date, const QString &format = "dd.MM.yyyy");

private:
    QHash<qsizetype, qsizetype> m_dates;
    qsizetype m_id = 0;
    QQueue<qsizetype> m_emptyIDs;
    mutable QReadWriteLock m_lock;
    mutable std::atomic<DatesListError::ErrorType> m_error{DatesListError::NoError};

    void setError(DatesListError::ErrorType error) const;
};

#endif // DATESLIST_H
