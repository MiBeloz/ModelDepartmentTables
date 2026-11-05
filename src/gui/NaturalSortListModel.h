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

    void setStringList(const QStringList &strings);
    QStringList sortedStringList() const;

    bool appendString(const QString &text);
    bool removeString(const QString &text);

protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override;

private:
    QStringListModel *m_stringListModel;
    QCollator m_collator;

    QStringList deduplicate(const QStringList &strings) const;
    bool containsCaseInsensitive(const QString &text) const;
};

#endif // NATURALSORTLISTMODEL_H
