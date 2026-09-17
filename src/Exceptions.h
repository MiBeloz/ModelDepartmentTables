#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <QString>

class Exception {
public:
    explicit Exception(const QString &message = { }) noexcept;
    [[nodiscard]] QString message() const noexcept;

private:
    QString m_message;
};

class RuntimeError : public Exception {
public:
    using Exception::Exception;
};

class InvalidArgument : public Exception {
public:
    using Exception::Exception;
};

#endif // EXCEPTIONS_H
