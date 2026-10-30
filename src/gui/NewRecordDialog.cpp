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

void NewRecordDialog::on_pushButtonOk_clicked() {
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

void NewRecordDialog::on_pushButtonCancel_clicked() {
    reject();
}

void NewRecordDialog::init() {
    connect(m_ui->comboBoxDrawingNumberValue,
            &QComboBox::currentTextChanged,
            this,
            [&](const QString &text) {
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

    m_ui->comboBoxDrawingNumberValue->addItems(numbers);
    m_ui->comboBoxDrawingNumberValue->setCurrentIndex(-1);

    auto executors = m_storageWork.get().executors();
    std::sort(executors.begin(), executors.end(), naturalLess);
    m_ui->comboBoxAddExecutor->addItems(executors);
    m_ui->comboBoxAddExecutor->setCurrentIndex(-1);
    m_ui->comboBoxAddExecutor->setEditable(false);
    m_ui->comboBoxAddExecutor->setStyleSheet(u"QComboBox { combobox-popup: 0; }"_s);
    m_ui->comboBoxAddExecutor->setMaxVisibleItems(10);
    m_ui->comboBoxAddExecutor->setPlaceholderText(u"Select executor..."_s);
    m_ui->listWidgetExecutorsValue->setFlow(QListView::LeftToRight);
    m_ui->listWidgetExecutorsValue->setWrapping(true);
    m_ui->listWidgetExecutorsValue->setResizeMode(QListView::Adjust);
    m_ui->listWidgetExecutorsValue->setSpacing(4);

    m_ui->listWidgetExecutorsValue->installEventFilter(this);
    m_ui->listWidgetExecutorsValue->setSelectionMode(QAbstractItemView::SingleSelection);

    connect(
        m_ui->listWidgetExecutorsValue,
        &QListWidget::itemDoubleClicked,
        this,
        [&](QListWidgetItem *item) {
            if (!item) {
                return;
            }
            delete m_ui->listWidgetExecutorsValue->takeItem(
                m_ui->listWidgetExecutorsValue->row(item));

            auto *model = qobject_cast<QStandardItemModel *>(m_ui->comboBoxAddExecutor->model());
            if (!model) {
                return;
            }

            for (int i = 0; i < model->rowCount(); ++i) {
                QStandardItem *item = model->item(i);
                if (!item) {
                    continue;
                }
                const bool used =
                    !m_ui->listWidgetExecutorsValue->findItems(item->text(), Qt::MatchExactly).isEmpty();
                item->setEnabled(!used);
            }
        });

    connect(m_ui->comboBoxAddExecutor,
            QOverload<int>::of(&QComboBox::activated),
            this,
            [&](int index) {
                if (index < 0) {
                    return;
                }

                const QString text = m_ui->comboBoxAddExecutor->itemText(index);
                m_ui->listWidgetExecutorsValue->addItem(text);

                m_ui->comboBoxAddExecutor->setCurrentIndex(-1);

                for (int i = 0; i < m_ui->comboBoxAddExecutor->count(); ++i) {
                    const QString t = m_ui->comboBoxAddExecutor->itemText(i);
                    const bool used =
                        !m_ui->listWidgetExecutorsValue->findItems(t, Qt::MatchExactly).isEmpty();
                    if (auto *model = qobject_cast<QStandardItemModel *>(
                            m_ui->comboBoxAddExecutor->model())) {
                        if (auto *item = model->item(i)) {
                            item->setEnabled(!used);
                        }
                    }
                }
            });

    auto authors = m_storageWork.get().authors();
    std::sort(authors.begin(), authors.end(), naturalLess);
    m_ui->comboBoxAddAuthor->addItems(authors);
    m_ui->comboBoxAddAuthor->setCurrentIndex(-1);
    m_ui->comboBoxAddAuthor->setEditable(false);

    auto castingMaterials = m_storageWork.get().castingMaterials();
    std::sort(castingMaterials.begin(), castingMaterials.end(), naturalLess);
    m_ui->comboBoxAddCastingMaterial->addItems(castingMaterials);
    m_ui->comboBoxAddCastingMaterial->setCurrentIndex(-1);
    m_ui->comboBoxAddCastingMaterial->setEditable(false);

    auto modelMaterials = m_storageWork.get().modelMaterials();
    std::sort(modelMaterials.begin(), modelMaterials.end(), naturalLess);
    m_ui->comboBoxAddModelMaterial->addItems(modelMaterials);
    m_ui->comboBoxAddModelMaterial->setCurrentIndex(-1);
    m_ui->comboBoxAddModelMaterial->setEditable(false);

    auto machines = m_storageWork.get().machines();
    std::sort(machines.begin(), machines.end(), naturalLess);
    m_ui->comboBoxAddMachine->addItems(machines);
    m_ui->comboBoxAddMachine->setCurrentIndex(-1);
    m_ui->comboBoxAddMachine->setEditable(false);

    auto notes = m_storageWork.get().notes();
    std::sort(notes.begin(), notes.end(), naturalLess);
    m_ui->comboBoxAddNote->addItems(notes);
    m_ui->comboBoxAddNote->setCurrentIndex(-1);
    m_ui->comboBoxAddNote->setEditable(false);
}
