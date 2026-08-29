#ifndef ITEMDATA_H
#define ITEMDATA_H

#include <QHash>
#include <QQueue>
#include <QDate>
#include <QReadWriteLock>
#include <memory>
#include <QDebug>

#include "Constants.h"

class DatesList final {
public:
    DatesList() {}

    qsizetype insert(const QString& date, QString format = "dd.MM.yyyy") {
        const QWriteLocker locker(&m_lock);
        qsizetype d = strToDate(date, format);
        if (d < 0) {
            return d;
        }

        for (auto [k, v] : m_dates.asKeyValueRange()) {
            if (v == d) {
                return k;
            }
        }

        qsizetype id;
        if (m_emptyIDs.isEmpty()) {
            ++m_id;
            id = m_id;
        }
        else {
            id = m_emptyIDs.first();
            m_emptyIDs.removeFirst();
        }
        m_dates.insert(id, d);
        return id;
    }

    bool remove(const QString& date, QString format = "dd.MM.yyyy") {
        const QWriteLocker locker(&m_lock);
        qsizetype d = strToDate(date, format);
        if (d < 0) {
            return false;
        }

        qsizetype id = -1;
        for (auto [k, v] : m_dates.asKeyValueRange()) {
            if (v == d) {
                id = k;
                break;
            }
        }
        if (id != -1) {
            m_dates.remove(id);
            m_emptyIDs.append(id);
            return true;
        }
        return false;
    }

    qsizetype getID(const QString& date, QString format = "dd.MM.yyyy") const {
        const QReadLocker locker(&m_lock);
        qsizetype d = strToDate(date, format);
        if (d < 0) {
            return d;
        }

        for (auto [k, v] : m_dates.asKeyValueRange()) {
            if (v == d) {
                return k;
            }
        }
        return 0;
    }

    QString getDate(qsizetype id, QString format = "dd.MM.yyyy") const {
        const QReadLocker locker(&m_lock);
        qsizetype d = m_dates.value(id, 0);

        if (d == 0) {
            return QString();
        }
        else {
            return dateToStr(d, format);
        }
    }

    qsizetype size() const {
        const QReadLocker locker(&m_lock);
        return m_dates.size();
    }

private:
    QString dateToStr(qsizetype date, QString format = "dd.MM.yyyy") const {
        QDate baseDate = QDate::fromJulianDay(startDateExcel);
        baseDate = baseDate.addDays(date);
        return baseDate.toString(format);
    }

    qsizetype strToDate(const QString& date, QString format = "dd.MM.yyyy") const {
        QDate baseDate = QDate::fromJulianDay(startDateExcel);
        QDate d = QDate::fromString(date, format);
        if (!d.isValid()) {
            //throw std::invalid_argument("Invalid date format: " + date.toStdString());
            return -1;
        }
        return d.toJulianDay() - baseDate.toJulianDay();
    }

    QHash<qsizetype, qsizetype> m_dates;
    qsizetype m_id = 0;
    QQueue<qsizetype> m_emptyIDs;
    mutable QReadWriteLock m_lock;
};

class Drawing {
public:
    Drawing(const QString number, const QString &title)
        : m_number(number)
        , m_title(title) {}

    QString getNumber() const {
        return m_number;
    }

    QString getTitle() const {
        return m_title;
    }

    bool operator==(const Drawing& other) const {
        return m_number == other.m_number && m_title == other.m_title;
    }

    bool operator<(const Drawing& other) const {
        if (m_number != other.m_number) {
            return m_number < other.m_number;
        }
        return m_title < other.m_title;
    }

private:
    QString m_number;
    QString m_title;
};

class DrawingsList final {
public:
    DrawingsList() {}

    qsizetype insert(const QString &number, const QString &title) {
        const QWriteLocker locker(&m_lock);
        auto drawing = std::make_shared<Drawing>(number, title);
        for (auto [k, v] : m_drawings.asKeyValueRange()) {
            if (*v == *drawing) {
                return k;
            }
        }

        qsizetype id;
        if (m_emptyIDs.isEmpty()) {
            ++m_id;
            id = m_id;
        }
        else {
            id = m_emptyIDs.first();
            m_emptyIDs.removeFirst();
        }
        m_drawings.insert(id, drawing);
        return id;
    }

    qsizetype insert(const std::shared_ptr<Drawing> drawing) {
        const QWriteLocker locker(&m_lock);
        for (auto [k, v] : m_drawings.asKeyValueRange()) {
            if (*v == *drawing) {
                return k;
            }
        }

        qsizetype id;
        if (m_emptyIDs.isEmpty()) {
            ++m_id;
            id = m_id;
        }
        else {
            id = m_emptyIDs.first();
            m_emptyIDs.removeFirst();
        }
        m_drawings.insert(id, drawing);
        return id;
    }

    bool remove(const std::shared_ptr<Drawing> drawing) {
        const QWriteLocker locker(&m_lock);
        qsizetype id = -1;
        for (auto [k, v] : m_drawings.asKeyValueRange()) {
            if (*v == *drawing) {
                id = k;
                break;
            }
        }
        if (id != -1) {
            m_drawings.remove(id);
            m_emptyIDs.append(id);
            return true;
        }
        return false;
    }

