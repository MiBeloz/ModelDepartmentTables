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
  DatesList() { }

  qsizetype insert(
      const QString& date, QString format = "dd.MM.yyyy") {
    const QWriteLocker locker(&m_lock);
    qsizetype d = strToDate(date, format);
    if (d < 0) {
      return d;
    }

    if (auto id = m_dates.key(d, 0); id > 0) {
      return id;
    }

    qsizetype id;
    if (m_emptyIDs.isEmpty()) {
      ++m_id;
      id = m_id;
    } else {
      id = m_emptyIDs.first();
      m_emptyIDs.removeFirst();
    }
    m_dates.insert(id, d);
    return id;
  }

  bool remove(
      const QString& date, QString format = "dd.MM.yyyy") {
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

  qsizetype getID(
      const QString& date, QString format = "dd.MM.yyyy") const {
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

  QString getDate(
      qsizetype id, QString format = "dd.MM.yyyy") const {
    const QReadLocker locker(&m_lock);
    qsizetype d = m_dates.value(id, 0);

    if (d == 0) {
      return QString();
    } else {
      return dateToStr(d, format);
    }
  }

  qsizetype size() const {
    const QReadLocker locker(&m_lock);
    return m_dates.size();
  }

  void clear() {
    const QWriteLocker locker(&m_lock);
    m_dates.clear();
    m_emptyIDs.clear();
    m_id = 0;
  }

private:
  QString dateToStr(
      qsizetype date, QString format = "dd.MM.yyyy") const {
    QDate baseDate = QDate::fromJulianDay(startDateExcel);
    baseDate = baseDate.addDays(date);
    return baseDate.toString(format);
  }

  qsizetype strToDate(
      const QString& date, QString format = "dd.MM.yyyy") const {
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
  Drawing(
      const QString number, const QString& title)
      : m_number(number)
      , m_title(title) { }

  QString getNumber() const { return m_number; }

  QString getTitle() const { return m_title; }

  bool operator ==(
      const Drawing& other) const {
    return m_number == other.m_number;
  }

  bool operator <(
      const Drawing& other) const {
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
  DrawingsList() { }

  qsizetype insert(
      const QString& number, const QString& title) {
    const QWriteLocker locker(&m_lock);
    auto drawing = std::make_shared<Drawing>(number, title);
    for (auto [k, v] : m_drawings.asKeyValueRange()) {
      if (*v == *drawing) {
        return k;
      }
    }

    qsizetype id = 0;
    if (m_emptyIDs.isEmpty()) {
      ++m_id;
      id = m_id;
    } else {
      id = m_emptyIDs.first();
      m_emptyIDs.removeFirst();
    }
    m_drawings.insert(id, drawing);
    return id;
  }

  qsizetype insert(
      const std::shared_ptr<Drawing>& drawing) {
    return insert(drawing->getNumber(), drawing->getTitle());
  }

  bool remove(
      const std::shared_ptr<Drawing> drawing) {
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

  qsizetype getID(
      const std::shared_ptr<Drawing>& drawing) const {
    const QReadLocker locker(&m_lock);
    for (auto [k, v] : m_drawings.asKeyValueRange()) {
      if (*v == *drawing) {
        return k;
      }
    }
    return 0;
  }

  std::shared_ptr<Drawing> getDrawing(
      qsizetype id) const {
    const QReadLocker locker(&m_lock);
    return m_drawings.value(id, nullptr);
  }

  qsizetype size() const {
    const QReadLocker locker(&m_lock);
    return m_drawings.size();
  }

  void clear() {
    const QWriteLocker locker(&m_lock);
    m_drawings.clear();
    m_emptyIDs.clear();
    m_id = 0;
  }

private:
  QHash<qsizetype, std::shared_ptr<Drawing>> m_drawings;
  qsizetype m_id = 0;
  QQueue<qsizetype> m_emptyIDs;
  mutable QReadWriteLock m_lock;
};

inline uint qHash(
    const Drawing& drawing, uint seed = 0) {
  return qHash(drawing.getNumber(), seed) ^ qHash(drawing.getTitle(), seed << 1);
}

template<typename T>
class CustomList final {
public:
  CustomList() { }

  qsizetype insert(
      const T& data) {
    const QWriteLocker locker(&m_lock);
    if (auto id = m_customData.key(data, 0); id > 0) {
      return id;
    }

    qsizetype id;
    if (m_emptyIDs.isEmpty()) {
      ++m_id;
      id = m_id;
    } else {
      id = m_emptyIDs.first();
      m_emptyIDs.removeFirst();
    }
    m_customData.insert(id, data);
    return id;
  }

  bool remove(
      const T& data) {
    const QWriteLocker locker(&m_lock);
    qsizetype id;
    if (id = m_customData.key(data, 0); id != 0) {
      m_customData.remove(id);
      m_emptyIDs.append(id);
      return true;
    }
    return false;
  }

  qsizetype getID(
      const T& data) const {
    const QReadLocker locker(&m_lock);
    return m_customData.key(data, 0);
  }

  T getValue(
      qsizetype id) const {
    const QReadLocker locker(&m_lock);
    return m_customData.value(id, T());
  }

  qsizetype size() const {
    const QReadLocker locker(&m_lock);
    return m_customData.size();
  }

  void clear() {
    const QWriteLocker locker(&m_lock);
    m_customData.clear();
    m_emptyIDs.clear();
    m_id = 0;
  }

private:
  QHash<qsizetype, T> m_customData;
  qsizetype m_id = 0;
  QQueue<qsizetype> m_emptyIDs;
  mutable QReadWriteLock m_lock;
};

template<typename T>
class IRepository {
public:
  virtual ~IRepository() = default;

  virtual qsizetype add(const T& value) = 0;
  virtual bool remove(const T& value) = 0;
  virtual std::optional<qsizetype> findId(const T& value) const = 0;
  virtual std::optional<T> findValue(qsizetype id) const = 0;
  virtual qsizetype count() const = 0;
  virtual void clear() = 0;
};

class StringRepository : public IRepository<QString> {
public:
  StringRepository() = default;

  qsizetype add(
      const QString& value) override {
    return m_list.insert(value);
  }

  bool remove(
      const QString& value) override {
    return m_list.remove(value);
  }

  std::optional<qsizetype> findId(
      const QString& value) const override {
    qsizetype id = m_list.getID(value);
    if (id != 0) {
      return id;
    }
    return std::nullopt;
  }

  std::optional<QString> findValue(
      qsizetype id) const override {
    QString value = m_list.getValue(id);
    if (!value.isEmpty()) {
      return value;
    }
    return std::nullopt;
  }

  qsizetype count() const override { return m_list.size(); }

  void clear() override { m_list.clear(); }

private:
  CustomList<QString> m_list;
};

class NumericRepository : public IRepository<int> {
public:
  NumericRepository() = default;

  qsizetype add(
      const int& value) override {
    return m_list.insert(value);
  }

  bool remove(
      const int& value) override {
    return m_list.remove(value);
  }

  std::optional<qsizetype> findId(
      const int& value) const override {
    qsizetype id = m_list.getID(value);
    if (id != 0) {
      return id;
    }
    return std::nullopt;
  }

  std::optional<int> findValue(
      qsizetype id) const override {
    int value = m_list.getValue(id);
    if (value != 0) {
      return value;
    }
    return std::nullopt;
  }

  qsizetype count() const override { return m_list.size(); }

  void clear() override { m_list.clear(); }

private:
  CustomList<int> m_list;
};

class DateRepository {
public:
  DateRepository() = default;

  qsizetype addDate(
      const QString& date, const QString& format = "dd.MM.yyyy") {
    return m_dates.insert(date, format);
  }

  bool removeDate(
      const QString& date, const QString& format = "dd.MM.yyyy") {
    return m_dates.remove(date, format);
  }

  std::optional<qsizetype> findDateId(
      const QString& date, const QString& format = "dd.MM.yyyy") const {
    qsizetype id = m_dates.getID(date, format);
    if (id != 0) {
      return id;
    }
    return std::nullopt;
  }

  std::optional<QString> findDate(
      qsizetype id, const QString& format = "dd.MM.yyyy") const {
    QString date = m_dates.getDate(id, format);
    if (!date.isEmpty()) {
      return date;
    }
    return std::nullopt;
  }

  qsizetype count() const { return m_dates.size(); }

  void clear() { m_dates.clear(); }

  void setDateFormat(
      const QString& format) {
    m_format = format;
  }

  QString getDateFormat() const { return m_format; }

private:
  DatesList m_dates;
  QString m_format = "dd.MM.yyyy";
};

class DrawingRepository {
public:
  DrawingRepository() = default;

  qsizetype addDrawing(
      const QString& number, const QString& title) {
    return m_drawings.insert(number, title);
  }

  qsizetype addDrawing(
      const std::shared_ptr<Drawing>& drawing) {
    return m_drawings.insert(drawing);
  }

  bool removeDrawing(
      const std::shared_ptr<Drawing>& drawing) {
    return m_drawings.remove(drawing);
  }

  bool removeDrawing(
      const QString& number, const QString& title) {
    auto drawing = std::make_shared<Drawing>(number, title);
    return m_drawings.remove(drawing);
  }

  std::optional<qsizetype> findDrawingId(
      const std::shared_ptr<Drawing>& drawing) const {
    qsizetype id = m_drawings.getID(drawing);
    if (id != 0) {
      return id;
    }
    return std::nullopt;
  }

  std::optional<std::shared_ptr<Drawing>> findDrawing(
      qsizetype id) const {
    auto drawing = m_drawings.getDrawing(id);
    if (drawing) {
      return drawing;
    }
    return std::nullopt;
  }

  qsizetype count() const { return m_drawings.size(); }

private:
  DrawingsList m_drawings;
};

class RepositoryFactory {
public:
  static std::unique_ptr<StringRepository> createStringRepository() {
    return std::make_unique<StringRepository>();
  }

  static std::unique_ptr<NumericRepository> createNumericRepository() {
    return std::make_unique<NumericRepository>();
  }

  static std::unique_ptr<DateRepository> createDateRepository() {
    return std::make_unique<DateRepository>();
  }

  static std::unique_ptr<DrawingRepository> createDrawingRepository() {
    return std::make_unique<DrawingRepository>();
  }
};

class CatalogService {
public:
  CatalogService()
      : m_dateRepo(std::make_unique<DateRepository>())
      , m_drawingRepo(std::make_unique<DrawingRepository>())
      , m_executorRepo(std::make_unique<StringRepository>())
      , m_authorRepo(std::make_unique<StringRepository>())
      , m_castingMaterialRepo(std::make_unique<StringRepository>())
      , m_modelMaterialRepo(std::make_unique<StringRepository>())
      , m_machineRepo(std::make_unique<StringRepository>())
      , m_noteRepo(std::make_unique<StringRepository>())
      , m_amountRepo(std::make_unique<NumericRepository>()) { }

  DateRepository& dates() { return *m_dateRepo; }
  const DateRepository& dates() const { return *m_dateRepo; }

  DrawingRepository& drawings() { return *m_drawingRepo; }
  const DrawingRepository& drawings() const { return *m_drawingRepo; }

  StringRepository& executors() { return *m_executorRepo; }
  const StringRepository& executors() const { return *m_executorRepo; }

  StringRepository& authors() { return *m_authorRepo; }
  const StringRepository& authors() const { return *m_authorRepo; }

  StringRepository& castingMaterials() { return *m_castingMaterialRepo; }
  const StringRepository& castingMaterials() const { return *m_castingMaterialRepo; }

  StringRepository& modelMaterials() { return *m_modelMaterialRepo; }
  const StringRepository& modelMaterials() const { return *m_modelMaterialRepo; }

  StringRepository& machines() { return *m_machineRepo; }
  const StringRepository& machines() const { return *m_machineRepo; }

  StringRepository& notes() { return *m_noteRepo; }
  const StringRepository& notes() const { return *m_noteRepo; }

  NumericRepository& amounts() { return *m_amountRepo; }
  const NumericRepository& amounts() const { return *m_amountRepo; }

  qsizetype insertDate(
      const QString& date) {
    return m_dateRepo->addDate(date);
  }

  QString getDate(
      qsizetype id) const {
    auto result = m_dateRepo->findDate(id);
    return result.value_or(QString());
  }

  qsizetype insertDrawing(
      const QString& number, const QString& title) {
    return m_drawingRepo->addDrawing(number, title);
  }

  std::shared_ptr<Drawing> getDrawing(
      qsizetype id) const {
    auto result = m_drawingRepo->findDrawing(id);
    return result.value_or(nullptr);
  }

private:
  std::unique_ptr<DateRepository> m_dateRepo;
  std::unique_ptr<DrawingRepository> m_drawingRepo;
  std::unique_ptr<StringRepository> m_executorRepo;
  std::unique_ptr<StringRepository> m_authorRepo;
  std::unique_ptr<StringRepository> m_castingMaterialRepo;
  std::unique_ptr<StringRepository> m_modelMaterialRepo;
  std::unique_ptr<StringRepository> m_machineRepo;
  std::unique_ptr<StringRepository> m_noteRepo;
  std::unique_ptr<NumericRepository> m_amountRepo;
};

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
