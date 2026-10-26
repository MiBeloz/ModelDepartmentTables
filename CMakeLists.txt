cmake_minimum_required(VERSION 3.19)
project(ModelDepartmentTables
  LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(BUILD_TESTING ON)

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

add_subdirectory(src)

if(BUILD_TESTING)
    add_subdirectory(test)
endif()

