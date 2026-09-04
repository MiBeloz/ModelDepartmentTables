#include "Repository.h"

std::optional<qsizetype> DatesRepository::add(const QString &date) {
    DatesList *datesListPtr = dynamic_cast<DatesList *>(m_list.get());
    if (datesListPtr) {
        return datesListPtr->insert(date);
    }
    return std::nullopt;
}

bool DatesRepository::remove(const QString &date) {
    return this->remove(date);
}

std::optional<qsizetype> DatesRepository::findId(const QString &date) const {
    return this->findId(date);
}

std::optional<QString> DatesRepository::findStrValue(qsizetype id) const {
    return this->findStrValue(id);
}

void DatesRepository::setDateFormat(const QString &format) {
    this->setDateFormat(format);
}

QString DatesRepository::getDateFormat() const {
    return this->getDateFormat();
}

DatesListError::ErrorType DatesRepository::lastError() const {
    return this->lastError();
}

DatesRepository &RepositoryService::dates() {
    return *m_dateRepo;
}

const DatesRepository &RepositoryService::dates() const {
    return *m_dateRepo;
}

DrawingsRepository &RepositoryService::drawings() {
    return *m_drawingRepo;
}

const DrawingsRepository &RepositoryService::drawings() const {
    return *m_drawingRepo;
}

CustomRepository<QString> &RepositoryService::executors() {
    return *m_executorRepo;
}

const CustomRepository<QString> &RepositoryService::executors() const {
    return *m_executorRepo;
}

CustomRepository<QString> &RepositoryService::authors() {
    return *m_authorRepo;
}

const CustomRepository<QString> &RepositoryService::authors() const {
    return *m_authorRepo;
}

CustomRepository<QString> &RepositoryService::castingMaterials() {
    return *m_castingMaterialRepo;
}

const CustomRepository<QString> &RepositoryService::castingMaterials() const {
    return *m_castingMaterialRepo;
}

CustomRepository<QString> &RepositoryService::modelMaterials() {
    return *m_modelMaterialRepo;
}

const CustomRepository<QString> &RepositoryService::modelMaterials() const {
    return *m_modelMaterialRepo;
}

CustomRepository<QString> &RepositoryService::machines() {
    return *m_machineRepo;
}

const CustomRepository<QString> &RepositoryService::machines() const {
    return *m_machineRepo;
}

CustomRepository<QString> &RepositoryService::notes() {
    return *m_noteRepo;
}

const CustomRepository<QString> &RepositoryService::notes() const {
    return *m_noteRepo;
}

CustomRepository<int> &RepositoryService::amounts() {
    return *m_amountRepo;
}

const CustomRepository<int> &RepositoryService::amounts() const {
    return *m_amountRepo;
}

std::optional<qsizetype> DrawingsRepository::add(const Drawing &drawing) {
    DrawingsList *drawingsListPtr = dynamic_cast<DrawingsList *>(m_list.get());
    if (drawingsListPtr) {
        return drawingsListPtr->insert(drawing);
    }
    return std::nullopt;
}

std::optional<Drawing> DrawingsRepository::findValue(qsizetype id) const {
    DrawingsList *drawingsListPtr = dynamic_cast<DrawingsList *>(m_list.get());
    if (drawingsListPtr) {
        return drawingsListPtr->getValue(id);
    }
    return std::nullopt;
}
