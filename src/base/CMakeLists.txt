file(GLOB_RECURSE SRC_SOURCES "*.cpp")
file(GLOB_RECURSE SRC_HEADERS "*.h" "*.hpp")

list(FILTER SRC_SOURCES EXCLUDE REGEX "main\\.cpp$")

add_library(mdt_base STATIC ${SRC_SOURCES} ${SRC_HEADERS} ${UI_FILES})

target_include_directories(mdt_base
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/..
)

target_link_libraries(mdt_base
    PUBLIC
        Qt6::Core
        QXlsx::QXlsx
        mdt_common_cfg
)
