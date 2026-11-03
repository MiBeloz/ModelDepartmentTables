#ifndef NATURALSORTCOMBOBOX_H
#define NATURALSORTCOMBOBOX_H

#include "NaturalSortListModel.h"

#include <QComboBox>

using namespace Qt::Literals::StringLiterals;

class NaturalSortComboBox : public QComboBox {
public:
    NaturalSortComboBox(QWidget *parent = nullptr)
        : QComboBox(parent)
        , m_model(new NaturalSortListModel(this)) {
        setModel(m_model);
    }

    void setStringList(const QStringList &strings) {
        m_model->setStringList(strings);
        setCurrentIndex(-1);
        setEditable(false);
        setStyleSheet(u"QComboBox { combobox-popup: 0; }"_s);
        setMaxVisibleItems(10);
    }

    QStringList stringList() const {
        return m_model->stringList();
    }

    void addString(const QString &s) {
        QStringList list = m_model->stringList();
        list.append(s);
        m_model->setStringList(list);
    }

    void removeString(const QString &s) {
        QStringList list = m_model->stringList();
        if (list.removeOne(s)) {
            m_model->setStringList(list);
        }
    }

private:
    NaturalSortListModel *m_model;
};

#endif // NATURALSORTCOMBOBOX_H
