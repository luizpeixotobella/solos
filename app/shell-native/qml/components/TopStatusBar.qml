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

    GridLayout {
        id: content
        anchors.fill: parent
        anchors.margins: 18
        columns: width > 1100 ? 3 : 1
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
    }
}
