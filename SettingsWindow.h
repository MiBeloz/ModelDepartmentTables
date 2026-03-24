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
    ~SettingsWindow() {}

signals:
    void sig_resizeMainWindow(int width, int height);

public Q_SLOTS:
    void open() override;

private:
    std::unique_ptr<Ui::SettingsWindow> ui;

    QList<Language> m_languages;
    SettingsSaver *m_settingsSaver;
    SettingsGroup *m_appSettings;

    void init();
    void initLanguages();

private slots:
    void rec_saveParameters();
};

#endif // SETTINGSWINDOW_H
