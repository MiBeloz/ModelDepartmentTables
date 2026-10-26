find_package(Qt6 6.5 REQUIRED COMPONENTS Core Gui Widgets LinguistTools Sql Network Concurrent)

qt_standard_project_setup()

add_executable(mdt_app WIN32 MACOSX_BUNDLE
    main.cpp
)

target_link_libraries(mdt_app PRIVATE
    Qt6::Gui
    Qt6::Widgets
    Qt6::Sql
    Qt6::Network
    Qt6::Concurrent
    mdt_gui
    QXlsx::QXlsx)

qt_add_translations(
    TARGETS mdt_app
    TS_FILES ../../translations/ModelDepartmentTables_ru_RU.ts
)

include(GNUInstallDirs)

install(TARGETS mdt_app
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET mdt_app
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
