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
    m_settingsWindow = new SettingsWindow(this);
    m_dbManager = new DatabaseManager(this);
    m_cacheManager = new CacheManager(this);

    init();
}

MainWindow::~MainWindow() {
    delete ui;
}

void MainWindow::rec_handleNewItem(const ItemData &newItem) {
    auto model = m_itemsModel->sourceModel();
    model->insertRow(0);
    for (int col = 0; col < model->columnCount(); ++col) {
        QModelIndex newIndex = model->index(0, col);
        model->setData(newIndex, newItem.at(col));
    }
    ui->TV_items->resizeColumnsToContents();



    auto dateCache = m_cacheManager->getTypedCache<size_t>(CacheManager::Column::cDate);
    auto id_date = dateCache->insert(newItem.item.date);

    auto drawingCache = m_cacheManager->getTypedCache<Drawing>(CacheManager::Column::cDrawing);
    auto id_drawing = drawingCache->insert(newItem.item.drawing);

    auto executorCache = m_cacheManager->getTypedCache<QString>(CacheManager::Column::cExecutor);
    QList<size_t> id_executors;
    for (size_t i = 0; i < newItem.executors.size(); ++i) {
        id_executors.append(executorCache->insert(newItem.executors[i]));
    }

    auto authorCache = m_cacheManager->getTypedCache<QString>(CacheManager::Column::cAuthor);
    QList<size_t> id_authors;
    for (size_t i = 0; i < newItem.authors.size(); ++i) {
        id_authors.append(authorCache->insert(newItem.authors[i]));
    }

    auto amountCache = m_cacheManager->getTypedCache<size_t>(CacheManager::Column::cAmount);
    auto id_amount = amountCache->insert(newItem.item.amount);

    auto castingMaterialCache = m_cacheManager->getTypedCache<QString>(CacheManager::Column::cCastingMaterial);
    QList<size_t> id_castingMaterials;
    for (size_t i = 0; i < newItem.castingMaterials.size(); ++i) {
        id_castingMaterials.append(castingMaterialCache->insert(newItem.castingMaterials[i]));
    }

    auto modelMaterialCache = m_cacheManager->getTypedCache<QString>(CacheManager::Column::cModelMaterial);
    QList<size_t> id_modelMaterials;
    for (size_t i = 0; i < newItem.modelMaterials.size(); ++i) {
        id_modelMaterials.append(modelMaterialCache->insert(newItem.modelMaterials[i]));
    }

    auto machineCache = m_cacheManager->getTypedCache<QString>(CacheManager::Column::cMachine);
    QList<size_t> id_machines;
    for (size_t i = 0; i < newItem.machines.size(); ++i) {
        id_machines.append(machineCache->insert(newItem.machines[i]));
    }

    auto noteCache = m_cacheManager->getTypedCache<QString>(CacheManager::Column::cNote);
    QList<size_t> id_notes;
    for (size_t i = 0; i < newItem.notes.size(); ++i) {
        id_notes.append(noteCache->insert(newItem.notes[i]));
    }

    auto itemCache = m_cacheManager->getTypedCache<ItemID>(CacheManager::Column::cItemFromIDs);
    auto id_item = itemCache->insert(ItemID{id_date, id_drawing, id_amount});



    auto itemExecutor = m_cacheManager->getTypedCache<size_t>(CacheManager::Column::cItemExecutor);
    for (size_t i = 0; i < id_executors.size(); ++i) {
        itemExecutor->insert(id_item, id_executors[i]);
    }

    auto itemAuthor = m_cacheManager->getTypedCache<size_t>(CacheManager::Column::cItemAuthor);
    for (size_t i = 0; i < id_authors.size(); ++i) {
        itemAuthor->insert(id_item, id_authors[i]);
    }

    auto itemCastingMaterial = m_cacheManager->getTypedCache<size_t>(CacheManager::Column::cItemCastingMaterial);
    for (size_t i = 0; i < id_castingMaterials.size(); ++i) {
        itemCastingMaterial->insert(id_item, id_castingMaterials[i]);
    }

    auto itemModelMaterial = m_cacheManager->getTypedCache<size_t>(CacheManager::Column::cItemModelMaterial);
    for (size_t i = 0; i < id_modelMaterials.size(); ++i) {
        itemModelMaterial->insert(id_item, id_modelMaterials[i]);
    }

    auto itemMachine = m_cacheManager->getTypedCache<size_t>(CacheManager::Column::cItemMachine);
    for (size_t i = 0; i < id_machines.size(); ++i) {
        itemMachine->insert(id_item, id_machines[i]);
    }

    auto itemNote = m_cacheManager->getTypedCache<size_t>(CacheManager::Column::cItemNote);
    for (size_t i = 0; i < id_notes.size(); ++i) {
        itemNote->insert(id_item, id_notes[i]);
    }

    dateCache->writeToFile(           "workDates.txt");
    drawingCache->writeToFile(        "drawings.txt");
    executorCache->writeToFile(       "executors.txt");
    amountCache->writeToFile(         "amounts.txt");
    castingMaterialCache->writeToFile("castingMaterials.txt");
    modelMaterialCache->writeToFile(  "modelMaterials.txt");
    machineCache->writeToFile(        "machines.txt");
    noteCache->writeToFile(           "notes.txt");
    itemCache->writeToFile(           "items.txt");
    itemExecutor->writeToFile(        "itemExecutors.txt");
    itemCastingMaterial->writeToFile( "itemCastingMaterials.txt");
    itemModelMaterial->writeToFile(   "itemModelMaterials.txt");
    itemMachine->writeToFile(         "itemMachines.txt");
    itemNote->writeToFile(            "itemNotes.txt");


    // dateCache->clear();
    // drawingCache->clear();
    // executorCache->clear();
    // amountCache->clear();
    // castingCache->clear();
    // materialCache->clear();
    // machineCache->clear();
    // noteCache->clear();
    // itemCache->clear();
    // itemExecutor->clear();
    // itemCastingMaterial->clear();
    // itemModelMaterial->clear();
    // itemMachine->clear();
    // itemNote->clear();


    // dateCache->readFromFile(           "workDates.txt");
    // drawingCache->readFromFile(            "drawings.txt");
    // executorCache->readFromFile(           "executors.txt");
    // amountCache->readFromFile(             "amounts.txt");
    // castingCache->readFromFile(    "castingMaterials.txt");
    // materialCache->readFromFile(      "modelMaterials.txt");
    // machineCache->readFromFile(            "machines.txt");
    // noteCache->readFromFile(               "notes.txt");
    // itemCache->readFromFile(               "items.txt");
    // itemExecutor->readFromFile(       "itemExecutors.txt");
    // itemCastingMaterial->readFromFile("itemCastingMaterials.txt");
    // itemModelMaterial->readFromFile(  "itemModelMaterials.txt");
    // itemMachine->readFromFile(        "itemMachines.txt");
    // itemNote->readFromFile(           "itemNotes.txt");



    // model->insertRow(0);

    // QModelIndex newIndex0 = model->index(0, 0);
    // model->setData(newIndex0, Item::dateToStr(dateCache->get(0).first()));

    // QModelIndex newIndex1 = model->index(0, 1);
    // model->setData(newIndex1, drawingCache->get(0).first().number);

    // QModelIndex newIndex2 = model->index(0, 2);
    // model->setData(newIndex2, drawingCache->get(0).first().title);

    // QModelIndex newIndex3 = model->index(0, 3);
    // model->setData(newIndex3, executorCache->get(0).first());

    // QModelIndex newIndex4 = model->index(0, 4);
    // model->setData(newIndex4, "!!!!!");

    // QModelIndex newIndex5 = model->index(0, 5);
    // model->setData(newIndex5, amountCache->get(0).first());

    // QModelIndex newIndex6 = model->index(0, 6);
    // model->setData(newIndex6, castingCache->get(0).first());

    // QModelIndex newIndex7 = model->index(0, 7);
    // model->setData(newIndex7, materialCache->get(0).first());

    // QModelIndex newIndex8 = model->index(0, 8);
    // model->setData(newIndex8, machineCache->get(0).first());

    // QModelIndex newIndex9 = model->index(0, 9);
    // model->setData(newIndex9, noteCache->get(0).first());

    ui->TV_items->resizeColumnsToContents();
}

