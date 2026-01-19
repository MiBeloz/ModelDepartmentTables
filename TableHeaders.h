#ifndef TABLEHEADERS_H
#define TABLEHEADERS_H

#include <QCoreApplication>


class TableHeaders {
    Q_DECLARE_TR_FUNCTIONS(TableHeaders)

public:
    static QStringList itemsTableHeaders() {
        return {
            tr("Date"),
            tr("# Drawing"),
            tr("Name"),
            tr("Executor"),
            tr("Amount"),
            tr("Casting Material"),
            tr("Model Material"),
            tr("Machine"),
            tr("Note")
        };
    }
};

#endif // TABLEHEADERS_H
