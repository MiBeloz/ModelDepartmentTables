#ifndef NEWRECORDDIALOG_H
#define NEWRECORDDIALOG_H

#include "NaturalSortComboBox.h"
#include "StorageManager.h"

#include <QDialog>
#include <QListWidget>

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
    void onPushButtonOkClicked();
    void onPushButtonCancelClicked();

private:
    Ui::NewRecordDialog *m_ui;
    mdt::work::StorageManager *m_storageWork;

    void init();
    void createListComboPair(QListWidget *list, NaturalSortComboBox *combo);
    void setListDefaultSettings(QListWidget *list);
};

#endif // NEWRECORDDIALOG_H
