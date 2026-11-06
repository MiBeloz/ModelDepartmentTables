#ifndef NATURALSORTFILTERINGCOMBOBOX_H
#define NATURALSORTFILTERINGCOMBOBOX_H

#include "NaturalSortComboBox.h"

#include <QCompleter>

class NaturalSortFilteringComboBox final : public NaturalSortComboBox {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(NaturalSortFilteringComboBox)

public:
    explicit NaturalSortFilteringComboBox(QWidget *parent = nullptr);

private slots:
    void onCompleterActivated(const QString &text);
};

#endif // NATURALSORTFILTERINGCOMBOBOX_H
