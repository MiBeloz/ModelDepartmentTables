#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "ItemsModel.h"


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_itemsModel = new ItemsModel(this);
    m_newItemWindow = new NewItemWindow(this);
    init();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::rec_onPB_addItemClicked() {
    m_newItemWindow->open();
}

void MainWindow::rec_handleNewItem(const ItemData &newItem) {
    auto model = m_itemsModel->sourceModel();
    model->insertRow(0);
    for (int col = 0; col < model->columnCount(); ++col) {
        QModelIndex newIndex = model->index(0, col);
        model->setData(newIndex, newItem.at(col));
    }
    ui->TV_items->resizeColumnsToContents();
}

void MainWindow::init() {
    connect(ui->PB_addItem, &QPushButton::clicked, this, &MainWindow::rec_onPB_addItemClicked);
    connect(m_newItemWindow, &NewItemWindow::sig_handleNewItem, this, &MainWindow::rec_handleNewItem);

    ui->TV_items->setModel(m_itemsModel);
    ui->TV_items->verticalHeader()->hide();
    ui->TV_items->setSortingEnabled(true);
    ui->TV_items->resizeColumnsToContents();
    ui->TV_items->sortByColumn(0, Qt::SortOrder::AscendingOrder);
}

