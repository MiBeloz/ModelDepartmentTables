#include <QApplication>
#include <QDir>
#include <QLocale>
#include <QTranslator>

#include "MainWindow.h"
#include "xlsxdocument.h"
#include "xlsxworkbook.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QXlsx::Document xlsxW;

    int row = 1;
    int col = 1; //

    // xlsxW.write("A1", "Hello Qt!"); // write "Hello Qt!" to cell(A,1). it's shared string.
    QVariant writeValue = QString("Hello Qt!");
    xlsxW.write(row, col, writeValue);

    if (xlsxW.saveAs("Test.xlsx")) // save the document as 'Test.xlsx'
    {
        qDebug() << "[debug] success to write xlsx file";
    } else {
        qDebug() << "[debug][error] failed to write xlsx file";
        exit(-1);
    }

    qDebug() << "[debug] current directory is " << QDir::currentPath();

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = "ModelDepartmentTables_" + QLocale(locale).name();
        if (translator.load(":/i18n/" + baseName)) {
            a.installTranslator(&translator);
            break;
        }
    }

    MainWindow w;
    w.show();

    return a.exec();
}
