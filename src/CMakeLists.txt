file(GLOB_RECURSE SRC_SOURCES "*.cpp")
file(GLOB_RECURSE SRC_HEADERS "*.h" "*.hpp")
file(GLOB_RECURSE UI_FILES "*.ui")

list(FILTER SRC_SOURCES EXCLUDE REGEX "main\\.cpp$")

add_library(ModelDepartmentTables_lib STATIC ${SRC_SOURCES} ${SRC_HEADERS} ${UI_FILES})
qt6_add_ui(ModelDepartmentTables_lib SOURCES MainWindow.ui NewItemWindow.ui SettingsWindow.ui)

target_include_directories(ModelDepartmentTables_lib 
    PUBLIC 
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(ModelDepartmentTables_lib
    PUBLIC
        Qt6::Core
        Qt6::Gui
        Qt6::Widgets
        Qt6::Sql
        Qt6::Network
)

qt_wrap_ui(ModelDepartmentTables_lib ${UI_FILES})