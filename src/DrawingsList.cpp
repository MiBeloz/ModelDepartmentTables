#include "DrawingsList.h"

std::optional<qsizetype> DrawingsList::insert(const QString &number, const QString &title) {
    const QWriteLocker locker(&m_lock);
    Drawing drawing(number, title);
    for (auto [k, v] : m_list.asKeyValueRange()) {
        if (v == drawing) {
            return k;
        }
    }

    qsizetype id = -1;
    if (m_emptyIDs.isEmpty()) {
        ++m_id;
        id = m_id;
    } else {
        id = m_emptyIDs.first();
        m_emptyIDs.removeFirst();
    }
    m_list.insert(id, drawing);
    return id;
}

std::optional<qsizetype> DrawingsList::insert(const Drawing &drawing) {
    return insert(drawing.getNumber(), drawing.getTitle());
}

bool DrawingsList::remove(const Drawing &drawing) {
    const QWriteLocker locker(&m_lock);
    qsizetype id = -1;
    for (auto [k, v] : m_list.asKeyValueRange()) {
        if (v == drawing) {
            id = k;
            break;
        }
    }

    if (id != -1) {
        m_list.remove(id);
        m_emptyIDs.append(id);
        return true;
    }
    return false;
}

std::optional<qsizetype> DrawingsList::getID(const Drawing &drawing) const {
    const QReadLocker locker(&m_lock);
    for (auto [k, v] : m_list.asKeyValueRange()) {
        if (v == drawing) {
            return k;
        }
    }
    return std::nullopt;
}

std::optional<Drawing> DrawingsList::getValue(qsizetype id) const {
    const QReadLocker locker(&m_lock);
    auto it = m_list.find(id);
    if (it != m_list.end()) {
        return *it;
    }
    return std::nullopt;
}

QList<Drawing> DrawingsList::getAllValues() const {
    return m_list.values();
}
