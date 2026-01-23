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



    auto dateCache = cache.getTypedCache<uint>(0);
    dateCache->insert(Item::strToDate(newItem.at(0)));

    auto drawingCache = cache.getTypedCache<Drawing>(1);
    drawingCache->insert(newItem.item.drawing);

    auto executorCache = cache.getTypedCache<QString>(2);
    executorCache->insert(newItem.at(3));

    auto authorCache = cache.getTypedCache<QString>(3);
    authorCache->insert(newItem.at(4));

    auto amountCache = cache.getTypedCache<uint>(4);
    amountCache->insert(newItem.at(5).toUInt());

    auto castingCache = cache.getTypedCache<QString>(5);
    castingCache->insert(newItem.at(6));

    auto materialCache = cache.getTypedCache<QString>(6);
    materialCache->insert(newItem.at(7));

    auto machineCache = cache.getTypedCache<QString>(7);
    machineCache->insert(newItem.at(8));

    auto noteCache = cache.getTypedCache<QString>(8);
    noteCache->insert(newItem.at(9));

    auto itemCache = cache.getTypedCache<Item>(9);
    itemCache->insert(newItem.item);


    qsizetype i = itemCache->getIdsByData(newItem.item).first();
    qsizetype j = executorCache->getIdsByData(newItem.executors.first()).first();
    auto itemExecutor = cache.getTypedCache<uint>(10);
    itemExecutor->insert(i, j);

    auto itemAuthor = cache.getTypedCache<uint>(11);
    itemAuthor->insert(itemCache->getIdsByData(newItem.item).first(), amountCache->getIdsByData(newItem.item.amount).first());

    auto itemCastingMaterial = cache.getTypedCache<uint>(12);
    itemCastingMaterial->insert(itemCache->getIdsByData(newItem.item).first(), castingCache->getIdsByData(newItem.castingMaterials.first()).first());

    auto itemModelMaterial = cache.getTypedCache<uint>(13);
    itemModelMaterial->insert(itemCache->getIdsByData(newItem.item).first(), materialCache->getIdsByData(newItem.modelMaterials.first()).first());

    auto itemMachine = cache.getTypedCache<uint>(14);
    itemMachine->insert(itemCache->getIdsByData(newItem.item).first(), machineCache->getIdsByData(newItem.machines.first()).first());

    auto itemNote = cache.getTypedCache<uint>(15);
    itemNote->insert(itemCache->getIdsByData(newItem.item).first(), noteCache->getIdsByData(newItem.notes.first()).first());



    dateCache->writeToFile(           "workDates.txt");
    drawingCache->writeToFile(            "drawings.txt");
    executorCache->writeToFile(           "executors.txt");
    amountCache->writeToFile(             "amounts.txt");
    castingCache->writeToFile(    "castingMaterials.txt");
    materialCache->writeToFile(      "modelMaterials.txt");
    machineCache->writeToFile(            "machines.txt");
    noteCache->writeToFile(               "notes.txt");
    itemCache->writeToFile(               "items.txt");
    itemExecutor->writeToFile(       "itemExecutors.txt");
    itemCastingMaterial->writeToFile("itemCastingMaterials.txt");
    itemModelMaterial->writeToFile(  "itemModelMaterials.txt");
    itemMachine->writeToFile(        "itemMachines.txt");
    itemNote->writeToFile(           "itemNotes.txt");


    dateCache->clear();
    drawingCache->clear();
    executorCache->clear();
    amountCache->clear();
    castingCache->clear();
    materialCache->clear();
    machineCache->clear();
    noteCache->clear();
    itemCache->clear();
    itemExecutor->clear();
    itemCastingMaterial->clear();
    itemModelMaterial->clear();
    itemMachine->clear();
    itemNote->clear();


    dateCache->readFromFile(           "workDates.txt");
    drawingCache->readFromFile(            "drawings.txt");
    executorCache->readFromFile(           "executors.txt");
    amountCache->readFromFile(             "amounts.txt");
    castingCache->readFromFile(    "castingMaterials.txt");
    materialCache->readFromFile(      "modelMaterials.txt");
    machineCache->readFromFile(            "machines.txt");
    noteCache->readFromFile(               "notes.txt");
    itemCache->readFromFile(               "items.txt");
    itemExecutor->readFromFile(       "itemExecutors.txt");
    itemCastingMaterial->readFromFile("itemCastingMaterials.txt");
    itemModelMaterial->readFromFile(  "itemModelMaterials.txt");
    itemMachine->readFromFile(        "itemMachines.txt");
    itemNote->readFromFile(           "itemNotes.txt");



    model->insertRow(0);

    QModelIndex newIndex0 = model->index(0, 0);
    model->setData(newIndex0, Item::dateToStr(dateCache->get(0).first()));

    QModelIndex newIndex1 = model->index(0, 1);
    model->setData(newIndex1, drawingCache->get(0).first().number);

    QModelIndex newIndex2 = model->index(0, 2);
    model->setData(newIndex2, drawingCache->get(0).first().title);

    QModelIndex newIndex3 = model->index(0, 3);
    model->setData(newIndex3, executorCache->get(0).first());

    QModelIndex newIndex4 = model->index(0, 4);
    model->setData(newIndex4, "!!!!!");

    QModelIndex newIndex5 = model->index(0, 5);
    model->setData(newIndex5, amountCache->get(0).first());

    QModelIndex newIndex6 = model->index(0, 6);
    model->setData(newIndex6, castingCache->get(0).first());

    QModelIndex newIndex7 = model->index(0, 7);
    model->setData(newIndex7, materialCache->get(0).first());

    QModelIndex newIndex8 = model->index(0, 8);
    model->setData(newIndex8, machineCache->get(0).first());

    QModelIndex newIndex9 = model->index(0, 9);
    model->setData(newIndex9, noteCache->get(0).first());

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

