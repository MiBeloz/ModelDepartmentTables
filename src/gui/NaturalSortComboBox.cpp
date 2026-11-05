#include "NaturalSortComboBox.h"

NaturalSortComboBox::NaturalSortComboBox(QWidget *parent)
    : QComboBox(parent)
    , m_model(new NaturalSortListModel(this)) {
    setModel(m_model);
    setEditable(false);
}

void NaturalSortComboBox::setStringList(const QStringList &strings) {
    m_model->setStringList(strings);
    setCurrentIndex(-1);
}

QStringList NaturalSortComboBox::sortedStringList() const {
    return m_model->sortedStringList();
}

bool NaturalSortComboBox::addString(const QString &text) {
    return m_model->appendString(text);
}

bool NaturalSortComboBox::removeString(const QString &text) {
    return m_model->removeString(text);
}
