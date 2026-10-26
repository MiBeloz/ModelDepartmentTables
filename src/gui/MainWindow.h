#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAbstractProxyModel>
#include <QMainWindow>
#include <QPointer>

#include "AboutDialog.h"
#include "DatabaseManager.h"
#include "NewItemWindow.h"
#include "SettingsWindow.h"

//class AboutDialog;

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
    //void rec_handleNewItem(const ItemData &newItem);

    void on_actionAbout_triggered();

private:
    Ui::MainWindow *ui;
    QPointer<AboutDialog> m_aboutDlg;
    NewItemWindow *m_newItemWindow;
    SettingsWindow *m_settingsWindow;
    QAbstractProxyModel *m_itemsModel;

    DatabaseManager *m_dbManager;

    void init();
};
#endif // MAINWINDOW_H
