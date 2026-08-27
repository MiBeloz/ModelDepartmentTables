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

add_subdirectory(src)
add_subdirectory(test)

add_executable(ModelDepartmentTables WIN32 MACOSX_BUNDLE)
target_sources(ModelDepartmentTables PRIVATE src/main.cpp)
target_link_libraries(ModelDepartmentTables PRIVATE ModelDepartmentTables_lib)

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