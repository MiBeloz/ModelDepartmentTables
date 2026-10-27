#include "SettingsDialog.h"

#include "Constants.h"
#include "ui_SettingsDialog.h"

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent), m_ui(new Ui::SettingsDialog()) {
    m_ui->setupUi(this);

    m_settingsSaver = new SettingsSaver(mdt::AppSettings::SettingFilename, this);
    m_appSettings = new SettingsGroup(mdt::AppSettings::GroupName, this);
    m_connectionSettings = new SettingsGroup(mdt::ConnectionSettings::GroupName, this);

    init();
}

SettingsDialog::~SettingsDialog() {
    delete m_ui;
}

void SettingsDialog::open() {
    fillAppSettingsTab();
    fillConnectionSettingsTab();

    m_ui->LineEdit_password->setEchoMode(QLineEdit::EchoMode::Password);
    m_ui->PushButton_showPassword->setIcon(QIcon("icon_eye.png"));

    QDialog::open();
}

void SettingsDialog::init() {
    initLanguages();
    initAppSettings();
    initConnectionSettings();

    connect(m_ui->PushButton_save, &QPushButton::clicked, this, &SettingsDialog::rec_saveParameters);
    connect(m_ui->PushButton_cancel, &QPushButton::clicked, this, [this] { QDialog::close(); });
    connect(m_ui->PushButton_showPassword, &QPushButton::clicked, this, [this] {
        if (m_ui->LineEdit_password->echoMode() == QLineEdit::EchoMode::Normal) {
            m_ui->LineEdit_password->setEchoMode(QLineEdit::EchoMode::Password);
            m_ui->PushButton_showPassword->setIcon(QIcon("icon_eye.png"));
        } else {
            m_ui->LineEdit_password->setEchoMode(QLineEdit::EchoMode::Normal);
            m_ui->PushButton_showPassword->setIcon(QIcon("icon_hide.png"));
        }
    });
}

void SettingsDialog::initLanguages() {
    Language newLang;
    newLang.name = "Automatic language recognition";
    newLang.code = "auto";
    m_languages.append(newLang);

    // newLang.name = "English";
    // newLang.code = "en";
    // m_languages.append(newLang);

    newLang.name = QString::fromUtf8("Русский");
    newLang.code = "ru";
    m_languages.append(newLang);
}

void SettingsDialog::initAppSettings() {
    m_appSettings->addParameters(QList<SettingsGroup::Parameter> {
        SettingsGroup::Parameter(mdt::AppSettings::Language,
                                 mdt::AppSettings::LanguageDefaultValue,
                                 mdt::AppSettings::LanguageDefaultValue),
        SettingsGroup::Parameter(mdt::AppSettings::Width,
                                 mdt::AppSettings::WidthDefaultValue,
                                 mdt::AppSettings::WidthDefaultValue),
        SettingsGroup::Parameter(mdt::AppSettings::Height,
                                 mdt::AppSettings::HeightDefaultValue,
                                 mdt::AppSettings::HeightDefaultValue),
        SettingsGroup::Parameter(mdt::AppSettings::RestoreMainWindow,
                                 mdt::AppSettings::RestoreMainWindowDefaultValue,
                                 mdt::AppSettings::RestoreMainWindowDefaultValue),
        SettingsGroup::Parameter(mdt::AppSettings::DateFormat,
                                 mdt::AppSettings::DateFormatDefaultValue,
                                 mdt::AppSettings::DateFormatDefaultValue),
    });
    m_settingsSaver->readGroup(*m_appSettings);
}

void SettingsDialog::initConnectionSettings() {
    m_connectionSettings->addParameters(QList<SettingsGroup::Parameter> {
        SettingsGroup::Parameter(mdt::ConnectionSettings::Host,
                                 mdt::ConnectionSettings::HostDefaultValue,
                                 mdt::ConnectionSettings::HostDefaultValue),
        SettingsGroup::Parameter(mdt::ConnectionSettings::Port,
                                 mdt::ConnectionSettings::PortDefaultValue,
                                 mdt::ConnectionSettings::PortDefaultValue),
        SettingsGroup::Parameter(mdt::ConnectionSettings::DatabaseName,
                                 mdt::ConnectionSettings::DatabaseNameDefaultValue,
                                 mdt::ConnectionSettings::DatabaseNameDefaultValue),
        SettingsGroup::Parameter(mdt::ConnectionSettings::User,
                                 mdt::ConnectionSettings::UserDefaultValue,
                                 mdt::ConnectionSettings::UserDefaultValue),
        SettingsGroup::Parameter(mdt::ConnectionSettings::Password,
                                 mdt::ConnectionSettings::PasswordDefaultValue,
                                 mdt::ConnectionSettings::PasswordDefaultValue),
        SettingsGroup::Parameter(mdt::ConnectionSettings::SavePassword,
                                 mdt::ConnectionSettings::SavePasswordDefaultValue,
                                 mdt::ConnectionSettings::SavePasswordDefaultValue),
        SettingsGroup::Parameter(mdt::ConnectionSettings::AutoConnect,
                                 mdt::ConnectionSettings::AutoConnectDefaultValue,
                                 mdt::ConnectionSettings::AutoConnectDefaultValue),
    });
    m_settingsSaver->readGroup(*m_connectionSettings);
}

