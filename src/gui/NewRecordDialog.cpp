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

    connect(m_ui->checkBoxAddToFinishedModels, &QCheckBox::clicked, this, [this](bool checked) {
        m_ui->dateEditDateValue->setEnabled(checked);
    });

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

    if (m_ui->checkBoxAddToFinishedModels->isChecked()) {
        m_ui->dateEditDateValue->setEnabled(true);
    } else {
        m_ui->dateEditDateValue->setEnabled(false);
    }

    createListComboPair(m_ui->listWidgetExecutors, m_ui->comboBoxAddExecutor);
    createListComboPair(m_ui->listWidgetAuthors, m_ui->comboBoxAddAuthor);
    createListComboPair(m_ui->listWidgetCastingMaterials, m_ui->comboBoxAddCastingMaterial);
    createListComboPair(m_ui->listWidgetModelMaterials, m_ui->comboBoxAddModelMaterial);
    createListComboPair(m_ui->listWidgetMachines, m_ui->comboBoxAddMachine);
    createListComboPair(m_ui->listWidgetNotes, m_ui->comboBoxAddNote);

    setListDefaultSettings(m_ui->listWidgetExecutors);
    setListDefaultSettings(m_ui->listWidgetAuthors);
    setListDefaultSettings(m_ui->listWidgetCastingMaterials);
    setListDefaultSettings(m_ui->listWidgetModelMaterials);
    setListDefaultSettings(m_ui->listWidgetMachines);
    setListDefaultSettings(m_ui->listWidgetNotes);

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

    m_ui->comboBoxDrawingNumberValue->setStringList(numbers);
    m_ui->comboBoxDrawingNumberValue->setPlaceholderText(u"Select or start typing the drawing..."_s);

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
}

void NewRecordDialog::createListComboPair(QListWidget *list, NaturalSortComboBox *combo) {
    connect(list, &QListWidget::itemDoubleClicked, this, [list, combo](QListWidgetItem *item) {
        if (!item) {
            return;
        }
        const QString text = item->text();
        delete list->takeItem(list->row(item));
        combo->addString(text);
    });

    connect(combo, qOverload<int>(&QComboBox::activated), this, [list, combo](int index) {
        if (index < 0) {
            return;
        }
        const QString text = combo->itemText(index);
        list->addItem(text);
        combo->removeItem(index);
        combo->setCurrentIndex(-1);
    });
}

void NewRecordDialog::setListDefaultSettings(QListWidget *list) {
    list->setFlow(QListView::LeftToRight);
    list->setWrapping(true);
    list->setResizeMode(QListView::Adjust);
    list->setSpacing(10);
    list->setSelectionMode(QAbstractItemView::SingleSelection);
}
