find_package(Qt6 REQUIRED COMPONENTS Test)

#file(GLOB test_Files "test_*.cpp")

enable_testing()

add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running tests"
)

set(test_Files
    test_DatesList.cpp
    test_DrawingsList.cpp
)

foreach(test_File ${test_Files})
    get_filename_component(test_Filename "${test_File}" NAME_WLE)
    
    add_executable("${test_Filename}" "${test_File}")
    
    target_include_directories("${test_Filename}" 
        PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/../src
    )
    
    target_link_libraries("${test_Filename}" 
        PRIVATE
            ModelDepartmentTables_lib
            Qt6::Test
    )
    
    add_test(NAME "${test_Filename}" COMMAND "${test_Filename}")
    
    add_dependencies(check "${test_Filename}")
endforeach()