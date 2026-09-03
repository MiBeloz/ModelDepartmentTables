#ifndef ITEMDATA_H
#define ITEMDATA_H

#include <QDate>
#include <QDebug>
#include <QHash>
#include <QQueue>
#include <QReadWriteLock>

#include "Repository.h"

struct Item {
    size_t date = 0;
    Drawing* drawing;
    size_t amount = 0;

    bool operator ==(const Item& other) const {
        return date == other.date && *drawing == *other.drawing && amount == other.amount;
    }

    bool operator <(const Item& other) const {
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

    bool operator ==(const ItemID& other) const {
        return date_id == other.date_id && drawing_id == other.drawing_id &&
               amount_id == other.amount_id;
    }
};

inline uint qHash(const ItemID& itemCache, uint seed = 0) {
    return qHash(itemCache.date_id, seed) ^ qHash(itemCache.drawing_id, seed << 1) ^
           qHash(itemCache.amount_id, seed << 2);
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

inline QTextStream& operator <<(QTextStream& out, const Drawing& drawing) {
    out << "[Name: " << drawing.getNumber() << ", Title: " << drawing.getTitle() << "]";
    return out;
}

inline QTextStream& operator <<(QTextStream& out, const Item& obj) {
    out << "[Date: " << obj.date << ", Drawing: " << obj.drawing << ", Amount: " << obj.amount
        << "]";
    return out;
}

inline QTextStream& operator <<(QTextStream& out, const ItemID& obj) {
    out << "[DateID: " << obj.date_id << ",\t DrawingID: " << obj.drawing_id
        << ",\t AmountID: " << obj.amount_id << "]";
    return out;
}

inline QDataStream& operator <<(QDataStream& out, const Drawing* data) {
    out << data->getNumber();
    out << data->getTitle();
    return out;
}

// inline QDataStream &operator<<(QDataStream &out, const Item &data) {
//     out << data.date;
//     out << data.drawing->getNumber() << data.drawing->getTitle();
//     out << data.amount;
//     return out;
// }

// inline QDataStream &operator<<(QDataStream &out, const ItemID &data) {
//     out << data.date_id;
//     out << data.drawing_id;
//     out << data.amount_id;
//     return out;
// }

inline QDataStream& operator >>(QDataStream& in, Drawing* data) {
    QString number;
    QString title;

    in >> number;
    in >> title;

    Drawing* drawing = new Drawing(number, title);
    qSwap(data, drawing);
    delete drawing;

    return in;
}

// inline QDataStream &operator>>(QDataStream &in, Item &data) {
//     in >> data.date;
//     in >> data.drawing;
//     in >> data.amount;
//     return in;
// }

// inline QDataStream &operator>>(QDataStream &in, ItemID &data) {
//     in >> data.date_id;
//     in >> data.drawing_id;
//     in >> data.amount_id;
//     return in;
// }

#endif // ITEMDATA_H
