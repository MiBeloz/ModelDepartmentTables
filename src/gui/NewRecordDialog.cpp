#include "NewRecordDialog.h"

#include "Constants.h"
#include "DatesList.h"
#include "SaverFile.h"
#include "ui_NewRecordDialog.h"

NewRecordDialog::NewRecordDialog(QWidget *parent) : QDialog(parent), m_ui(new Ui::NewRecordDialog) {
    m_ui->setupUi(this);
    init();
}

NewRecordDialog::~NewRecordDialog() {
    delete m_ui;
}

void NewRecordDialog::onPushButtonOkClicked() {
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
}

void NewRecordDialog::onPushButtonCancelClicked() {
    reject();
}

void NewRecordDialog::init() {
    connect(m_ui->pushButtonOk,
            &QPushButton::clicked,
            this,
            &NewRecordDialog::onPushButtonOkClicked);
    connect(m_ui->pushButtonCancel,
            &QPushButton::clicked,
            this,
            &NewRecordDialog::onPushButtonCancelClicked);

    connect(m_ui->comboBoxDrawingNumberValue,
            &QComboBox::currentTextChanged,
            this,
            [this](const QString &text) {
                auto drawings = m_storageWork.get().drawings();
                for (const auto &drawing : std::as_const(drawings)) {
                    if (drawing.getNumber() == text) {
                        m_ui->lineEditDrawingTitleValue->setText(drawing.getTitle());
                        m_ui->lineEditDrawingTitleValue->setEnabled(false);
                        return;
                    }
                }
                m_ui->lineEditDrawingTitleValue->setText(QString());
                m_ui->lineEditDrawingTitleValue->setEnabled(true);
            });
    createListComboPair(m_ui->listWidgetExecutors, m_ui->comboBoxAddExecutor);
    createListComboPair(m_ui->listWidgetAuthors, m_ui->comboBoxAddAuthor);
    createListComboPair(m_ui->listWidgetCastingMaterials, m_ui->comboBoxAddCastingMaterial);
    createListComboPair(m_ui->listWidgetModelMaterials, m_ui->comboBoxAddModelMaterial);
    createListComboPair(m_ui->listWidgetMachines, m_ui->comboBoxAddMachine);
    createListComboPair(m_ui->listWidgetNotes, m_ui->comboBoxAddNote);

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
    //std::sort(numbers.begin(), numbers.end(), naturalLess);

    m_ui->comboBoxDrawingNumberValue->addItems(numbers);
    m_ui->comboBoxDrawingNumberValue->setCurrentIndex(-1);

    m_ui->comboBoxAddExecutor->setStringList(m_storageWork.get().executors());
    m_ui->comboBoxAddExecutor->setPlaceholderText(u"Select executor..."_s);

    m_ui->comboBoxAddAuthor->setStringList(m_storageWork.get().authors());
    m_ui->comboBoxAddAuthor->setPlaceholderText(u"Select author..."_s);

    m_ui->comboBoxAddCastingMaterial->setStringList(m_storageWork.get().castingMaterials());
    m_ui->comboBoxAddCastingMaterial->setPlaceholderText(u"Select casting material..."_s);

    m_ui->comboBoxAddModelMaterial->setStringList(m_storageWork.get().modelMaterials());
    m_ui->comboBoxAddModelMaterial->setPlaceholderText(u"Select model material..."_s);

    m_ui->comboBoxAddMachine->setStringList(m_storageWork.get().machines());
    m_ui->comboBoxAddMachine->setPlaceholderText(u"Select machine..."_s);

    m_ui->comboBoxAddNote->setStringList(m_storageWork.get().notes());
    m_ui->comboBoxAddNote->setPlaceholderText(u"Select note..."_s);

    m_ui->listWidgetExecutors->setFlow(QListView::LeftToRight);
    m_ui->listWidgetExecutors->setWrapping(true);
    m_ui->listWidgetExecutors->setResizeMode(QListView::Adjust);
    m_ui->listWidgetExecutors->setSpacing(10);

    //m_ui->listWidgetExecutors->installEventFilter(this);
    m_ui->listWidgetExecutors->setSelectionMode(QAbstractItemView::SingleSelection);
}
