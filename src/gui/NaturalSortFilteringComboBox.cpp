#include "NaturalSortFilteringComboBox.h"

#include <QLineEdit>

NaturalSortFilteringComboBox::NaturalSortFilteringComboBox(QWidget *parent)
    : NaturalSortComboBox(parent) {
    this->setEditable(true);

    this->completer()->setCompletionMode(QCompleter::PopupCompletion);
    this->completer()->setFilterMode(Qt::MatchContains);

    connect(this->completer(),
            qOverload<const QString &>(&QCompleter::activated),
            this,
            &NaturalSortFilteringComboBox::onCompleterActivated);
}

void NaturalSortFilteringComboBox::onCompleterActivated(const QString &text) {
    //this->setCurrentText(text);
}
