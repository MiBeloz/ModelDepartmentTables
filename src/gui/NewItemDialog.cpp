#include "NewItemDialog.h"

#include "ui_NewItemDialog.h"

NewItemDialog::NewItemDialog(QWidget *parent) : QDialog(parent), m_ui(new Ui::NewItemDialog) {
    m_ui->setupUi(this);
    init();
}

NewItemDialog::~NewItemDialog() {
    delete m_ui;
}

// void NewItemDialog::rec_onPB_okClicked() {
//     Drawing *drawing = new Drawing(ui->LE_drawingNumber->text(), ui->LE_title->text());

//     Item item;
//     item.date = Item::strToDate(ui->DE_date->text());
//     item.drawing = drawing;
//     item.amount = ui->SPB_amount->value();

//     ItemData newItem;
//     newItem.item = item;
//     newItem.executors << ui->LE_executor->text();
//     newItem.authors << ui->LE_author->text();
//     newItem.castingMaterials << ui->LE_castingMaterial->text();
//     newItem.modelMaterials << ui->LE_modelMaterial->text();
//     newItem.machines << ui->LE_machine->text();
//     newItem.notes << ui->LE_note->text();

//     emit sig_handleNewItem(newItem);

//     accept();
// }

void NewItemDialog::rec_onPB_cancelClicked() {
    reject();
}

void NewItemDialog::init() {
    //connect(ui->PB_ok, &QPushButton::clicked, this, &NewItemWindow::rec_onPB_okClicked);
    connect(m_ui->PB_cancel, &QPushButton::clicked, this, &NewItemDialog::rec_onPB_cancelClicked);

    m_ui->DE_date->setDate(QDate::currentDate());
    //ui->DE_date->setDisplayFormat(Item::dateFormat);
    m_ui->DE_date->setCalendarPopup(true);
}
