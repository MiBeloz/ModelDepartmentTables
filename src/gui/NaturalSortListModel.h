#ifndef NATURALSORTLISTMODEL_H
#define NATURALSORTLISTMODEL_H

#include <QSortFilterProxyModel>
#include <QStringListModel>

class NaturalSortListModel : public QSortFilterProxyModel {
public:
    NaturalSortListModel(const QStringList &strings, QObject *parent = nullptr);

protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

private:
    QStringListModel *m_stringListModel;

    static bool naturalLess(const QString &a, const QString &b);
};

#endif // NATURALSORTLISTMODEL_H
