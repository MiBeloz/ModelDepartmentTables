#ifndef NATURALSORTLISTMODEL_H
#define NATURALSORTLISTMODEL_H

#include <QPointer>
#include <QSortFilterProxyModel>
#include <QStringListModel>

class NaturalSortListModel : public QSortFilterProxyModel {
public:
    NaturalSortListModel(QObject *parent = nullptr);
    NaturalSortListModel(const QStringList &strings, QObject *parent = nullptr);

    QStringList stringList() const;
    void setStringList(const QStringList &strings);

protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

private:
    QPointer<QStringListModel> m_stringListModel;

    static bool naturalLess(const QString &a, const QString &b);
};

#endif // NATURALSORTLISTMODEL_H
