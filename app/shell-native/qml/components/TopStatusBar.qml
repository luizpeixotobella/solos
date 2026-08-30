import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import SolOS.Shell 1.0

Rectangle {
    id: root
    color: Theme.surface
    radius: 18
    border.color: Theme.border
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
        anchors.margins: Theme.cardPadding
        columns: width > 1280 ? 4 : width > 900 ? 2 : 1
        rowSpacing: 12
        columnSpacing: 18

        ColumnLayout {
            Layout.fillWidth: true
            Layout.alignment: Qt.AlignTop
            spacing: 6

            Label {
                text: root.sessionLabel
                color: Theme.textStrong
                font.pixelSize: 18
                font.bold: true
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }

            Label {
                text: root.systemLabel
                color: Theme.textSoft
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.minimumWidth: 240
            Layout.preferredWidth: 320
            radius: 14
            color: Theme.surfaceRaised
            border.color: Theme.borderMuted
            border.width: 1
            implicitHeight: walletText.implicitHeight + 24

            Label {
                id: walletText
                anchors.fill: parent
                anchors.margins: 12
                text: root.walletLabel
                color: Theme.textBright
                wrapMode: Text.WordWrap
                verticalAlignment: Text.AlignVCenter
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.minimumWidth: 220
            Layout.preferredWidth: 280
            radius: 14
            color: Theme.panelAlt
            border.color: Theme.borderStrong
            border.width: 1
            implicitHeight: agentText.implicitHeight + 24

            Label {
                id: agentText
                anchors.fill: parent
                anchors.margins: 12
                text: root.agentStatus
                color: Theme.textBlue
                wrapMode: Text.WordWrap
                verticalAlignment: Text.AlignVCenter
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.minimumWidth: 240
            Layout.preferredWidth: 340
            radius: 14
            color: Theme.networkPanel
            border.color: root.online ? Theme.onlineBorder : Theme.offlineBorder
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
                        color: root.online ? Theme.successText : Theme.offlineText
                        font.bold: true
                        Layout.fillWidth: true
                    }

                    Label {
                        text: root.approvalsCount + " approvals"
                        color: Theme.textBright
                    }
                }

                Label {
                    text: root.hostRuntimeSummary
                    color: Theme.textSoft
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                }

                Label {
                    text: root.notificationsCount + " runtime events visible"
                    color: Theme.accent
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                }
            }
        }
    }
}
