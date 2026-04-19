cmake_minimum_required(VERSION 3.21)
project(SolOSShellNative VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Quick Qml)

qt_standard_project_setup(REQUIRES 6.5)

qt_add_executable(solos-shell-native
    src/main.cpp
    src/appcontroller.cpp
    src/appcontroller.h
    src/appregistrymodel.cpp
    src/appregistrymodel.h
    src/activityfeedmodel.cpp
    src/activityfeedmodel.h
    src/quickactionsmodel.cpp
    src/quickactionsmodel.h
    src/approvalqueuemodel.cpp
    src/approvalqueuemodel.h
    src/ghostruntime.cpp
    src/ghostruntime.h
    src/homestate.cpp
    src/homestate.h
    src/runtimebridge.cpp
    src/runtimebridge.h
)

qt_add_qml_module(solos-shell-native
    URI SolOS.Shell
    VERSION 1.0
    QML_FILES
        qml/Main.qml
        qml/components/SectionCard.qml
        qml/components/SidebarNav.qml
        qml/components/TopStatusBar.qml
        qml/components/AppTile.qml
        qml/components/ActivityItem.qml
        qml/components/ApprovalItem.qml
        qml/components/QuickActionTile.qml
        qml/screens/HomeScreen.qml
        qml/screens/AgentScreen.qml
        qml/screens/WalletScreen.qml
        qml/screens/AppsScreen.qml
)

target_link_libraries(solos-shell-native
    PRIVATE Qt6::Quick Qt6::Qml
)

set_target_properties(solos-shell-native PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)

install(TARGETS solos-shell-native
    BUNDLE DESTINATION .
    RUNTIME DESTINATION bin
)
