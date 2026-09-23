#include "Exceptions.h"

Exception::Exception(const QString &message) noexcept : m_message { message } { }

Exception *Exception::clone() const {
    return new Exception(*this);
}

void Exception::raise() const {
    throw *this;
}

QString Exception::message() const noexcept {
    return m_message;
}

RuntimeError *RuntimeError::clone() const {
    return new RuntimeError(*this);
}

void RuntimeError::raise() const {
    throw *this;
}

InvalidArgument *InvalidArgument::clone() const {
    return new InvalidArgument(*this);
}

void InvalidArgument::raise() const {
    throw *this;
}
