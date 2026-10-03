cmake_minimum_required(VERSION 3.21)
project(SolOSExplorerPrototype VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick Qml)
qt_standard_project_setup(REQUIRES 6.4)

qt_add_executable(solos-explorer-prototype
    src/main.cpp
    src/explorercontroller.cpp
    src/explorercontroller.h
)

qt_add_qml_module(solos-explorer-prototype
    URI SolOS.ExplorerPrototype
    VERSION 1.0
    QML_FILES
        qml/Main.qml
)

target_link_libraries(solos-explorer-prototype PRIVATE Qt6::Quick Qt6::Qml)

install(TARGETS solos-explorer-prototype
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
