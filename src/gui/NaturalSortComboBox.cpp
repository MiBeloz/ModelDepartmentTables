#include "NaturalSortComboBox.h"

NaturalSortComboBox::NaturalSortComboBox(QWidget *parent)
    : QComboBox(parent)
    , m_model(new NaturalSortListModel(this)) {
    setModel(m_model);
}

void NaturalSortComboBox::setStringList(const QStringList &strings) {
    m_model->setStringList(strings);
    setCurrentIndex(-1);
    setEditable(false);
}

QStringList NaturalSortComboBox::stringList() const {
    return m_model->stringList();
}

void NaturalSortComboBox::addString(const QString &s) {
    QStringList list = m_model->stringList();
    list.append(s);
    m_model->setStringList(list);
    setCurrentIndex(-1);
}

void NaturalSortComboBox::removeString(const QString &s) {
    QStringList list = m_model->stringList();
    if (list.removeOne(s)) {
        m_model->setStringList(list);
    }
}
