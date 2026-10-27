#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

namespace Ui {
    class AboutDialog;
}

class AboutDialog final : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(AboutDialog)

public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog() override;

private:
    Ui::AboutDialog *m_ui;
};

#endif // ABOUTDIALOG_H
