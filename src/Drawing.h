#ifndef DRAWING_H
#define DRAWING_H

#include <QHash>

class Drawing final {
public:
    explicit Drawing(const QString& number, const QString& title)
        : m_number(number)
        , m_title(title) { }

    QString getNumber() const {
        return m_number;
    }

    QString getTitle() const {
        return m_title;
    }

    bool operator ==(const Drawing& other) const {
        return m_number == other.m_number && m_title == other.m_title;
    }

    bool operator <(const Drawing& other) const {
        if (m_number != other.m_number) {
            return m_number < other.m_number;
        }
        return m_title < other.m_title;
    }

private:
    QString m_number;
    QString m_title;
};

inline uint qHash(const Drawing& drawing, uint seed = 0) {
    return qHash(drawing.getNumber(), seed) ^ qHash(drawing.getTitle(), seed << 1);
}

#endif // DRAWING_H
