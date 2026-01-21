#include <QDate>

#include "ItemsModel.h"
#include "TableHeaders.h"
#include "ItemData.h"


ItemsModel::ItemsModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{
    m_itemModel = new QStandardItemModel(this);
    m_itemModel->setHorizontalHeaderLabels(TableHeaders::itemsTableHeaders());
    setSourceModel(m_itemModel);
}

bool ItemsModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
    QString leftData = sourceModel()->data(left).toString();
    QString rightData = sourceModel()->data(right).toString();

    QDate leftDate;
    QDate rightDate;
    leftDate = QDate::fromString(leftData, Item::dateFormat);
    rightDate = QDate::fromString(rightData, Item::dateFormat);

    if (!leftDate.isValid() || !rightDate.isValid()) {
        return QString::localeAwareCompare(leftData, rightData) < 0;
    }

    return leftDate < rightDate;
}
