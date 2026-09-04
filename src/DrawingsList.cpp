#include "DrawingsList.h"

std::optional<qsizetype> DrawingsList::insert(const Drawing &drawing) {
    return CustomList::insert(drawing);
}

bool DrawingsList::remove(const Drawing &drawing) {
    return CustomList::remove(drawing);
}

std::optional<qsizetype> DrawingsList::getID(const Drawing &drawing) const {
    return CustomList::getID(drawing);
}

std::optional<Drawing> DrawingsList::getValue(qsizetype id) const {
    const QReadLocker locker(&m_lock);
    if (auto it = m_list.find(id); it != m_list.end()) {
        return *it;
    }
    return std::nullopt;
}
