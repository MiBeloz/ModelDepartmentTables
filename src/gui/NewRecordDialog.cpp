#include "NewRecordDialog.h"

#include "DatesList.h"
#include "Record.h"
#include "ui_NewRecordDialog.h"

NewRecordDialog::NewRecordDialog(QWidget *parent) : QDialog(parent), m_ui(new Ui::NewRecordDialog) {
    m_ui->setupUi(this);
    init();
}

NewRecordDialog::~NewRecordDialog() {
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

void NewRecordDialog::rec_onPB_cancelClicked() {
    reject();
}

void NewRecordDialog::init() {
    //connect(ui->PB_ok, &QPushButton::clicked, this, &NewItemWindow::rec_onPB_okClicked);
    connect(m_ui->PB_cancel, &QPushButton::clicked, this, &NewRecordDialog::rec_onPB_cancelClicked);

    m_ui->DE_date->setDate(QDate::currentDate());
    m_ui->DE_date->setDisplayFormat(mdt::work::DatesList::getDateFormat());
    m_ui->DE_date->setCalendarPopup(true);
}
