set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)

set(QT_NO_PRIVATE_MODULE_WARNING ON)
find_package(Qt6 ${minQt6Version} REQUIRED COMPONENTS Core Gui GuiPrivate Widgets LinguistTools Concurrent)
