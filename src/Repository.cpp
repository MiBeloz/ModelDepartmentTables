#include "Repository.h"

std::optional<qsizetype> StringRepository::add(const QString &value) {
    if (auto id = m_list.insert(value); id.has_value()) {
        return id;
    }
    return std::nullopt;
}

bool StringRepository::remove(const QString &value) {
    return m_list.remove(value);
}

std::optional<qsizetype> StringRepository::findId(const QString &value) const {
    if (auto id = m_list.getID(value); id.has_value()) {
        return id;
    }
    return std::nullopt;
}

std::optional<QString> StringRepository::findValue(qsizetype id) const {
    if (auto value = m_list.getValue(id); value.has_value()) {
        return value;
    }
    return std::nullopt;
}

qsizetype StringRepository::count() const {
    return m_list.size();
}

void StringRepository::clear() {
    m_list.clear();
}

std::optional<qsizetype> NumericRepository::add(const int &value) {
    if (auto id = m_list.insert(value); id.has_value()) {
        return id;
    }
    return std::nullopt;
}

bool NumericRepository::remove(const int &value) {
    return m_list.remove(value);
}

std::optional<qsizetype> NumericRepository::findId(const int &value) const {
    if (auto id = m_list.getID(value); id.has_value()) {
        return id;
    }
    return std::nullopt;
}

std::optional<int> NumericRepository::findValue(qsizetype id) const {
    if (auto value = m_list.getValue(id); value.has_value()) {
        return value;
    }
    return std::nullopt;
}

qsizetype NumericRepository::count() const {
    return m_list.size();
}

void NumericRepository::clear() {
    m_list.clear();
}

std::optional<qsizetype> DateRepository::add(const QString &date) {
    if (auto id = m_dates.insert(date); id.has_value()) {
        return id;
    }
    return std::nullopt;
}

bool DateRepository::remove(const QString &date) {
    return m_dates.remove(date);
}

std::optional<qsizetype> DateRepository::findId(const QString &date) const {
    if (auto id = m_dates.getID(date); id.has_value()) {
        return id;
    }
    return std::nullopt;
}

std::optional<QString> DateRepository::findValue(qsizetype id) const {
    if (auto date = m_dates.getValue(id); date.has_value()) {
        return date;
    }
    return std::nullopt;
}

qsizetype DateRepository::count() const {
    return m_dates.size();
}

void DateRepository::clear() {
    m_dates.clear();
}

void DateRepository::setDateFormat(const QString &format) {
    m_dates.setDateFormat(format);
}

QString DateRepository::getDateFormat() const {
    return m_dates.getDateFormat();
}

DatesListError::ErrorType DateRepository::lastError() const {
    return m_dates.lastError();
}

std::optional<qsizetype> DrawingRepository::add(const Drawing &drawing) {
    if (auto id = m_drawings.insert(drawing); id.has_value()) {
        return id;
    }
    return std::nullopt;
}

std::optional<qsizetype> DrawingRepository::add(const QString &number, const QString &title) {
    if (auto id = m_drawings.insert(number, title); id.has_value()) {
        return id;
    }
    return std::nullopt;
}

bool DrawingRepository::remove(const Drawing &drawing) {
    return m_drawings.remove(drawing);
}

bool DrawingRepository::remove(const QString &number, const QString &title) {
    Drawing drawing(number, title);
    return m_drawings.remove(drawing);
}

std::optional<qsizetype> DrawingRepository::findId(const Drawing &drawing) const {
    if (auto id = m_drawings.getID(drawing); id.has_value()) {
        return id;
    }
    return std::nullopt;
}

std::optional<Drawing> DrawingRepository::findValue(qsizetype id) const {
    if (auto drawing = m_drawings.getValue(id); drawing.has_value()) {
        return drawing;
    }
    return std::nullopt;
}

QList<Drawing> DrawingRepository::getAllValues() const {
    return m_drawings.getAllValues();
}

qsizetype DrawingRepository::count() const {
    return m_drawings.size();
}

void DrawingRepository::clear() {
    m_drawings.clear();
}

DateRepository &RepositoryService::dates() {
    return *m_dateRepo;
}

const DateRepository &RepositoryService::dates() const {
    return *m_dateRepo;
}

DrawingRepository &RepositoryService::drawings() {
    return *m_drawingRepo;
}

const DrawingRepository &RepositoryService::drawings() const {
    return *m_drawingRepo;
}

StringRepository &RepositoryService::executors() {
    return *m_executorRepo;
}

const StringRepository &RepositoryService::executors() const {
    return *m_executorRepo;
}

StringRepository &RepositoryService::authors() {
    return *m_authorRepo;
}

const StringRepository &RepositoryService::authors() const {
    return *m_authorRepo;
}

StringRepository &RepositoryService::castingMaterials() {
    return *m_castingMaterialRepo;
}

const StringRepository &RepositoryService::castingMaterials() const {
    return *m_castingMaterialRepo;
}

StringRepository &RepositoryService::modelMaterials() {
    return *m_modelMaterialRepo;
}

const StringRepository &RepositoryService::modelMaterials() const {
    return *m_modelMaterialRepo;
}

StringRepository &RepositoryService::machines() {
    return *m_machineRepo;
}

const StringRepository &RepositoryService::machines() const {
    return *m_machineRepo;
}

StringRepository &RepositoryService::notes() {
    return *m_noteRepo;
}

const StringRepository &RepositoryService::notes() const {
    return *m_noteRepo;
}

NumericRepository &RepositoryService::amounts() {
    return *m_amountRepo;
}

const NumericRepository &RepositoryService::amounts() const {
    return *m_amountRepo;
}
