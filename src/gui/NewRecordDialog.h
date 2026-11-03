#ifndef NEWRECORDDIALOG_H
#define NEWRECORDDIALOG_H

#include "NaturalSortComboBox.h"
#include "StorageManager.h"

#include <QDialog>
#include <QListWidget>
#include <QPointer>
#include <QStandardItemModel>

namespace Ui {
    class NewRecordDialog;
}

class NewRecordDialog final : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(NewRecordDialog)

public:
    explicit NewRecordDialog(QWidget *parent = nullptr);
    ~NewRecordDialog();

signals:
    //void sig_handleNewItem(const ItemData &newItem);

private slots:
    void onPushButtonOkClicked();
    void onPushButtonCancelClicked();

private:
    Ui::NewRecordDialog *m_ui;
    mdt::work::StorageManager m_storageWork;
    QPointer<QStandardItemModel> m_sourceModelDrawings;

    void init();

    void createListComboPair(QListWidget *list, NaturalSortComboBox *combo) {
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
};

#endif // NEWRECORDDIALOG_H
