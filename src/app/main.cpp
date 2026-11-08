#include "MainWindow.h"

#include <QApplication>
#include <QDir>
#include <QLocale>
#include <QStyleFactory>
#include <QStyleHints>
#include <QTranslator>

using namespace Qt::Literals::StringLiterals;

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    qDebug() << QStyleFactory::keys();

    // QApplication::setStyle(u"Fusion"_s);
    // QApplication::setStyle(u"Windows"_s);
    app.styleHints()->setColorScheme(Qt::ColorScheme::Light);
    // app.styleHints()->setColorScheme(Qt::ColorScheme::Dark);
    qDebug() << app.style();

    // app.setStyleSheet((u" QComboBox { background: yellow } QCheckBox { color: red }"_s));

    QTranslator translator;
    const QStringList uiLanguages = QLocale::system().uiLanguages();
    for (const QString &locale : uiLanguages) {
        const QString baseName = u"ModelDepartmentTables_" + QLocale(locale).name();
        if (translator.load(u":/i18n/" + baseName)) {
            app.installTranslator(&translator);
            break;
        }
    }

    MainWindow w;
    w.show();

    return app.exec();
}
