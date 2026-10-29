#include "FilteringComboBox.h"

#include <QAbstractItemView>
#include <QLineEdit>

FilteringComboBox::FilteringComboBox(QWidget *parent) : QComboBox(parent) {
    setEditable(true);
    setCurrentIndex(-1);

    m_proxyModel = new QSortFilterProxyModel(this);
    m_completer = new QCompleter(m_proxyModel, this);
    m_completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    m_completer->setMaxVisibleItems(10);

    setCompleter(m_completer);

    connect(lineEdit(),
            &QLineEdit::textEdited,
            m_proxyModel,
            &QSortFilterProxyModel::setFilterFixedString);

    connect(m_completer,
            QOverload<const QString &>::of(&QCompleter::activated),
            this,
            &FilteringComboBox::onCompleterActivated);

    view()->setMinimumWidth(view()->sizeHintForColumn(0) + 20);
}

void FilteringComboBox::setSourceModel(QAbstractItemModel *sourceModel) {
    m_proxyModel->setSourceModel(sourceModel);
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseInsensitive);
    m_proxyModel->setFilterKeyColumn(0);
}

void FilteringComboBox::onCompleterActivated(const QString &text) {
    const int index = findText(text);
    if (index >= 0) {
        setCurrentIndex(index);
    }
}
