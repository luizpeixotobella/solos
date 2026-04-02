import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

Rectangle {
    id: root
    color: "#10182b"

    property string currentScreen: "Home"
    signal screenSelected(string screen)

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 18

        Label {
            text: "SolOS"
            color: "#eef3ff"
            font.pixelSize: 28
            font.bold: true
        }

        Label {
            text: "native shell"
            color: "#8aa0c8"
            font.pixelSize: 13
        }

        Repeater {
            model: ["Home", "Agent", "Wallet", "Apps"]

            delegate: Button {
                required property string modelData
                Layout.fillWidth: true
                text: modelData
                highlighted: root.currentScreen === modelData
                onClicked: root.screenSelected(modelData)
            }
        }

        Item { Layout.fillHeight: true }

        Rectangle {
            Layout.fillWidth: true
            radius: 14
            color: "#17213a"
            border.color: "#2a3a5d"
            border.width: 1
            implicitHeight: statusColumn.implicitHeight + 24

            Column {
                id: statusColumn
                anchors.fill: parent
                anchors.margins: 12
                spacing: 6

                Label {
                    text: "Ghost"
                    color: "#eef3ff"
                    font.bold: true
                }
                Label {
                    text: "Active · approval-aware"
                    color: "#9fb0d0"
                    wrapMode: Text.Wrap
                }
            }
        }
    }
}