void MainWindow::init() {
    connect(ui->actionSettings, &QAction::triggered, this, [&]() { m_settingsWindow->open(); });
    connect(ui->PB_addItem, &QPushButton::clicked, this, [&]() { m_newItemWindow->open(); });
    connect(m_newItemWindow, &NewItemWindow::sig_handleNewItem, this, &MainWindow::rec_handleNewItem);

    ui->TV_items->setModel(m_itemsModel);
    ui->TV_items->verticalHeader()->hide();
    ui->TV_items->setSortingEnabled(true);
    ui->TV_items->resizeColumnsToContents();
    ui->TV_items->sortByColumn(0, Qt::SortOrder::AscendingOrder);







    auto dateCache = m_cacheManager->getTypedCache<size_t>(CacheManager::Column::cDate);
    auto drawingCache = m_cacheManager->getTypedCache<Drawing>(CacheManager::Column::cDrawing);
    auto executorCache = m_cacheManager->getTypedCache<QString>(CacheManager::Column::cExecutor);
    auto authorCache = m_cacheManager->getTypedCache<QString>(CacheManager::Column::cAuthor);
    auto amountCache = m_cacheManager->getTypedCache<size_t>(CacheManager::Column::cAmount);
    auto castingMaterialCache = m_cacheManager->getTypedCache<QString>(CacheManager::Column::cCastingMaterial);
    auto modelMaterialCache = m_cacheManager->getTypedCache<QString>(CacheManager::Column::cModelMaterial);
    auto machineCache = m_cacheManager->getTypedCache<QString>(CacheManager::Column::cMachine);
    auto noteCache = m_cacheManager->getTypedCache<QString>(CacheManager::Column::cNote);
    auto itemCache = m_cacheManager->getTypedCache<ItemID>(CacheManager::Column::cItemFromIDs);
    auto itemExecutor = m_cacheManager->getTypedCache<size_t>(CacheManager::Column::cItemExecutor);
    auto itemAuthor = m_cacheManager->getTypedCache<size_t>(CacheManager::Column::cItemAuthor);
    auto itemCastingMaterial = m_cacheManager->getTypedCache<size_t>(CacheManager::Column::cItemCastingMaterial);
    auto itemModelMaterial = m_cacheManager->getTypedCache<size_t>(CacheManager::Column::cItemModelMaterial);
    auto itemMachine = m_cacheManager->getTypedCache<size_t>(CacheManager::Column::cItemMachine);
    auto itemNote = m_cacheManager->getTypedCache<size_t>(CacheManager::Column::cItemNote);

    dateCache->readFromFile(           "workDates.txt");
    drawingCache->readFromFile(        "drawings.txt");
    executorCache->readFromFile(       "executors.txt");
    amountCache->readFromFile(         "amounts.txt");
    castingMaterialCache->readFromFile("castingMaterials.txt");
    modelMaterialCache->readFromFile(  "modelMaterials.txt");
    machineCache->readFromFile(        "machines.txt");
    noteCache->readFromFile(           "notes.txt");
    itemCache->readFromFile(           "items.txt");
    itemExecutor->readFromFile(        "itemExecutors.txt");
    itemCastingMaterial->readFromFile( "itemCastingMaterials.txt");
    itemModelMaterial->readFromFile(   "itemModelMaterials.txt");
    itemMachine->readFromFile(         "itemMachines.txt");
    itemNote->readFromFile(            "itemNotes.txt");

    auto model = m_itemsModel->sourceModel();

    QList<Item> items;
    for (size_t i = 0; i < itemCache->size(); ++i) {
        ItemID itemID = itemCache->get(i).first();
    }

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
    model->setData(newIndex6, castingMaterialCache->get(0).first());

    QModelIndex newIndex7 = model->index(0, 7);
    model->setData(newIndex7, modelMaterialCache->get(0).first());

    QModelIndex newIndex8 = model->index(0, 8);
    model->setData(newIndex8, machineCache->get(0).first());

    QModelIndex newIndex9 = model->index(0, 9);
    model->setData(newIndex9, noteCache->get(0).first());

    ui->TV_items->resizeColumnsToContents();
}

