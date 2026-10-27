#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QDialog>

#include "SettingsSaver.h"

enum ApplicationSettings {
    language,
    width,
    height,
    restoreMainWindow
};

enum ConnectionSettings {
    host,
    port,
    dbName,
    user,
    password,
    autoConnect
};

struct Language {
    Language() : isRTL(false) { }

    QString name;
    QString code;
    bool isRTL;
};

namespace Ui {
    class SettingsDialog;
}

class SettingsDialog final : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(SettingsDialog)

public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

signals:
    void sig_resizeMainWindow(int width, int height);

public Q_SLOTS:
    void open() override;

private:
    Ui::SettingsDialog *m_ui;

    QList<Language> m_languages;
    SettingsSaver *m_settingsSaver;
    SettingsGroup *m_appSettings;
    SettingsGroup *m_connectionSettings;

    void init();
    void initLanguages();
    void initAppSettings();
    void initConnectionSettings();
    void fillAppSettingsTab();
    void fillConnectionSettingsTab();
    void saveAppSettings();
    void saveConnectionSettings();

private slots:
    void rec_saveParameters();
};

#endif // SETTINGSDIALOG_H
