#ifndef DRAWING_H
#define DRAWING_H

#include <QHash>

class Drawing final {
public:
    Drawing() = default;

    explicit Drawing(const QString& number, const QString& title);

    bool operator ==(const Drawing& other) const;
    bool operator !=(const Drawing& other) const;
    bool operator <(const Drawing& other) const;

    const QString& getNumber() const;
    const QString& getTitle() const;

    void setNumber(const QString& number);
    void setTitle(const QString& title);

    bool isValid() const;

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

#endif // DRAWING_H
