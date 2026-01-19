#include <QDate>

#include "ItemsModel.h"
#include "TableHeaders.h"


ItemsModel::ItemsModel(QObject *parent)
    : QSortFilterProxyModel{parent}
{
    m_itemModel = new QStandardItemModel;
    m_itemModel->setHorizontalHeaderLabels(TableHeaders::itemsTableHeaders());
    dateFormat = "dd.MM.yyyy";
    setSourceModel(m_itemModel);
}

ItemsModel::~ItemsModel() {
    delete m_itemModel;
}

bool ItemsModel::lessThan(const QModelIndex &left, const QModelIndex &right) const {
    // Получаем строки из исходной модели
    QString leftData = sourceModel()->data(left).toString();
    QString rightData = sourceModel()->data(right).toString();

    // Преобразуем строки в QDate
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

    // Если строки не удалось преобразовать, выполняем строковое сравнение
    if (!leftDate.isValid() || !rightDate.isValid()) {
        return QString::localeAwareCompare(leftData, rightData) < 0;
    }

    // Сравниваем даты
    return leftDate < rightDate;
}
