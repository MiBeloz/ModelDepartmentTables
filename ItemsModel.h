#ifndef ITEMSMODEL_H
#define ITEMSMODEL_H

#include <QSortFilterProxyModel>
#include <QStandardItemModel>


class ItemsModel : public QSortFilterProxyModel {
public:
    explicit ItemsModel(QObject *parent = nullptr);

    virtual ~ItemsModel() {};

    QString dateFormat;

protected:
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const override;

private:
    QStandardItemModel *m_itemModel;
};

#endif // ITEMSMODEL_H
