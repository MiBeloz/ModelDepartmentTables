#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

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
    Language(): isRTL(false) {}

    QString name;
    QString code;
    bool isRTL;
};

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QDialog
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();

signals:
    void sig_resizeMainWindow(int width, int height);

public Q_SLOTS:
    void open() override;

private:
    Ui::SettingsWindow *ui;

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

#endif // SETTINGSWINDOW_H
