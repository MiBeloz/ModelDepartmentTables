#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QAbstractProxyModel>

#include "NewItemWindow.h"
#include "StorageCache.hpp"


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void rec_onPB_addItemClicked();
    void rec_handleNewItem(const ItemData& newItem);

private:
    Ui::MainWindow *ui;
    NewItemWindow *m_newItemWindow;
    QAbstractProxyModel *m_itemsModel;

    StorageCache cache;

    void init();
};
#endif // MAINWINDOW_H
