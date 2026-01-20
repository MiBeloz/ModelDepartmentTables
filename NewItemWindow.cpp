#include "NewItemWindow.h"
#include "ui_NewItemWindow.h"


NewItemWindow::NewItemWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::NewItemWindow)
{
    ui->setupUi(this);
    init();
}

NewItemWindow::~NewItemWindow() {
    delete ui;
}

void NewItemWindow::rec_onPB_okClicked() {
    Drawing drawing;
    drawing.number = ui->LE_drawingNumber->text();
    drawing.title = ui->LE_title->text();

    Item item;
    item.date = strToDate(ui->DE_date->text());
    item.drawing = drawing;
    item.amount = ui->SPB_amount->value();

    ItemData newItem;
    newItem.item = item;
    newItem.executors << ui->LE_executor->text();
    newItem.authors << ui->LE_author->text();
    newItem.castingMaterials << ui->LE_castingMaterial->text();
    newItem.modelMaterials << ui->LE_modelMaterial->text();
    newItem.machines << ui->LE_machine->text();
    newItem.notes << ui->LE_note->text();

    emit sig_handleNewItem(newItem);

    accept();
}

void NewItemWindow::rec_onPB_cancelClicked() {
    reject();
}

void NewItemWindow::init() {
    connect(ui->PB_ok, &QPushButton::clicked, this, &NewItemWindow::rec_onPB_okClicked);
    connect(ui->PB_cancel, &QPushButton::clicked, this, &NewItemWindow::rec_onPB_cancelClicked);
}
