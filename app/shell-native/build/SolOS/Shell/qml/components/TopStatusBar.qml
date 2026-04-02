import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    color: "#121a2f"
    radius: 18
    border.color: "#263453"
    border.width: 1
    implicitHeight: 88

    property string sessionLabel: ""
    property string systemLabel: ""
    property string walletLabel: ""
    property string agentStatus: ""

    RowLayout {
        anchors.fill: parent
        anchors.margins: 18
        spacing: 18

        ColumnLayout {
            Layout.fillWidth: true
            spacing: 6
            Label {
                text: root.sessionLabel
                color: "#eef3ff"
                font.pixelSize: 18
                font.bold: true
            }
            Label {
                text: root.systemLabel
                color: "#9fb0d0"
            }
        }

        Rectangle {
            radius: 14
            color: "#18233d"
            border.color: "#2a3a5d"
            border.width: 1
            implicitWidth: 300
            implicitHeight: 52

            Label {
                anchors.centerIn: parent
                text: root.walletLabel
                color: "#dfe8ff"
            }
        }

        Rectangle {
            radius: 14
            color: "#1a1f3f"
            border.color: "#3b4580"
            border.width: 1
            implicitWidth: 240
            implicitHeight: 52

            Label {
                anchors.centerIn: parent
                text: root.agentStatus
                color: "#c7d7ff"
            }
        }
    }
}
