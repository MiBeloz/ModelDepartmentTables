#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

using namespace Qt::Literals::StringLiterals;

namespace mdt {
    namespace AppSettings {
        const QString SettingFilename(u"settings.ini"_s);
        const QString GroupName(u"ApplicationSettings"_s);
        const QString Language(u"Language"_s);
        const QString LanguageDefaultValue(u"auto"_s);
        const QString Width(u"Width"_s);
        const int WidthDefaultValue = 800;
        const QString Height(u"Height"_s);
        const int HeightDefaultValue = 600;
        const QString RestoreMainWindow(u"RestoreMainWindow"_s);
        const bool RestoreMainWindowDefaultValue = false;
        const QString DateFormat(u"DateFormat"_s);
        const QString DateFormatDefaultValue(u"dd.MM.yyyy"_s);
    } // namespace AppSettings
    namespace SaverSettings {
        const QString SAVER_STORAGE_FILENAME(u"storage.mdt"_s);
        const QString SAVER_STORAGE_FILENAME_TMP(u"storage.tmp"_s);
    } // namespace SaverSettings
} // namespace mdt

#endif // CONSTANTS_H
