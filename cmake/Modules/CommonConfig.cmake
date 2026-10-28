set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTORCC_OPTIONS --compress-algo best --threshold 5)

add_library(mdt_common_cfg INTERFACE)

target_compile_definitions(mdt_common_cfg INTERFACE QXLSX_VERSION_STRING="1.5.1")

target_compile_features(mdt_common_cfg INTERFACE
    cxx_std_17
)

target_compile_definitions(mdt_common_cfg INTERFACE
    QT_DISABLE_DEPRECATED_UP_TO=0x060500
    QT_NO_CAST_FROM_ASCII
    QT_NO_CAST_TO_ASCII
    QT_NO_CAST_FROM_BYTEARRAY
    QT_NO_CONTEXTLESS_CONNECT
    QT_NO_NARROWING_CONVERSIONS_IN_CONNECT
    QT_USE_QSTRINGBUILDER
    QT_STRICT_ITERATORS
    $<$<NOT:$<CONFIG:Debug>>:QT_NO_DEBUG_OUTPUT>
)

if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang"))
    target_compile_options(mdt_common_cfg INTERFACE
        -Wall
        -Wextra
        -Wcast-qual
        -Wcast-align
        -Winvalid-pch
        -pedantic
        -pedantic-errors
        $<$<COMPILE_LANGUAGE:CXX>:
            -Woverloaded-virtual
            -Wold-style-cast
            -Wnon-virtual-dtor>
    )
endif()