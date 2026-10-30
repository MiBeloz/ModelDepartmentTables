#include "FilteringComboBox.h"

#include <QAbstractItemView>
#include <QLineEdit>

//#include <qOverload>

FilteringComboBox::FilteringComboBox(QWidget *parent) : QComboBox(parent) {
    setEditable(true);

    m_proxyModel = new QSortFilterProxyModel(this);
    m_proxyModel->setSourceModel(model());
    m_proxyModel->setFilterCaseSensitivity(Qt::CaseSensitivity::CaseSensitive);

    m_completer = new QCompleter(m_proxyModel, this);
    m_completer->setCompletionMode(QCompleter::UnfilteredPopupCompletion);
    //m_completer->setMaxVisibleItems(10);

    setCompleter(m_completer);

    connect(lineEdit(),
            &QLineEdit::textEdited,
            m_proxyModel,
            &QSortFilterProxyModel::setFilterFixedString);

    connect(m_completer,
            qOverload<const QString &>(&QCompleter::activated),
            this,
            &FilteringComboBox::onCompleterActivated);

    view()->setMinimumWidth(view()->sizeHintForColumn(0) + 20);
}

void FilteringComboBox::onCompleterActivated(const QString &text) {
    const int index = findText(text);
    if (index >= 0) {
        setCurrentIndex(index);
    }
}
