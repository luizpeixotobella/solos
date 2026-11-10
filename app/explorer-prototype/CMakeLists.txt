cmake_minimum_required(VERSION 3.21)
project(SolOSExplorerPrototype VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 6.4 REQUIRED COMPONENTS Quick Qml Network)
qt_standard_project_setup(REQUIRES 6.4)

qt_add_executable(solos-explorer-prototype
    src/main.cpp
    src/explorercontroller.cpp
    src/explorercontroller.h
    src/runtimehealth.cpp
    src/runtimehealth.h
)

qt_add_qml_module(solos-explorer-prototype
    URI SolOS.ExplorerPrototype
    VERSION 1.0
    RESOURCE_PREFIX /qt/qml
    QML_FILES
        qml/Main.qml
)

target_link_libraries(solos-explorer-prototype PRIVATE Qt6::Quick Qt6::Qml Qt6::Network)

include(CTest)
if(BUILD_TESTING)
    find_package(Python3 REQUIRED COMPONENTS Interpreter)
    add_executable(runtime-health-smoke tests/runtime-health-smoke.cpp src/runtimehealth.cpp)
    target_include_directories(runtime-health-smoke PRIVATE src)
    target_link_libraries(runtime-health-smoke PRIVATE Qt6::Network)
    add_test(NAME runtime-health-framing COMMAND ${Python3_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/tests/runtime-health.py $<TARGET_FILE:runtime-health-smoke>)
endif()

install(TARGETS solos-explorer-prototype
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
