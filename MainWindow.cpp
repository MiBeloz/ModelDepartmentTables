#include "MainWindow.h"
#include "ui_mainwindow.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_itemsModel = new ItemsModel;

    ui->TV_items->setModel(m_itemsModel);
    ui->TV_items->verticalHeader()->hide();
    ui->TV_items->setSortingEnabled(true);

    ui->TV_items->resizeColumnsToContents();
    ui->TV_items->sortByColumn(0, Qt::SortOrder::AscendingOrder);
}

MainWindow::~MainWindow() {
    delete ui;
    delete m_itemsModel;
}
