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



    auto dateCache = cache.getTypedCache<uint, uint>(0);
    dateCache->insert(dateCache->size(), Item::strToDate(newItem.at(0)));

    auto drawingCache = cache.getTypedCache<uint, Drawing>(1);
    drawingCache->insert(drawingCache->size(), newItem.item.drawing);

    auto executorCache = cache.getTypedCache<uint, QString>(2);
    executorCache->insert(executorCache->size(), newItem.at(3));

    auto authorCache = cache.getTypedCache<uint, QString>(3);
    authorCache->insert(authorCache->size(), newItem.at(4));

    auto amountCache = cache.getTypedCache<uint, uint>(4);
    amountCache->insert(amountCache->size(), newItem.at(5).toUInt());

    auto castingCache = cache.getTypedCache<uint, QString>(5);
    castingCache->insert(castingCache->size(), newItem.at(6));

    auto materialCache = cache.getTypedCache<uint, QString>(6);
    materialCache->insert(materialCache->size(), newItem.at(7));

    auto machineCache = cache.getTypedCache<uint, QString>(7);
    machineCache->insert(machineCache->size(), newItem.at(8));

    auto noteCache = cache.getTypedCache<uint, QString>(8);
    noteCache->insert(noteCache->size(), newItem.at(9));

    auto itemCache = cache.getTypedCache<uint, Item>(9);
    itemCache->insert(itemCache->size(), newItem.item);


    auto itemExecutor = cache.getTypedCache<uint, QString>(10);
    itemExecutor->insert(0, 0);

    auto itemAuthor = cache.getTypedCache<uint, QString>(11);
    itemAuthor->insert(0, 0);

    auto itemCastingMaterial = cache.getTypedCache<uint, QString>(12);
    itemCastingMaterial->insert(0, 0);

    auto itemModelMaterial = cache.getTypedCache<uint, QString>(13);
    itemModelMaterial->insert(0, 0);

    auto itemMachine = cache.getTypedCache<uint, QString>(14);
    itemMachine->insert(0, 0);

    auto itemNote = cache.getTypedCache<uint, QString>(15);
    itemNote->insert(0, 0);
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

