import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    color: "#17213a"
    radius: 16
    border.color: "#2a3a5d"
    border.width: 1
    implicitHeight: content.implicitHeight + 28

    property string title: ""
    property string detail: ""
    property string status: ""

    Layout.fillWidth: true

    ColumnLayout {
        id: content
        anchors.fill: parent
        anchors.margins: 14
        spacing: 8

        RowLayout {
            Layout.fillWidth: true

            Label {
                text: root.title
                color: "#eef3ff"
                font.bold: true
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }

            Rectangle {
                radius: 10
                color: root.status === "active" ? "#143a2d"
                     : root.status === "ready" ? "#2a2c58"
                     : "#3a2b14"
                implicitHeight: 24
                implicitWidth: statusLabel.implicitWidth + 18

                Label {
                    id: statusLabel
                    anchors.centerIn: parent
                    text: root.status
                    color: "#dfe8ff"
                    font.pixelSize: 12
                }
            }
        }

        Label {
            text: root.detail
            color: "#9fb0d0"
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }
    }
}
