#include "SettingsGroup.h"


SettingsGroup::SettingsGroup(const QString& groupName, QObject *parent)
    : m_groupName(groupName)
    , QObject(parent) {}

bool SettingsGroup::operator==(const SettingsGroup& other) const {
    return m_groupName == other.m_groupName && m_parameters == other.m_parameters;
}

void SettingsGroup::addParameter(const Parameter& parameter) {
    if (qsizetype index = m_parameters.indexOf(parameter); index >= 0) {
        m_parameters[index].m_value = parameter.m_value;
        m_parameters[index].m_defaultValue = parameter.m_defaultValue;
    }
    else {
        m_parameters.append(parameter);
    }
}

void SettingsGroup::addParameters(const QList<Parameter>& parameters) {
    for (const auto& parameter : std::as_const(parameters)) {
        addParameter(parameter);
    }
}

QString SettingsGroup::getGroupName() const {
    return m_groupName;
}

QVariant SettingsGroup::readParameter(const QString& parameterName) const {
    for (const auto& p : std::as_const(m_parameters)) {
        if (p.m_name == parameterName) {
            return p.m_value;
        }
    }
    return QVariant();
}

void SettingsGroup::writeParameter(const QString& parameterName, const QVariant& parameter) {
    for (auto& p : m_parameters) {
        if (p.m_name == parameterName) {
            p.m_value = parameter;
        }
    }
}

const QList<SettingsGroup::Parameter>& SettingsGroup::parameters() const {
    return m_parameters;
}
