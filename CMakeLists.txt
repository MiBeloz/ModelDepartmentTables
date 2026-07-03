cmake_minimum_required(VERSION 3.19)
project(ModelDepartmentTables LANGUAGES CXX)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 1)
set(PROJECT_VERSION "${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(
    APP_VERSION=${CMAKE_PROJECT_VERSION}
    APP_NAME=${CMAKE_PROJECT_NAME}
)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Widgets LinguistTools Sql Network)

qt_standard_project_setup()

qt_add_executable(ModelDepartmentTables
    WIN32 MACOSX_BUNDLE
    main.cpp
    ItemsModel.h ItemsModel.cpp
    TableHeaders.h
    MainWindow.h MainWindow.cpp MainWindow.ui
    NewItemWindow.h NewItemWindow.cpp NewItemWindow.ui
    ItemData.h
    CacheLocker.hpp
    CacheManager.hpp
    DatabaseManager.h DatabaseManager.cpp
    SettingsWindow.h SettingsWindow.cpp SettingsWindow.ui
    SettingsSaver.h SettingsSaver.cpp
    Constants.h
    SettingsGroup.h SettingsGroup.cpp
)

qt_add_translations(
    TARGETS ModelDepartmentTables
    TS_FILES ModelDepartmentTables_ru_RU.ts
)

target_link_libraries(ModelDepartmentTables
    PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::Sql
        Qt::Network
)

include(GNUInstallDirs)

install(TARGETS ModelDepartmentTables
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET ModelDepartmentTables
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
