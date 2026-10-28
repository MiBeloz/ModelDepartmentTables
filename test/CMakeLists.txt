find_package(Qt6 REQUIRED COMPONENTS Test Concurrent)

#set(CMAKE_AUTOMOC ON)

enable_testing(true)

add_custom_target(check
    COMMAND ${CMAKE_CTEST_COMMAND} --output-on-failure
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Running tests"
)

include_directories("../src")

set(testFiles
    test_CustomList.cpp
    test_DatesList.cpp
    test_Drawing.cpp
    test_RecordLink.cpp
    test_StorageRecordsLinks.cpp
)

set(testFilesWithConcurrent
    test_SaverFile.cpp
    test_StorageManager.cpp
)

set(testStats TestStats.h)

foreach(testFile ${testFiles})
    get_filename_component(testFilename "${testFile}" NAME_WLE)
    
    add_executable("${testFilename}" "${testFile}")
    target_link_libraries("${testFilename}"
        PRIVATE
            Qt6::Test
            mdt_base
    )
    add_test(NAME "${testFilename}" COMMAND "${testFilename}")
    
    add_dependencies(check "${testFilename}")
endforeach()

foreach(testFile ${testFilesWithConcurrent})
    get_filename_component(testFilename "${testFile}" NAME_WLE)

    add_executable("${testFilename}" "${testFile}" ${testStats})
    target_link_libraries("${testFilename}"
        PRIVATE
            Qt6::Test
            Qt6::Concurrent
            mdt_base
    )
    add_test(NAME "${testFilename}" COMMAND "${testFilename}")

    add_dependencies(check "${testFilename}")
endforeach()