    qsizetype getID(const std::shared_ptr<Drawing> drawing) const {
        const QReadLocker locker(&m_lock);
        for (auto [k, v] : m_drawings.asKeyValueRange()) {
            if (*v == *drawing) {
                return k;
            }
        }
        return 0;
    }

    std::shared_ptr<Drawing> getDrawing(qsizetype id) const {
        const QReadLocker locker(&m_lock);
        return m_drawings.value(id, nullptr);
    }

    qsizetype size() const {
        const QReadLocker locker(&m_lock);
        return m_drawings.size();
    }

private:
    QHash<qsizetype, std::shared_ptr<Drawing>> m_drawings;
    qsizetype m_id = 0;
    QQueue<qsizetype> m_emptyIDs;
    mutable QReadWriteLock m_lock;
};

inline uint qHash(const Drawing& drawing, uint seed = 0) {
    return qHash(drawing.getNumber(), seed) ^ qHash(drawing.getTitle(), seed << 1);
}

struct Item {
    size_t date = 0;
    Drawing *drawing;
    size_t amount = 0;

    bool operator==(const Item& other) const {
        return date == other.date && *drawing == *other.drawing && amount == other.amount;
    }

    bool operator<(const Item& other) const {
        if (date < other.date) {
            return true;
        } else {
            if (drawing < other.drawing) {
                return true;
            } else {
                if (amount < other.amount) {
                    return true;
                } else {
                    return false;
                }
            }
        }
    }

    static inline QString dateFormat = "dd.MM.yyyy";

    static QString dateToStr(uint date) {
        QDate d(1899, 12, 30);
        d = d.addDays(date);
        return d.toString(dateFormat);
    }

    static uint strToDate(const QString& date) {
        QDate baseDate(1899, 12, 30);
        QDate d = QDate::fromString(date, dateFormat);
        if (!d.isValid()) {
            throw std::invalid_argument("Invalid date format: " + date.toStdString());
        }
        return d.toJulianDay() - baseDate.toJulianDay();
    }
};

struct ItemID {
    size_t date_id = 0;
    size_t drawing_id = 0;
    size_t amount_id = 0;

    bool operator==(const ItemID& other) const {
        return date_id == other.date_id && drawing_id == other.drawing_id && amount_id == other.amount_id;
    }
};

inline uint qHash(const ItemID& itemCache, uint seed = 0) {
    return qHash(itemCache.date_id, seed) ^ qHash(itemCache.drawing_id, seed << 1) ^ qHash(itemCache.amount_id, seed << 2);
}

struct ItemData {
    Item item;
    QStringList executors;
    QStringList authors;
    QStringList castingMaterials;
    QStringList modelMaterials;
    QStringList machines;
    QStringList notes;

    QString at(int index) const {
        switch (index) {
            case 0: {
                return Item::dateToStr(item.date);
            }
            case 1: {
                return item.drawing->getNumber();
            }
            case 2: {
                return item.drawing->getTitle();
            }
            case 3: {
                return executors.join(',');
            }
            case 4: {
                return authors.join(',');
            }
            case 5: {
                return QString::number(item.amount);
            }
            case 6: {
                return castingMaterials.join(',');
            }
            case 7: {
                return modelMaterials.join(',');
            }
            case 8: {
                return machines.join(',');
            }
            case 9: {
                return notes.join(',');
            }
            default: {
                QString();
            }
        }
        return QString();
    }
};

inline QTextStream& operator<<(QTextStream& out, const Drawing& drawing) {
    out << "[Name: " << drawing.getNumber() << ", Title: " << drawing.getTitle() << "]";
    return out;
}

inline QTextStream& operator<<(QTextStream& out, const Item& obj) {
    out << "[Date: " << obj.date << ", Drawing: " << obj.drawing << ", Amount: " << obj.amount << "]";
    return out;
}

inline QTextStream& operator<<(QTextStream& out, const ItemID& obj) {
    out << "[DateID: " << obj.date_id << ",\t DrawingID: " << obj.drawing_id << ",\t AmountID: " << obj.amount_id << "]";
    return out;
}

inline QDataStream &operator<<(QDataStream &out, const Drawing *data) {
    out << data->getNumber();
    out << data->getTitle();
    return out;
}

inline QDataStream &operator<<(QDataStream &out, const Item &data) {
    out << data.date;
    out << data.drawing->getNumber() << data.drawing->getTitle();
    out << data.amount;
    return out;
}

inline QDataStream &operator<<(QDataStream &out, const ItemID &data) {
    out << data.date_id;
    out << data.drawing_id;
    out << data.amount_id;
    return out;
}

inline QDataStream &operator>>(QDataStream &in, Drawing *data) {
    QString number;
    QString title;

    in >> number;
    in >> title;

    Drawing *drawing = new Drawing(number, title);
    qSwap(data, drawing);
    delete drawing;

    return in;
}

inline QDataStream &operator>>(QDataStream &in, Item &data) {
    in >> data.date;
    in >> data.drawing;
    in >> data.amount;
    return in;
}

inline QDataStream &operator>>(QDataStream &in, ItemID &data) {
    in >> data.date_id;
    in >> data.drawing_id;
    in >> data.amount_id;
    return in;
}

#endif // ITEMDATA_H
