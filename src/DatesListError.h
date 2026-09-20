#ifndef DATESLISTERROR_H
#define DATESLISTERROR_H

class DatesListError final {
public:
    enum ErrorType {
        NoError,
        IdError,
        DateError,
        FormatError,
    };

    DatesListError(ErrorType error = ErrorType::NoError);

    ErrorType lastError() const;

private:
    ErrorType m_error;
};

#endif // DATESLISTERROR_H
