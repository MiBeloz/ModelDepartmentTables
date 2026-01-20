#include <QDate>

#include "ItemsModel.h"
#include "TableHeaders.h"


ItemsModel::ItemsModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{
    m_itemModel = new QStandardItemModel(this);
    m_itemModel->setHorizontalHeaderLabels(TableHeaders::itemsTableHeaders());
    dateFormat = "dd.MM.yyyy";
    setSourceModel(m_itemModel);
}

bool ItemsModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
    QString leftData = sourceModel()->data(left).toString();
    QString rightData = sourceModel()->data(right).toString();

    QDate leftDate;
    QDate rightDate;
    // if (dateFormat == DateFormat::MMyyyy) {
    //     leftDate = QDate::fromString(leftData, "MM-yyyy");
    //     rightDate = QDate::fromString(rightData, "MM-yyyy");
    // }
    // else {
    leftDate = QDate::fromString(leftData, dateFormat);
    rightDate = QDate::fromString(rightData, dateFormat);
    //}

    if (!leftDate.isValid() || !rightDate.isValid()) {
        return QString::localeAwareCompare(leftData, rightData) < 0;
    }

    return leftDate < rightDate;
}
