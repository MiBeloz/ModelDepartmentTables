#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <QException>
#include <QString>

class Exception : public QException {
public:
    explicit Exception(const QString &message) noexcept;

    Exception *clone() const override;
    void raise() const override;

    [[nodiscard]] QString message() const noexcept;

private:
    QString m_message;
};

class RuntimeError : public Exception {
public:
    using Exception::Exception;

    RuntimeError *clone() const override;
    void raise() const override;
};

class InvalidArgument : public Exception {
public:
    using Exception::Exception;

    InvalidArgument *clone() const override;
    void raise() const override;
};

#endif // EXCEPTIONS_H
