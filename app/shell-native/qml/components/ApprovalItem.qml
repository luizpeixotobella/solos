import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    color: "#1a1f3f"
    radius: 16
    border.color: "#3b4580"
    border.width: 1
    implicitHeight: content.implicitHeight + 28

    property string title: ""
    property string description: ""
    property string requestedBy: ""
    property string capability: ""
    property string scope: ""
    property string risk: ""
    property string status: ""
    property string createdAt: ""

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

            Label {
                text: root.risk
                color: root.risk === "low" ? "#7ee2b8" : root.risk === "medium" ? "#ffd479" : "#ff9f9f"
                font.bold: true
            }
        }

        Label {
            text: root.description
            color: "#d9e4ff"
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }

        Label {
            text: "Scope: " + root.scope
            color: "#9fb0d0"
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }

        Label {
            text: "Requested by: " + root.requestedBy + " · capability: " + root.capability
            color: "#8fb2ff"
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }

        Label {
            text: "Status: " + root.status + " · created: " + root.createdAt
            color: "#73d0ff"
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }
    }
}
