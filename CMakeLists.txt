cmake_minimum_required(VERSION 3.19)
project(ModelDepartmentTables LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets LinguistTools)

qt_standard_project_setup()

qt_add_executable(ModelDepartmentTables
    WIN32 MACOSX_BUNDLE
    main.cpp
    MainWindow.cpp
    MainWindow.h
    MainWindow.ui
    ItemsModel.h ItemsModel.cpp
    TableHeaders.h
)

qt_add_translations(
    TARGETS ModelDepartmentTables
    TS_FILES ModelDepartmentTables_ru_RU.ts
)

target_link_libraries(ModelDepartmentTables
    PRIVATE
        Qt::Core
        Qt::Widgets
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
