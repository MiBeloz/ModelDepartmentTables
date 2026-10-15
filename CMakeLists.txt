cmake_minimum_required(VERSION 3.19)
project(ModelDepartmentTables VERSION 0.0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(
    APP_VERSION="${PROJECT_VERSION}"
    APP_NAME="${PROJECT_NAME}"
)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Widgets LinguistTools Sql Network Concurrent)

qt_standard_project_setup()

set(QT_NO_PRIVATE_MODULE_WARNING ON)
include(FetchContent)
FetchContent_Declare(
  QXlsx
  GIT_REPOSITORY https://github.com/QtExcel/QXlsx.git
  GIT_TAG        v1.5.1.1
  SOURCE_SUBDIR  QXlsx
  EXCLUDE_FROM_ALL
)
FetchContent_MakeAvailable(QXlsx)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(test)
endif()

add_executable(ModelDepartmentTables WIN32 MACOSX_BUNDLE
    src/main.cpp
)

add_subdirectory(src build/src)

target_link_libraries(ModelDepartmentTables PRIVATE
    ModelDepartmentTables_lib
    QXlsx::QXlsx)

qt_add_translations(
    TARGETS ModelDepartmentTables
    TS_FILES translations/ModelDepartmentTables_ru_RU.ts
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