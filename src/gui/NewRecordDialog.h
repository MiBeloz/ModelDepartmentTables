#ifndef NEWRECORDDIALOG_H
#define NEWRECORDDIALOG_H

#include <QDialog>
#include <QEvent>
#include <QPointer>
#include <QSortFilterProxyModel>
#include <QStandardItemModel>
#include <QStringListModel>

#include "StorageManager.h"
#include "Utils.h"

class StringSortFilterProxyModel : public QSortFilterProxyModel {
public:
    StringSortFilterProxyModel(QObject *parent = nullptr) : QSortFilterProxyModel(parent) { }

    using QSortFilterProxyModel::QSortFilterProxyModel;

    void refreshFilter() {
        beginFilterChange();
        endFilterChange(QSortFilterProxyModel::Direction::Rows);
    }

protected:
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const override {
        QString leftData = sourceModel()->data(source_left).toString();
        QString rightData = sourceModel()->data(source_right).toString();

        return naturalLess(leftData, rightData);
    }
};

namespace Ui {
    class NewRecordDialog;
}

class NewRecordDialog final : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(NewRecordDialog)

public:
    explicit NewRecordDialog(QWidget *parent = nullptr);
    ~NewRecordDialog();

signals:
    //void sig_handleNewItem(const ItemData &newItem);

private slots:
    void on_pushButtonOk_clicked();
    void on_pushButtonCancel_clicked();

private:
    Ui::NewRecordDialog *m_ui;
    mdt::work::StorageManager m_storageWork;
    QPointer<QStandardItemModel> m_sourceModelDrawings;
    QPointer<QStringListModel> m_stringListModelExecutors;
    QPointer<StringSortFilterProxyModel> m_sortFilterProxyModelExecutors;

    void init();
};

#endif // NEWRECORDDIALOG_H
