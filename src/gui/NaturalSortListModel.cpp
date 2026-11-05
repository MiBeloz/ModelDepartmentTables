#include "NaturalSortListModel.h"

#include <QSet>
#include <QStringList>
#include <QVariant>

NaturalSortListModel::NaturalSortListModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_stringListModel(new QStringListModel(this)) {
    m_collator.setNumericMode(true);
    m_collator.setCaseSensitivity(Qt::CaseInsensitive);

    setSourceModel(m_stringListModel);
    sort(0, Qt::AscendingOrder);
}

void NaturalSortListModel::setStringList(const QStringList &strings) {
    m_stringListModel->setStringList(deduplicate(strings));
}

QStringList NaturalSortListModel::sortedStringList() const {
    QStringList result;
    const int rows = rowCount();
    result.reserve(rows);
    for (int row = 0; row < rows; ++row) {
        result.append(data(index(row, 0), Qt::DisplayRole).toString());
    }
    return result;
}

bool NaturalSortListModel::appendString(const QString &text) {
    if (containsCaseInsensitive(text)) {
        return false;
    }

    const int row = m_stringListModel->rowCount();
    if (!m_stringListModel->insertRows(row, 1)) {
        return false;
    }
    if (!m_stringListModel->setData(m_stringListModel->index(row), text, Qt::EditRole)) {
        m_stringListModel->removeRows(row, 1);
        return false;
    }
    return true;
}

bool NaturalSortListModel::removeString(const QString &text) {
    const QString value = text.toCaseFolded();
    const QStringList list = m_stringListModel->stringList();
    for (int i = 0; i < list.count(); ++i) {
        if (list[i].toCaseFolded() == value) {
            return m_stringListModel->removeRows(i, 1);
        }
    }
    return false;
}

bool NaturalSortListModel::lessThan(const QModelIndex &source_left,
                                    const QModelIndex &source_right) const {
    QVariant leftData = sourceModel()->data(source_left, sortRole());
    QVariant rightData = sourceModel()->data(source_right, sortRole());

    if (source_left.column() == 0 && leftData.typeId() == QMetaType::QString) {
        return m_collator.compare(leftData.toString(), rightData.toString()) < 0;
    }

    return QSortFilterProxyModel::lessThan(source_left, source_right);
}

QStringList NaturalSortListModel::deduplicate(const QStringList &strings) const {
    QStringList result;
    result.reserve(strings.size());
    QSet<QString> seen;
    for (const QString &s : strings) {
        const QString key = s.toCaseFolded();
        if (!seen.contains(key)) {
            seen.insert(key);
            result.append(s);
        }
    }
    return result;
}

bool NaturalSortListModel::containsCaseInsensitive(const QString &text) const {
    const QString value = text.toCaseFolded();
    const QStringList current = m_stringListModel->stringList();
    for (const QString &existing : current) {
        if (existing.toCaseFolded() == value) {
            return true;
        }
    }
    return false;
}
