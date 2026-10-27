find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Widgets LinguistTools Sql Network Concurrent)

qt_standard_project_setup()

file(GLOB_RECURSE SRC_SOURCES "*.cpp")
file(GLOB_RECURSE SRC_HEADERS "*.h" "*.hpp")
file(GLOB_RECURSE UI_FILES "*.ui")

list(FILTER SRC_SOURCES EXCLUDE REGEX "main\\.cpp$")

add_library(mdt_gui STATIC ${SRC_SOURCES} ${SRC_HEADERS} ${UI_FILES})
qt6_add_ui(mdt_gui SOURCES MainWindow.ui AboutDialog.ui NewItemDialog.ui SettingsDialog.ui)

target_include_directories(mdt_gui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(mdt_gui
    PUBLIC
        Qt6::Gui
        Qt6::Widgets
        Qt6::Sql
        Qt6::Network
        Qt6::Concurrent
        mdt_base
)

qt_wrap_ui(mdt_storage ${UI_FILES})
