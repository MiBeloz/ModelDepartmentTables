#ifndef FILTERINGCOMBOBOX_H
#define FILTERINGCOMBOBOX_H

#include <QAbstractItemModel>
#include <QComboBox>
#include <QCompleter>
#include <QPointer>
#include <QSortFilterProxyModel>

class FilteringComboBox final : public QComboBox {
    Q_OBJECT

public:
    explicit FilteringComboBox(QWidget *parent = nullptr);

private slots:
    void onCompleterActivated(const QString &text);

private:
    QPointer<QSortFilterProxyModel> m_proxyModel = nullptr;
    QPointer<QCompleter> m_completer = nullptr;
};

#endif // FILTERINGCOMBOBOX_H
