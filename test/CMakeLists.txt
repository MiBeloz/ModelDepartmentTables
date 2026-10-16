find_package(Qt6 REQUIRED COMPONENTS Test Concurrent)

set(CMAKE_AUTOMOC ON)

enable_testing()

add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running tests"
)

include_directories("../src")

file(GLOB test_Files "test_*.cpp")

foreach(test_File ${test_Files})
    get_filename_component(test_Filename "${test_File}" NAME_WLE)
    
    add_executable("${test_Filename}" "${test_File}")
    target_link_libraries("${test_Filename}" 
        PRIVATE
            Qt6::Test
            Qt6::Concurrent
            mdt_base
    )
    add_test(NAME "${test_Filename}" COMMAND "${test_Filename}")
    
    add_dependencies(check "${test_Filename}")
endforeach()