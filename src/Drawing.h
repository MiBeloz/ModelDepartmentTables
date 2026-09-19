#ifndef DRAWING_H
#define DRAWING_H

#include <QDebug>
#include <QHash>

class Drawing final {
public:
    Drawing() = default;
    ~Drawing() = default;

    explicit Drawing(const QString& number, const QString& title)
        : m_number(number)
        , m_title(title) { }

    Drawing(const Drawing& other) {
        m_number = other.m_number;
        m_title = other.m_title;
    }

    Drawing(Drawing&& other) noexcept {
        m_number = std::move(other.m_number);
        m_title = std::move(other.m_title);
    }

    Drawing& operator =(const Drawing& other) {
        if (this == &other) {
            return *this;
        }

        m_number = other.m_number;
        m_title = other.m_title;
        return *this;
    }
    Drawing& operator =(Drawing&& other) noexcept {
        if (this == &other) {
            return *this;
        }

        m_number = std::move(other.m_number);
        m_title = std::move(other.m_title);
        return *this;
    }

    bool operator ==(const Drawing& other) const {
        return m_number == other.m_number && m_title == other.m_title;
    }

    bool operator !=(const Drawing& other) const {
        return !(*this == other);
    }

    bool operator <(const Drawing& other) const {
        if (m_number != other.m_number) {
            return m_number < other.m_number;
        }
        return m_title < other.m_title;
    }

    QString getNumber() const {
        return m_number;
    }

    QString getTitle() const {
        return m_title;
    }

    void setNumber(const QString& number) {
        m_number = number;
    }

    void setTitle(const QString& title) {
        m_title = title;
    }

    friend QDebug& operator <<(QDebug& deb, const Drawing& dr) {
        deb << dr.m_number << " - " << dr.m_title;
        return deb;
    }

private:
    QString m_number;
    QString m_title;

    friend uint qHash(const Drawing& drawing, uint seed);
    friend QDataStream& operator <<(QDataStream& out, const Drawing& drawing);
    friend QDataStream& operator >>(QDataStream& in, Drawing& drawing);
};

inline uint qHash(const Drawing& drawing, uint seed = 0) {
    return qHash(drawing.m_number, seed) ^ qHash(drawing.m_title, seed << 1);
}

inline QDataStream& operator <<(QDataStream& out, const Drawing& drawing) {
    return out << drawing.m_number << drawing.m_title;
}

inline QDataStream& operator >>(QDataStream& in, Drawing& drawing) {
    in >> drawing.m_number >> drawing.m_title;
    return in;
}

#endif // DRAWING_H
