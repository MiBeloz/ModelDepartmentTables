#include "AboutDialog.h"

#include "Version.h"
#include "ui_AboutDialog.h"

using namespace Qt::Literals::StringLiterals;

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent), m_ui(new Ui::AboutDialog) {
    m_ui->setupUi(this);

    // Title
    m_ui->labelAppName->setText(u"<b><h2>ModelDepartmentTables:</h2></b>"_s);
    m_ui->labelAppVer->setText(
        QString(u"<b><h2>" MDT_VERSION_DISPLAY " (%1-bit)</h2></b>"_s).arg(QT_POINTER_SIZE * 8));

    // Compiler
    QString compilerInfo = u"Unknown Compiler"_s;
#if defined(Q_CC_GNU)
    compilerInfo = QString(u"GCC %1"_s).arg(__VERSION__);
#elif defined(Q_CC_MSVC)
    int msvcVer = _MSC_VER;
    if (msvcVer >= 1930) {
        compilerInfo = u"MSVC 2022"_s;
    } else if (msvcVer >= 1920) {
        compilerInfo = u"MSVC 2019"_s;
    } else if (msvcVer >= 1910) {
        compilerInfo = u"MSVC 2017"_s;
    } else {
        compilerInfo = QString(u"MSVC (Version %1)"_s).arg(msvcVer);
    }
#elif defined(Q_CC_CLANG)
    compilerInfo = QString(u"Clang %1.%2"_s).arg(__clang_major__).arg(__clang_minor__);
#endif
    m_ui->labelCompilerVer->setText(compilerInfo);

    // Qt
    m_ui->labelQtVer->setText(QStringLiteral(QT_VERSION_STR));

    // QXlsx
    m_ui->labelQXlsxVer->setText(QStringLiteral(QXLSX_VERSION_STRING));
}

AboutDialog::~AboutDialog() {
    delete m_ui;
}
