import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    color: "#121a2f"
    radius: 18
    border.color: "#263453"
    border.width: 1
    implicitHeight: content.implicitHeight + 36

    property string sessionLabel: ""
    property string systemLabel: ""
    property string walletLabel: ""
    property string agentStatus: ""
    property string hostRuntimeSummary: ""
    property bool online: false
    property int approvalsCount: 0
    property int notificationsCount: 0

    GridLayout {
        id: content
        anchors.fill: parent
        anchors.margins: 18
        columns: width > 1280 ? 4 : width > 900 ? 2 : 1
        rowSpacing: 12
        columnSpacing: 18

        ColumnLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
            spacing: 6

            Label {
                text: root.sessionLabel
                color: "#eef3ff"
                font.pixelSize: 18
                font.bold: true
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }

            Label {
                text: root.systemLabel
                color: "#9fb0d0"
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.minimumWidth: 240
            Layout.preferredWidth: 320
            radius: 14
            color: "#18233d"
            border.color: "#2a3a5d"
            border.width: 1
            implicitHeight: walletText.implicitHeight + 24

            Label {
                id: walletText
                anchors.fill: parent
                anchors.margins: 12
                text: root.walletLabel
                color: "#dfe8ff"
                wrapMode: Text.WordWrap
                verticalAlignment: Text.AlignVCenter
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.minimumWidth: 220
            Layout.preferredWidth: 280
            radius: 14
            color: "#1a1f3f"
            border.color: "#3b4580"
            border.width: 1
            implicitHeight: agentText.implicitHeight + 24

            Label {
                id: agentText
                anchors.fill: parent
                anchors.margins: 12
                text: root.agentStatus
                color: "#c7d7ff"
                wrapMode: Text.WordWrap
                verticalAlignment: Text.AlignVCenter
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.minimumWidth: 240
            Layout.preferredWidth: 340
            radius: 14
            color: "#142038"
            border.color: root.online ? "#2f8f6b" : "#6a4a4a"
            border.width: 1
            implicitHeight: runtimeColumn.implicitHeight + 24

            ColumnLayout {
                id: runtimeColumn
                anchors.fill: parent
                anchors.margins: 12
                spacing: 6

                RowLayout {
                    Layout.fillWidth: true

                    Label {
                        text: root.online ? "Runtime online" : "Runtime offline"
                        color: root.online ? "#8df0c2" : "#ffb1b1"
                        font.bold: true
                        Layout.fillWidth: true
                    }

                    Label {
                        text: root.approvalsCount + " approvals"
                        color: "#dfe8ff"
                    }
                }

                Label {
                    text: root.hostRuntimeSummary
                    color: "#9fb0d0"
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                }

                Label {
                    text: root.notificationsCount + " runtime events visible"
                    color: "#73d0ff"
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                }
            }
        }
    }
}
