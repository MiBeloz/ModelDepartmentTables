#include "SettingsWindow.h"
#include "ui_SettingsWindow.h"
#include "Constants.h"


SettingsWindow::SettingsWindow(QWidget *parent)
    : QDialog(parent)
    , ui(std::make_unique<Ui::SettingsWindow>())
{
    ui->setupUi(this);

    m_settingsSaver = new SettingsSaver(mdt::AppSettings::SettingFilename, this);
    m_appSettings = new SettingsGroup(mdt::AppSettings::GroupName, this);

    init();
}

void SettingsWindow::open() {
    int langIndex = 0;
    for (int i = 0; i < m_languages.count(); i++) {
        ui->CBOX_language->addItem(m_languages.at(i).name, m_languages.at(i).code);
        if (m_languages.at(i).code == m_appSettings->readParameter(mdt::AppSettings::Language).toString()) {
            langIndex = i;
        }
    }
    ui->CBOX_language->setCurrentIndex(langIndex);

    ui->SPBOX_mainWindowWidth->setValue(m_appSettings->readParameter(mdt::AppSettings::Width).toInt());
    ui->SPBOX_mainWindowHeight->setValue(m_appSettings->readParameter(mdt::AppSettings::Height).toInt());

    ui->CHBOX_restoreMainWindow->setChecked(m_appSettings->readParameter(mdt::AppSettings::RestoreMainWindow).toBool());

    ui->LEDIT_dateFormat->setText(m_appSettings->readParameter(mdt::AppSettings::DateFormat).toString());

    QDialog::open();
}

void SettingsWindow::init() {
    initLanguages();

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
    m_settingsManager->readGroup(*m_appSettings);

    connect(ui->PB_save, &QPushButton::clicked, this, &SettingsWindow::rec_saveParameters);
    connect(ui->PB_cancel, &QPushButton::clicked, this, [this] { QDialog::close(); });
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

void SettingsWindow::rec_saveParameters() {
    int width = m_appSettings->readParameter(mdt::AppSettings::Width).toInt();
    int height = m_appSettings->readParameter(mdt::AppSettings::Height).toInt();
    if (width != ui->SPBOX_mainWindowWidth->value() || height != ui->SPBOX_mainWindowHeight->value()) {
        emit sig_resizeMainWindow(ui->SPBOX_mainWindowWidth->value(), ui->SPBOX_mainWindowHeight->value());
    }
    m_appSettings->writeParameter(mdt::AppSettings::Language, ui->CBOX_language->currentData().toString());
    bool rw = ui->CHBOX_restoreMainWindow->isChecked();
    m_appSettings->writeParameter(mdt::AppSettings::RestoreMainWindow, rw);
    if (rw) {
        m_appSettings->writeParameter(mdt::AppSettings::Width, ui->SPBOX_mainWindowWidth->value());
        m_appSettings->writeParameter(mdt::AppSettings::Height, ui->SPBOX_mainWindowHeight->value());
    }
    else {
        m_appSettings->writeParameter(mdt::AppSettings::Width, mdt::AppSettings::WidthDefaultValue);
        m_appSettings->writeParameter(mdt::AppSettings::Height, mdt::AppSettings::HeightDefaultValue);
    }
    m_appSettings->writeParameter(mdt::AppSettings::DateFormat, ui->LEDIT_dateFormat->text());

    m_settingsManager->writeGroup(*m_appSettings);

    QDialog::close();
}
