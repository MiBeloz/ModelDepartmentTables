#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace mdt {

#define STRINGIZE(x) #x
#define STRINGIZE_VALUE_OF(x) STRINGIZE(x)
namespace AppInfo {
constexpr const char* Name = STRINGIZE_VALUE_OF(APP_NAME);
constexpr const char* Version = STRINGIZE_VALUE_OF(APP_VERSION);
}
#undef STRINGIZE
#undef STRINGIZE_VALUE_OF

namespace AppSettings {
constexpr const char* SettingFilename = "settings.ini";
constexpr const char* GroupName = "ApplicationSettings";
constexpr const char* Language = "Language";
constexpr const char* LanguageDefaultValue = "auto";
constexpr const char* Width = "Width";
constexpr const int   WidthDefaultValue = 800;
constexpr const char* Height = "Height";
constexpr const int   HeightDefaultValue = 600;
constexpr const char* RestoreMainWindow = "RestoreMainWindow";
constexpr const bool  RestoreMainWindowDefaultValue = false;
constexpr const char* DateFormat = "DateFormat";
constexpr const char* DateFormatDefaultValue = "dd.MM.yyyy";
}

namespace ConnectionSettings {
constexpr const char* GroupName = "ConnectionSettings";
constexpr const char* Host = "Host";
constexpr const char* HostDefaultValue = "127.0.0.1";
constexpr const char* Port = "Port";
constexpr const int   PortDefaultValue = 5432;
constexpr const char* DatabaseName = "DatabaseName";
constexpr const char* DatabaseNameDefaultValue = "mydb";
constexpr const char* User = "User";
constexpr const char* UserDefaultValue = "postgres";
constexpr const char* Password = "Password";
constexpr const char* PasswordDefaultValue = "rcmiha57";
constexpr const char* SavePassword = "SavePassword";
constexpr const bool  SavePasswordDefaultValue = true;
constexpr const char* AutoConnect = "AutoConnect";
constexpr const bool  AutoConnectDefaultValue = true;
}

}

#endif // CONSTANTS_H
