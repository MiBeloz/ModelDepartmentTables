#ifndef NATURALSORTCOMBOBOX_H
#define NATURALSORTCOMBOBOX_H

#include "NaturalSortListModel.h"

#include <QComboBox>

class NaturalSortComboBox final : public QComboBox {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(NaturalSortComboBox)

public:
    NaturalSortComboBox(QWidget *parent = nullptr);

    void setStringList(const QStringList &strings);
    QStringList stringList() const;

    void addString(const QString &s);
    void removeString(const QString &s);

private:
    NaturalSortListModel *m_model;
};

#endif // NATURALSORTCOMBOBOX_H
