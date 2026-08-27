#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"
#include "Constants.h"


SettingsWindow::SettingsWindow(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::SettingsWindow())
{
    ui->setupUi(this);

    m_settingsSaver = new SettingsSaver(mdt::AppSettings::SettingFilename, this);
    m_appSettings = new SettingsGroup(mdt::AppSettings::GroupName, this);
    m_connectionSettings = new SettingsGroup(mdt::ConnectionSettings::GroupName, this);

    init();
}

SettingsWindow::~SettingsWindow() {
    delete ui;
}

void SettingsWindow::open() {
    fillAppSettingsTab();
    fillConnectionSettingsTab();

    ui->LineEdit_password->setEchoMode(QLineEdit::EchoMode::Password);
    ui->PushButton_showPassword->setIcon(QIcon("icon_eye.png"));

    QDialog::open();
}

void SettingsWindow::init() {
    initLanguages();
    initAppSettings();
    initConnectionSettings();

    connect(ui->PushButton_save, &QPushButton::clicked, this, &SettingsWindow::rec_saveParameters);
    connect(ui->PushButton_cancel, &QPushButton::clicked, this, [this] { QDialog::close(); });
    connect(ui->PushButton_showPassword, &QPushButton::clicked, this, [this] {
        if (ui->LineEdit_password->echoMode() == QLineEdit::EchoMode::Normal) {
            ui->LineEdit_password->setEchoMode(QLineEdit::EchoMode::Password);
            ui->PushButton_showPassword->setIcon(QIcon("icon_eye.png"));
        }
        else {
            ui->LineEdit_password->setEchoMode(QLineEdit::EchoMode::Normal);
            ui->PushButton_showPassword->setIcon(QIcon("icon_hide.png"));
        }
    });
}

void SettingsWindow::initLanguages() {
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

void SettingsWindow::initAppSettings() {
    m_appSettings->addParameters(QList<SettingsGroup::Parameter>{
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

void SettingsWindow::initConnectionSettings() {
    m_connectionSettings->addParameters(QList<SettingsGroup::Parameter>{
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

void SettingsWindow::fillAppSettingsTab() {
    int langIndex = 0;
    for (int i = 0; i < m_languages.count(); i++) {
        ui->ComboBox_language->addItem(m_languages.at(i).name, m_languages.at(i).code);
        if (m_languages.at(i).code == m_appSettings->readParameter(mdt::AppSettings::Language).toString()) {
            langIndex = i;
        }
    }
    ui->ComboBox_language->setCurrentIndex(langIndex);
    ui->SpinBox_mainWindowWidth->setValue(m_appSettings->readParameter(mdt::AppSettings::Width).toInt());
    ui->SpinBox_mainWindowHeight->setValue(m_appSettings->readParameter(mdt::AppSettings::Height).toInt());
    ui->CheckBox_restoreMainWindow->setChecked(m_appSettings->readParameter(mdt::AppSettings::RestoreMainWindow).toBool());
    ui->LineEdit_dateFormat->setText(m_appSettings->readParameter(mdt::AppSettings::DateFormat).toString());
}

void SettingsWindow::fillConnectionSettingsTab() {
    ui->LineEdit_host->setText(m_connectionSettings->readParameter(mdt::ConnectionSettings::Host).toString());
    ui->SpinBox_port->setValue(m_connectionSettings->readParameter(mdt::ConnectionSettings::Port).toInt());
    ui->LineEdit_dbName->setText(m_connectionSettings->readParameter(mdt::ConnectionSettings::DatabaseName).toString());
    ui->LineEdit_user->setText(m_connectionSettings->readParameter(mdt::ConnectionSettings::User).toString());
    ui->LineEdit_password->setText(m_connectionSettings->readParameter(mdt::ConnectionSettings::Password).toString());
    ui->CheckBox_savePassword->setChecked(m_connectionSettings->readParameter(mdt::ConnectionSettings::SavePassword).toBool());
    ui->CheckBox_autoConnect->setChecked(m_connectionSettings->readParameter(mdt::ConnectionSettings::AutoConnect).toBool());
}

void SettingsWindow::saveAppSettings() {
    int width = m_appSettings->readParameter(mdt::AppSettings::Width).toInt();
    int height = m_appSettings->readParameter(mdt::AppSettings::Height).toInt();
    if (width != ui->SpinBox_mainWindowWidth->value() || height != ui->SpinBox_mainWindowHeight->value()) {
        emit sig_resizeMainWindow(ui->SpinBox_mainWindowWidth->value(), ui->SpinBox_mainWindowHeight->value());
    }
    m_appSettings->writeParameter(mdt::AppSettings::Language, ui->ComboBox_language->currentData().toString());
    bool rw = ui->CheckBox_restoreMainWindow->isChecked();
    m_appSettings->writeParameter(mdt::AppSettings::RestoreMainWindow, rw);
    if (rw) {
        m_appSettings->writeParameter(mdt::AppSettings::Width, ui->SpinBox_mainWindowWidth->value());
        m_appSettings->writeParameter(mdt::AppSettings::Height, ui->SpinBox_mainWindowHeight->value());
    }
    else {
        m_appSettings->writeParameter(mdt::AppSettings::Width, mdt::AppSettings::WidthDefaultValue);
        m_appSettings->writeParameter(mdt::AppSettings::Height, mdt::AppSettings::HeightDefaultValue);
    }
    m_appSettings->writeParameter(mdt::AppSettings::DateFormat, ui->LineEdit_dateFormat->text());

    m_settingsSaver->writeGroup(*m_appSettings);
}

void SettingsWindow::saveConnectionSettings() {
    m_connectionSettings->writeParameter(mdt::ConnectionSettings::Host, ui->LineEdit_host->text());
    m_connectionSettings->writeParameter(mdt::ConnectionSettings::Port, ui->SpinBox_port->text());
    m_connectionSettings->writeParameter(mdt::ConnectionSettings::DatabaseName, ui->LineEdit_dbName->text());
    m_connectionSettings->writeParameter(mdt::ConnectionSettings::User, ui->LineEdit_user->text());
    if (ui->CheckBox_savePassword->isChecked()) {
        m_connectionSettings->writeParameter(mdt::ConnectionSettings::Password, ui->LineEdit_password->text());
    }
    else {
        m_connectionSettings->writeParameter(mdt::ConnectionSettings::Password, QString());
    }
    m_connectionSettings->writeParameter(mdt::ConnectionSettings::SavePassword, ui->CheckBox_savePassword->isChecked());
    m_connectionSettings->writeParameter(mdt::ConnectionSettings::AutoConnect, ui->CheckBox_autoConnect->isChecked());

    m_settingsSaver->writeGroup(*m_connectionSettings);
}

void SettingsWindow::rec_saveParameters() {
    saveAppSettings();
    saveConnectionSettings();

    QDialog::close();
}
