import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import SolOS.Shell 1.0

Rectangle {
    id: root
    color: Theme.sidebar

    property string currentScreen: "Home"
    signal screenSelected(string screen)

    ColumnLayout {
        anchors.fill: parent
        anchors.margins: 20
        spacing: 18

        Label {
            text: "SolOS"
            color: Theme.textStrong
            font.pixelSize: 28
            font.bold: true
        }

        Label {
            text: "native shell"
            color: Theme.textSidebar
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
            color: Theme.panelMuted
            border.color: Theme.borderMuted
            border.width: 1
            implicitHeight: statusColumn.implicitHeight + 24

            Column {
                id: statusColumn
                anchors.fill: parent
                anchors.margins: 12
                spacing: 6

                Label {
                    text: "Ghost"
                    color: Theme.textStrong
                    font.bold: true
                }
                Label {
                    text: "Active · approval-aware"
                    color: Theme.textSoft
                    wrapMode: Text.Wrap
                }
            }
        }
    }
}
