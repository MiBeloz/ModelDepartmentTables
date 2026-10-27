#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QAbstractProxyModel>
#include <QMainWindow>
#include <QPointer>

#include "AboutDialog.h"
#include "NewItemDialog.h"
#include "SettingsDialog.h"

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

    void on_PB_addItem_clicked();
    void on_actionSettings_triggered();
    void on_actionAbout_triggered();

private:
    Ui::MainWindow *m_ui;
    QPointer<AboutDialog> m_aboutDialog;
    QPointer<NewItemDialog> m_newItemDialog;
    QPointer<SettingsDialog> m_settingsDialog;
    QPointer<QAbstractProxyModel> m_itemsModel;

    void init();
};
#endif // MAINWINDOW_H
