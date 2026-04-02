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
    property string scope: ""
    property string risk: ""

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
            text: root.scope
            color: "#9fb0d0"
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }
    }
}
