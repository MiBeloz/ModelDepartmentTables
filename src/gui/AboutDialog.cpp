#include "AboutDialog.h"

#include "Version.h"
#include "ui_AboutDialog.h"

AboutDialog::AboutDialog(QWidget *parent) : QDialog(parent), ui(new Ui::AboutDialog) {
    ui->setupUi(this);

    // Title
    ui->label->setText(
        QStringLiteral("<b><h2>ModelDepartmentTables " MDT_VERSION_DISPLAY " (%1-bit)</h2></b>")
            .arg(QT_POINTER_SIZE * 8));
}

AboutDialog::~AboutDialog() {
    delete ui;
}