void SettingsDialog::fillAppSettingsTab() {
    int langIndex = 0;
    for (int i = 0; i < m_languages.count(); i++) {
        m_ui->ComboBox_language->addItem(m_languages.at(i).name, m_languages.at(i).code);
        if (m_languages.at(i).code ==
            m_appSettings->readParameter(mdt::AppSettings::Language).toString()) {
            langIndex = i;
        }
    }
    m_ui->ComboBox_language->setCurrentIndex(langIndex);
    m_ui->SpinBox_mainWindowWidth->setValue(
        m_appSettings->readParameter(mdt::AppSettings::Width).toInt());
    m_ui->SpinBox_mainWindowHeight->setValue(
        m_appSettings->readParameter(mdt::AppSettings::Height).toInt());
    m_ui->CheckBox_restoreMainWindow->setChecked(
        m_appSettings->readParameter(mdt::AppSettings::RestoreMainWindow).toBool());
    m_ui->LineEdit_dateFormat->setText(
        m_appSettings->readParameter(mdt::AppSettings::DateFormat).toString());
}

void SettingsDialog::fillConnectionSettingsTab() {
    m_ui->LineEdit_host->setText(
        m_connectionSettings->readParameter(mdt::ConnectionSettings::Host).toString());
    m_ui->SpinBox_port->setValue(
        m_connectionSettings->readParameter(mdt::ConnectionSettings::Port).toInt());
    m_ui->LineEdit_dbName->setText(
        m_connectionSettings->readParameter(mdt::ConnectionSettings::DatabaseName).toString());
    m_ui->LineEdit_user->setText(
        m_connectionSettings->readParameter(mdt::ConnectionSettings::User).toString());
    m_ui->LineEdit_password->setText(
        m_connectionSettings->readParameter(mdt::ConnectionSettings::Password).toString());
    m_ui->CheckBox_savePassword->setChecked(
        m_connectionSettings->readParameter(mdt::ConnectionSettings::SavePassword).toBool());
    m_ui->CheckBox_autoConnect->setChecked(
        m_connectionSettings->readParameter(mdt::ConnectionSettings::AutoConnect).toBool());
}

void SettingsDialog::saveAppSettings() {
    int width = m_appSettings->readParameter(mdt::AppSettings::Width).toInt();
    int height = m_appSettings->readParameter(mdt::AppSettings::Height).toInt();
    if (width != m_ui->SpinBox_mainWindowWidth->value() ||
        height != m_ui->SpinBox_mainWindowHeight->value()) {
        emit sig_resizeMainWindow(m_ui->SpinBox_mainWindowWidth->value(),
                                  m_ui->SpinBox_mainWindowHeight->value());
    }
    m_appSettings->writeParameter(mdt::AppSettings::Language,
                                  m_ui->ComboBox_language->currentData().toString());
    bool rw = m_ui->CheckBox_restoreMainWindow->isChecked();
    m_appSettings->writeParameter(mdt::AppSettings::RestoreMainWindow, rw);
    if (rw) {
        m_appSettings->writeParameter(mdt::AppSettings::Width, m_ui->SpinBox_mainWindowWidth->value());
        m_appSettings->writeParameter(mdt::AppSettings::Height,
                                      m_ui->SpinBox_mainWindowHeight->value());
    } else {
        m_appSettings->writeParameter(mdt::AppSettings::Width, mdt::AppSettings::WidthDefaultValue);
        m_appSettings->writeParameter(mdt::AppSettings::Height,
                                      mdt::AppSettings::HeightDefaultValue);
    }
    m_appSettings->writeParameter(mdt::AppSettings::DateFormat, m_ui->LineEdit_dateFormat->text());

    m_settingsSaver->writeGroup(*m_appSettings);
}

void SettingsDialog::saveConnectionSettings() {
    m_connectionSettings->writeParameter(mdt::ConnectionSettings::Host, m_ui->LineEdit_host->text());
    m_connectionSettings->writeParameter(mdt::ConnectionSettings::Port, m_ui->SpinBox_port->text());
    m_connectionSettings->writeParameter(mdt::ConnectionSettings::DatabaseName,
                                         m_ui->LineEdit_dbName->text());
    m_connectionSettings->writeParameter(mdt::ConnectionSettings::User, m_ui->LineEdit_user->text());
    if (m_ui->CheckBox_savePassword->isChecked()) {
        m_connectionSettings->writeParameter(mdt::ConnectionSettings::Password,
                                             m_ui->LineEdit_password->text());
    } else {
        m_connectionSettings->writeParameter(mdt::ConnectionSettings::Password, QString());
    }
    m_connectionSettings->writeParameter(mdt::ConnectionSettings::SavePassword,
                                         m_ui->CheckBox_savePassword->isChecked());
    m_connectionSettings->writeParameter(mdt::ConnectionSettings::AutoConnect,
                                         m_ui->CheckBox_autoConnect->isChecked());

    m_settingsSaver->writeGroup(*m_connectionSettings);
}

void SettingsDialog::rec_saveParameters() {
    saveAppSettings();
    saveConnectionSettings();

    QDialog::close();
}
