#ifndef NEWITEMWINDOW_H
#define NEWITEMWINDOW_H

#include <QDialog>

#include "ItemData.h"


namespace Ui {
class NewItemWindow;
}

class NewItemWindow : public QDialog
{
    Q_OBJECT

public:
    explicit NewItemWindow(QWidget *parent = nullptr);
    ~NewItemWindow();

signals:
    void sig_handleNewItem(const ItemData& newItem);

private slots:
    void rec_onPB_okClicked();
    void rec_onPB_cancelClicked();

private:
    Ui::NewItemWindow *ui;

    void init();
};

#endif // NEWITEMWINDOW_H
