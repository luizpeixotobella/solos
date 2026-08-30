import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import SolOS.Shell 1.0

Rectangle {
    id: root
    color: Theme.panelMuted
    radius: 16
    border.color: Theme.borderMuted
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
                color: Theme.textStrong
                font.bold: true
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }

            Rectangle {
                radius: 10
                color: root.status === "active" ? Theme.panelOnline
                     : root.status === "ready" ? Theme.panelReady
                     : Theme.panelWaiting
                implicitHeight: 24
                implicitWidth: statusLabel.implicitWidth + 18

                Label {
                    id: statusLabel
                    anchors.centerIn: parent
                    text: root.status
                    color: Theme.textBright
                    font.pixelSize: 12
                }
            }
        }

        Label {
            text: root.detail
            color: Theme.textSoft
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }
    }
}
