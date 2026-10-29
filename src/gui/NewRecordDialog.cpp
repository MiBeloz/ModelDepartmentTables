#include "NewRecordDialog.h"

#include "Constants.h"
#include "DatesList.h"
#include "SaverFile.h"
#include "Utils.h"
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
    connect(m_ui->comboBoxDrawingNumberValue,
            &QComboBox::currentTextChanged,
            this,
            [&](const QString &text) {
                auto drawings = m_storageWork.get().drawings();
                for (const auto &drawing : std::as_const(drawings)) {
                    if (drawing.getNumber() == text) {
                        m_ui->labelDrawingTitleValue->setText(drawing.getTitle());
                        return;
                    }
                }
                m_ui->labelDrawingTitleValue->setText(QString());
            });

    m_ui->dateEditDateValue->setDate(QDate::currentDate());
    m_ui->dateEditDateValue->setDisplayFormat(mdt::work::DatesList::getDateFormat());
    m_ui->dateEditDateValue->setCalendarPopup(true);

    mdt::SaverFile saver(mdt::SaverSettings::SAVER_STORAGE_FILENAME,
                         mdt::SaverSettings::SAVER_STORAGE_FILENAME_TMP);
    if (!saver.read(m_storageWork)) {
        throw mdt::RuntimeError(u"Error read storage"_s);
    }

    auto drawings = m_storageWork.get().drawings();
    QStringList numbers;
    for (const auto &drawing : std::as_const(drawings)) {
        numbers.append(drawing.getNumber());
    }
    std::sort(numbers.begin(), numbers.end(), naturalLess);

    m_sourceModelDrawings = new QStandardItemModel(this);
    for (const auto &number : numbers) {
        m_sourceModelDrawings->appendRow(new QStandardItem(number));
    }
    m_ui->comboBoxDrawingNumberValue->setSourceModel(m_sourceModelDrawings);

    m_ui->comboBoxDrawingNumberValue->addItems(numbers);
    m_ui->comboBoxDrawingNumberValue->setCurrentIndex(-1);

    auto executors = m_storageWork.get().executors();
    std::sort(executors.begin(), executors.end(), naturalLess);
    m_ui->comboBoxAddExecutor->addItems(executors);

    auto authors = m_storageWork.get().authors();
    std::sort(authors.begin(), authors.end(), naturalLess);
    m_ui->comboBoxAddAuthor->addItems(authors);

    auto castingMaterials = m_storageWork.get().castingMaterials();
    std::sort(castingMaterials.begin(), castingMaterials.end(), naturalLess);
    m_ui->comboBoxAddCastingMaterial->addItems(castingMaterials);

    auto modelMaterials = m_storageWork.get().modelMaterials();
    std::sort(modelMaterials.begin(), modelMaterials.end(), naturalLess);
    m_ui->comboBoxAddModelMaterial->addItems(modelMaterials);

    auto machines = m_storageWork.get().machines();
    std::sort(machines.begin(), machines.end(), naturalLess);
    m_ui->comboBoxAddMachine->addItems(machines);

    auto notes = m_storageWork.get().notes();
    std::sort(notes.begin(), notes.end(), naturalLess);
    m_ui->comboBoxAddNote->addItems(notes);
}
