import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    color: "#121a2f"
    radius: 20
    border.color: "#263453"
    border.width: 1
    implicitHeight: contentColumn.implicitHeight + 36

    property string title: ""
    property string subtitle: ""
    property string body: ""

    Layout.fillWidth: true

    ColumnLayout {
        id: contentColumn
        anchors.fill: parent
        anchors.margins: 18
        spacing: 10

        Label {
            text: root.title
            color: "#eef3ff"
            font.pixelSize: 20
            font.bold: true
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }

        Label {
            text: root.subtitle
            color: "#73d0ff"
            font.pixelSize: 13
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }

        Label {
            text: root.body
            color: "#9fb0d0"
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }
    }
}
