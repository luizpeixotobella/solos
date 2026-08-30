import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import SolOS.Shell 1.0

Rectangle {
    id: root
    color: Theme.panelAlt
    radius: 16
    border.color: Theme.borderStrong
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
                color: Theme.textStrong
                font.bold: true
                Layout.fillWidth: true
                wrapMode: Text.WordWrap
            }

            Label {
                text: root.risk
                color: root.risk === "low" ? Theme.safeText : root.risk === "medium" ? Theme.warning : Theme.dangerSoft
                font.bold: true
            }
        }

        Label {
            text: root.description
            color: Theme.textBodyAlt
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }

        Label {
            text: "Scope: " + root.scope
            color: Theme.textSoft
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }

        Label {
            text: "Requested by: " + root.requestedBy + " · capability: " + root.capability
            color: Theme.textLink
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }

        Label {
            text: "Status: " + root.status + " · created: " + root.createdAt
            color: Theme.accent
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }
    }
}
