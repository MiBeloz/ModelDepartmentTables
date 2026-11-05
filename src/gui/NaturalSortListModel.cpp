#include "NaturalSortListModel.h"

NaturalSortListModel::NaturalSortListModel(QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_stringListModel(new QStringListModel(this)) {
    m_collator.setNumericMode(true);
    m_collator.setCaseSensitivity(Qt::CaseInsensitive);

    setSourceModel(m_stringListModel);
    sort(0, Qt::AscendingOrder);
}

NaturalSortListModel::NaturalSortListModel(const QStringList &strings, QObject *parent)
    : QSortFilterProxyModel(parent)
    , m_stringListModel(new QStringListModel(strings, this)) {
    m_collator.setNumericMode(true);
    m_collator.setCaseSensitivity(Qt::CaseInsensitive);

    setSourceModel(m_stringListModel);
    sort(0, Qt::AscendingOrder);
}

QStringList NaturalSortListModel::stringList() const {
    return m_stringListModel->stringList();
}

void NaturalSortListModel::setStringList(const QStringList &strings) {
    m_stringListModel->setStringList(strings);
}

bool NaturalSortListModel::lessThan(const QModelIndex &source_left,
                                    const QModelIndex &source_right) const {
    QVariant leftData = sourceModel()->data(source_left, sortRole());
    QVariant rightData = sourceModel()->data(source_right, sortRole());

    if (leftData.typeId() == QMetaType::QString) {
        return m_collator.compare(leftData.toString(), rightData.toString()) < 0;
    }

    return QSortFilterProxyModel::lessThan(source_left, source_right);
}
