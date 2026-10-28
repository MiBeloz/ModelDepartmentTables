cmake_minimum_required(VERSION 3.19)
project(ModelDepartmentTables
  LANGUAGES CXX)

set(BUILD_TESTING ON)

set(minQt6Version 6.11.1)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/Modules)

include(CheckPackages)
include(CommonConfig)

add_subdirectory(src)
add_subdirectory(
  ${CMAKE_SOURCE_DIR}/src/3rdparty/QXlsx/QXlsx
  ${CMAKE_BINARY_DIR}/QXlsx_build
)

if(BUILD_TESTING)
    add_subdirectory(test)
endif()

