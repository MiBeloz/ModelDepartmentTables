#ifndef DRAWING_H
#define DRAWING_H

#include <QDebug>
#include <QHash>

class Drawing final {
public:
    Drawing() = default;

    explicit Drawing(const QString& number, const QString& title)
        : m_number(number.trimmed())
        , m_title(title.trimmed()) { }

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

    const QString& getNumber() const {
        return m_number;
    }

    const QString& getTitle() const {
        return m_title;
    }

    void setNumber(const QString& number) {
        m_number = number.trimmed();
    }

    void setTitle(const QString& title) {
        m_title = title.trimmed();
    }

    bool isValid() const {
        return !m_number.isEmpty() && !m_title.isEmpty();
    }

    static const Drawing Null;

private:
    QString m_number;
    QString m_title;

    friend QDataStream& operator <<(QDataStream& out, const Drawing& drawing);
    friend QDataStream& operator >>(QDataStream& in, Drawing& drawing);
};

inline const Drawing Drawing::Null { };

inline size_t qHash(const Drawing& drawing, size_t seed = 0) {
    return qHashMulti(seed, drawing.getNumber(), drawing.getTitle());
}

inline QDataStream& operator <<(QDataStream& out, const Drawing& drawing) {
    return out << drawing.m_number << drawing.m_title;
}

inline QDataStream& operator >>(QDataStream& in, Drawing& drawing) {
    in >> drawing.m_number >> drawing.m_title;
    return in;
}

inline QDebug operator <<(QDebug dbg, const Drawing& d) {
    QDebugStateSaver saver(dbg);
    dbg.nospace() << d.getNumber() << " - " << d.getTitle();
    return dbg;
}

#endif // DRAWING_H
