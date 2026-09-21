#include "Drawing.h"

Drawing::Drawing(const QString &number, const QString &title)
    : m_number(number.trimmed())
    , m_title(title.trimmed()) { }

bool Drawing::operator ==(const Drawing &other) const {
    return m_number == other.m_number && m_title == other.m_title;
}

bool Drawing::operator !=(const Drawing &other) const {
    return !(*this == other);
}

bool Drawing::operator <(const Drawing &other) const {
    if (m_number != other.m_number) {
        return m_number < other.m_number;
    }
    return m_title < other.m_title;
}

const QString &Drawing::getNumber() const {
    return m_number;
}

const QString &Drawing::getTitle() const {
    return m_title;
}

void Drawing::setNumber(const QString &number) {
    m_number = number.trimmed();
}

void Drawing::setTitle(const QString &title) {
    m_title = title.trimmed();
}

bool Drawing::isValid() const {
    return !m_number.isEmpty() && !m_title.isEmpty();
}
