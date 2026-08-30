import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import SolOS.Shell 1.0

Rectangle {
    id: root
    color: Theme.surface
    radius: Theme.cardRadius
    border.color: Theme.border
    border.width: 1
    implicitHeight: contentColumn.implicitHeight + 36

    property string title: ""
    property string subtitle: ""
    property string body: ""

    Layout.fillWidth: true

    ColumnLayout {
        id: contentColumn
        anchors.fill: parent
        anchors.margins: Theme.cardPadding
        spacing: 10

        Label {
            text: root.title
            color: Theme.textStrong
            font.pixelSize: 20
            font.bold: true
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }

        Label {
            text: root.subtitle
            color: Theme.accent
            font.pixelSize: 13
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }

        Label {
            text: root.body
            color: Theme.textSoft
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }
    }
}
