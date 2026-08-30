import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import SolOS.Shell 1.0

Rectangle {
    id: root
    signal launchRequested()
    property string appId: ""
    property string appName: ""
    property string appSubtitle: ""
    property string appDescription: ""

    property string appStatus: ""
    property string appCapability: ""
    radius: Theme.cardRadius
    color: Theme.surface
    border.color: Theme.border
    border.width: 1
    implicitHeight: tileColumn.implicitHeight + 36

    ColumnLayout {
        id: tileColumn
        anchors.fill: parent
        anchors.margins: Theme.cardPadding
        spacing: 9

        Label { text: appName; color: Theme.textStrong; font.pixelSize: 20; font.bold: true }
        Label { text: appSubtitle + " · " + appStatus; color: Theme.accent; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        Label { text: appDescription; color: Theme.textSoft; wrapMode: Text.WordWrap; Layout.fillWidth: true }
        Button {
            text: "Open via " + appCapability
            enabled: appId.length > 0 && appCapability === "app.open.safe"
            onClicked: root.launchRequested()
        }
    }
}
