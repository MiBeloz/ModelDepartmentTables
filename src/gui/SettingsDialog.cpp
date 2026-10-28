#include "SettingsDialog.h"

#include "Constants.h"
#include "ui_SettingsDialog.h"

using namespace Qt::Literals::StringLiterals;

SettingsDialog::SettingsDialog(QWidget *parent) : QDialog(parent), m_ui(new Ui::SettingsDialog()) {
    m_ui->setupUi(this);

    m_settingsSaver = new SettingsSaver(mdt::AppSettings::SettingFilename, this);
    m_appSettings = new SettingsGroup(mdt::AppSettings::GroupName, this);

    init();
}

SettingsDialog::~SettingsDialog() {
    delete m_ui;
}

void SettingsDialog::open() {
    fillAppSettingsTab();

    m_ui->LineEdit_password->setEchoMode(QLineEdit::EchoMode::Password);
    m_ui->PushButton_showPassword->setIcon(QIcon(u"icon_eye.png"_s));

    QDialog::open();
}

void SettingsDialog::init() {
    initLanguages();
    initAppSettings();

    connect(m_ui->PushButton_save, &QPushButton::clicked, this, &SettingsDialog::rec_saveParameters);
    connect(m_ui->PushButton_cancel, &QPushButton::clicked, this, [this] { QDialog::close(); });
    connect(m_ui->PushButton_showPassword, &QPushButton::clicked, this, [this] {
        if (m_ui->LineEdit_password->echoMode() == QLineEdit::EchoMode::Normal) {
            m_ui->LineEdit_password->setEchoMode(QLineEdit::EchoMode::Password);
            m_ui->PushButton_showPassword->setIcon(QIcon(u"icon_eye.png"_s));
        } else {
            m_ui->LineEdit_password->setEchoMode(QLineEdit::EchoMode::Normal);
            m_ui->PushButton_showPassword->setIcon(QIcon(u"icon_hide.png"_s));
        }
    });
}

void SettingsDialog::initLanguages() {
    Language newLang;
    newLang.name = u"Automatic language recognition"_s;
    newLang.code = u"auto"_s;
    m_languages.append(newLang);

    // newLang.name = "English";
    // newLang.code = "en";
    // m_languages.append(newLang);

    newLang.name = QString::fromUtf8("Русский");
    newLang.code = u"ru"_s;
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

void SettingsDialog::rec_saveParameters() {
    saveAppSettings();

    QDialog::close();
}
