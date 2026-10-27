#include "AboutDialog.h"

#include "Version.h"
#include "ui_AboutDialog.h"

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent), m_ui(new Ui::AboutDialog) {
    m_ui->setupUi(this);

    // Title
    m_ui->labelAppName->setText(QStringLiteral("<b><h2>ModelDepartmentTables:</h2></b>"));
    m_ui->labelAppVer->setText(
        QStringLiteral("<b><h2>" MDT_VERSION_DISPLAY " (%1-bit)</h2></b>").arg(QT_POINTER_SIZE * 8));

    // Compiler
    QString compilerInfo = "Unknown Compiler";
#if defined(Q_CC_GNU)
    compilerInfo = QString("GCC %1").arg(__VERSION__);
#elif defined(Q_CC_MSVC)
    int msvcVer = _MSC_VER;
    if (msvcVer >= 1930) {
        compilerInfo = "MSVC 2022";
    } else if (msvcVer >= 1920) {
        compilerInfo = "MSVC 2019";
    } else if (msvcVer >= 1910) {
        compilerInfo = "MSVC 2017";
    } else {
        compilerInfo = QString("MSVC (Version %1)").arg(msvcVer);
    }
#elif defined(Q_CC_CLANG)
    compilerInfo = QString("Clang %1.%2").arg(__clang_major__).arg(__clang_minor__);
#endif
    m_ui->labelCompilerVer->setText(compilerInfo);

    // Qt
    m_ui->labelQtVer->setText(QStringLiteral(QT_VERSION_STR));
}

AboutDialog::~AboutDialog() {
    delete m_ui;
}
