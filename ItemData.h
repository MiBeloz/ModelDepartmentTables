#ifndef ITEMDATA_H
#define ITEMDATA_H

#include <QStringList>
#include <QDate>


inline QString dateToStr(uint date) {
    QDate d(1899, 12, 30);
    d = d.addDays(date);
    return d.toString("dd.MM.yyyy");
}

inline uint strToDate(const QString& date) {
    QDate baseDate(1899, 12, 30);
    QDate d = QDate::fromString(date, "dd.MM.yyyy");
    if (!d.isValid()) {
        throw std::invalid_argument("Invalid date format: " + date.toStdString());
    }
    return d.toJulianDay() - baseDate.toJulianDay();
}

struct Drawing {
    Drawing() {}
    Drawing(const Drawing& other) {
        number = other.number;
        title = other.title;
    }

    QString number;
    QString title;

    bool operator==(const Drawing& other) const {
        return number == other.number && title == other.title;
    }
};

// inline uint qHash(const Drawing& drawing, uint seed = 0) {
//     return qHash(drawing.name, seed) ^ qHash(drawing.title, seed << 1);
// }

struct Item {
    uint date = 0;
    Drawing drawing;
    uint amount = 0;
};

// struct ItemCache {
//     uint date_id = 0;
//     uint drawing_id = 0;
//     uint amount_id = 0;

//     bool operator==(const ItemCache& other) const {
//         return date_id == other.date_id && drawing_id == other.drawing_id && amount_id == other.amount_id;
//     }
// };

// inline uint qHash(const ItemCache& itemCache, uint seed = 0) {
//     return qHash(itemCache.date_id, seed) ^ qHash(itemCache.drawing_id, seed << 1) ^ qHash(itemCache.amount_id, seed << 2);
// }

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
                return dateToStr(item.date);
            }
            case 1: {
                return item.drawing.number;
            }
            case 2: {
                return item.drawing.title;
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

#endif // ITEMDATA_H
