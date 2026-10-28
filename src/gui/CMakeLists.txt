qt_standard_project_setup()

file(GLOB_RECURSE SRC_SOURCES "*.cpp")
file(GLOB_RECURSE SRC_HEADERS "*.h" "*.hpp")
file(GLOB_RECURSE UI_FILES "*.ui")

list(FILTER SRC_SOURCES EXCLUDE REGEX "main\\.cpp$")

add_library(mdt_gui STATIC ${SRC_SOURCES} ${SRC_HEADERS} ${UI_FILES})
qt6_add_ui(mdt_gui SOURCES MainWindow.ui AboutDialog.ui NewRecordDialog.ui SettingsDialog.ui)

target_include_directories(mdt_gui
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(mdt_gui
    PUBLIC
        Qt6::Gui
        Qt6::Widgets
        Qt6::Concurrent
        mdt_base
)

qt_wrap_ui(mdt_storage ${UI_FILES})
