#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAbstractProxyModel>
#include <QMainWindow>

#include "CacheManager.hpp"
#include "DatabaseManager.h"
#include "NewItemWindow.h"
#include "SettingsWindow.h"

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
    void rec_handleNewItem(const ItemData &newItem);

private:
    Ui::MainWindow *ui;
    NewItemWindow *m_newItemWindow;
    SettingsWindow *m_settingsWindow;
    QAbstractProxyModel *m_itemsModel;

    DatabaseManager *m_dbManager;
    CacheManager *m_cacheManager;

    void init();
};
#endif // MAINWINDOW_H
