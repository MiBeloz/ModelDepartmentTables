#include "SettingsSaver.h"


SettingsSaver::SettingsSaver(const QString& filename, QObject *parent)
    : QObject(parent)
{
    m_settings = new QSettings(filename, QSettings::Format::IniFormat, this);
}

void SettingsSaver::readGroup(SettingsGroup& settingsGroup) const {
    m_settings->beginGroup(settingsGroup.getGroupName());
    for (auto& parameter : settingsGroup.parameters()) {
        if (m_settings->contains(parameter.m_name)) {
            settingsGroup.writeParameter(parameter.m_name, m_settings->value(parameter.m_name, parameter.m_defaultValue));
        }
        else {
            m_settings->setValue(parameter.m_name, parameter.m_value);
        }
    }
    m_settings->endGroup();
}

void SettingsSaver::writeGroup(const SettingsGroup& settingsGroup) const {
    m_settings->beginGroup(settingsGroup.getGroupName());
    for (const auto& parameter : std::as_const(settingsGroup.parameters())) {
        m_settings->setValue(parameter.m_name, parameter.m_value);
    }
    m_settings->endGroup();
}
