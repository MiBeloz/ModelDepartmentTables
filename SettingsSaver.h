#ifndef SETTINGSSAVER_H
#define SETTINGSSAVER_H

#include <QObject>
#include <QSettings>

#include "SettingsGroup.h"


class SettingsSaver : public QObject {
    Q_OBJECT

public:
    explicit SettingsSaver(const QString& filename, QObject *parent = nullptr);
    virtual ~SettingsSaver() {};

    void readGroup(SettingsGroup& settingsGroup) const;
    void writeGroup(const SettingsGroup& settingsGroup) const;

private:
    QSettings *m_settings;
};

#endif // SETTINGSSAVER_H
