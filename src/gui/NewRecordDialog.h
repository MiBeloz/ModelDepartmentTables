#ifndef NEWRECORDDIALOG_H
#define NEWRECORDDIALOG_H

#include <QDialog>

#include "StorageManager.h"

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
    //void rec_onPB_okClicked();
    void rec_onPB_cancelClicked();

private:
    Ui::NewRecordDialog *m_ui;
    mdt::work::StorageManager m_storageWork;

    void init();
};

#endif // NEWRECORDDIALOG_H
