#ifndef NEWITEMDIALOG_H
#define NEWITEMDIALOG_H

#include <QDialog>

namespace Ui {
    class NewItemDialog;
}

class NewItemDialog final : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(NewItemDialog)

public:
    explicit NewItemDialog(QWidget *parent = nullptr);
    ~NewItemDialog();

signals:
    //void sig_handleNewItem(const ItemData &newItem);

private slots:
    //void rec_onPB_okClicked();
    void rec_onPB_cancelClicked();

private:
    Ui::NewItemDialog *m_ui;

    void init();
};

#endif // NEWITEMDIALOG_H
