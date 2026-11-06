#ifndef NATURALSORTCOMBOBOX_H
#define NATURALSORTCOMBOBOX_H

#include "NaturalSortListModel.h"

#include <QComboBox>

class NaturalSortComboBox : public QComboBox {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(NaturalSortComboBox)

public:
    NaturalSortComboBox(QWidget *parent = nullptr);

    void setStringList(const QStringList &strings);
    QStringList sortedStringList() const;

    bool addString(const QString &text);
    bool removeString(const QString &text);

private:
    NaturalSortListModel *m_model;
};

#endif // NATURALSORTCOMBOBOX_H
