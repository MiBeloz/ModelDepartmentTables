#ifndef NEWRECORDDIALOG_H
#define NEWRECORDDIALOG_H

#include "NaturalSortListModel.h"
#include "StorageManager.h"

#include <QDialog>
#include <QPointer>
#include <QStandardItemModel>
#include <QStringListModel>

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
    QPointer<NaturalSortListModel> m_NaturalSortListModelExecutors;

    void init();
};

#endif // NEWRECORDDIALOG_H
