#include "Exceptions.h"

Exception::Exception(const QString &message) noexcept : m_message { message } { }

QString Exception::message() const noexcept {
    return m_message;
}
