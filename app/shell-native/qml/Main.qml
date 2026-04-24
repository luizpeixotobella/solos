import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import SolOS.Shell 1.0

ApplicationWindow {
    id: root
    width: 1440
    height: 900
    visible: true
    title: "SolOS Native Shell"
    color: "#0b1020"

    property color bg0: "#0b1020"
    property color bg1: "#121a2f"
    property color bg2: "#18233d"
    property color line: "#263453"
    property color textStrong: "#eef3ff"
    property color textSoft: "#9fb0d0"
    property color accent: "#73d0ff"
    property color accent2: "#8b7dff"
    property var appController: globalAppController

    RowLayout {
        anchors.fill: parent
        spacing: 0

        SidebarNav {
            Layout.fillHeight: true
            Layout.preferredWidth: 220
            currentScreen: appController.currentScreen
            onScreenSelected: function(screen) { appController.currentScreen = screen }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 20

            TopStatusBar {
                Layout.fillWidth: true
                Layout.topMargin: 20
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                sessionLabel: appController.sessionLabel
                systemLabel: appController.systemLabel
                walletLabel: appController.walletLabel
                agentStatus: appController.agentStatus
                hostRuntimeSummary: appController.hostRuntimeSummary
                online: appController.online
                approvalsCount: appController.approvalsCount
                notificationsCount: appController.notificationsCount
            }

            StackLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                Layout.leftMargin: 20
                Layout.rightMargin: 20
                Layout.bottomMargin: 20
                currentIndex: {
                    if (appController.currentScreen === "Home") return 0
                    if (appController.currentScreen === "Agent") return 1
                    if (appController.currentScreen === "Wallet") return 2
                    return 3
                }

                HomeScreen {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    quickActionsModel: root.appController.quickActionsModel
                    homeState: root.appController.homeState
                    runtimeStatus: root.appController.runtimeStatus
                    runtimeSource: root.appController.runtimeSource
                    hostRuntimeSummary: root.appController.hostRuntimeSummary
                    online: root.appController.online
                    approvalsCount: root.appController.approvalsCount
                    notificationsCount: root.appController.notificationsCount
                    lastRuntimeRefresh: root.appController.lastRuntimeRefresh
                    appController: root.appController
                }

                AgentScreen {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    activityFeedModel: root.appController.activityFeedModel
                    approvalQueueModel: root.appController.approvalQueueModel
                    ghostRuntime: root.appController.ghostRuntime
                    appController: root.appController
                }

                WalletScreen {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                }

                AppsScreen {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    appRegistryModel: root.appController.appRegistryModel
                    appController: root.appController
                }
            }
        }
    }
}
