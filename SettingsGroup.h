#ifndef SETTINGSGROUP_H
#define SETTINGSGROUP_H

#include <QObject>
#include <QList>
#include <QVariant>


class SettingsGroup : public QObject {
    Q_OBJECT

public:
    struct Parameter {
        explicit Parameter(const QString& name, const QVariant& value, const QVariant& defaultValue)
            : m_name(name), m_value(value), m_defaultValue(defaultValue) {}

        bool operator==(const Parameter &other) const {
            return m_name == other.m_name && m_value == other.m_value && m_defaultValue == other.m_defaultValue;
        }

        QString m_name;
        QVariant m_value;
        QVariant m_defaultValue;
    };

    explicit SettingsGroup(const QString& groupName, QObject *parent = nullptr);
    virtual ~SettingsGroup() {}

    bool operator==(const SettingsGroup &other) const;

    void addParameter(const Parameter& parameter);
    void addParameters(const QList<Parameter>& parameters);

    QString getGroupName() const;
    const QList<Parameter>& parameters() const;

    QVariant readParameter(const QString& parameterName) const;
    void writeParameter(const QString& parameterName, const QVariant& parameter);

private:
    QString m_groupName;
    QList<Parameter> m_parameters;
};

#endif // SETTINGSGROUP_H
