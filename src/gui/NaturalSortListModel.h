#ifndef NATURALSORTLISTMODEL_H
#define NATURALSORTLISTMODEL_H

#include <QCollator>
#include <QSortFilterProxyModel>
#include <QStringListModel>

class NaturalSortListModel final : public QSortFilterProxyModel {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(NaturalSortListModel)

public:
    NaturalSortListModel(QObject *parent = nullptr);
    NaturalSortListModel(const QStringList &strings, QObject *parent = nullptr);

    QStringList stringList() const;
    void setStringList(const QStringList &strings);

protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

private:
    QStringListModel *m_stringListModel;
    QCollator m_collator;
};

#endif // NATURALSORTLISTMODEL_H
