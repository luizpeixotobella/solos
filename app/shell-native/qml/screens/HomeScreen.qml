import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import SolOS.Shell 1.0

Item {
    required property var quickActionsModel
    required property var homeState
    required property string runtimeStatus
    required property string runtimeSource
    required property string lastRuntimeRefresh
    required property var appController

    ScrollView {
        anchors.fill: parent
        clip: true

        Item {
            width: parent.width
            implicitHeight: content.implicitHeight + 24

            ColumnLayout {
                id: content
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.top: parent.top
                anchors.margins: 8
                spacing: 16

                GridLayout {
                    Layout.fillWidth: true
                    columns: width > 980 ? 2 : 1
                    columnSpacing: 16
                    rowSpacing: 16

                    SectionCard {
                        Layout.fillWidth: true
                        title: homeState.summaryTitle
                        subtitle: homeState.summarySubtitle
                        body: homeState.summaryBody
                    }

                    SectionCard {
                        Layout.fillWidth: true
                        title: "Runtime pulse"
                        subtitle: runtimeStatus
                        body: "Source: " + runtimeSource + "\nLast refresh: " + lastRuntimeRefresh + "\n\nEdit the runtime snapshot or re-run the Rust generator and the shell should visibly refresh."
                    }
                }

                Button {
                    text: "Refresh runtime now"
                    Layout.alignment: Qt.AlignLeft
                    onClicked: appController.refreshRuntime()
                }

                SectionCard {
                    Layout.fillWidth: true
                    title: homeState.nextActionTitle
                    subtitle: homeState.nextActionSubtitle
                    body: homeState.nextActionBody
                }

                GridLayout {
                    Layout.fillWidth: true
                    columns: width > 980 ? 2 : 1
                    columnSpacing: 16
                    rowSpacing: 16

                    Repeater {
                        model: quickActionsModel

                        delegate: QuickActionTile {
                            required property string title
                            required property string subtitle
                            required property string description

                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignTop
                            actionTitle: model.title
                            actionSubtitle: model.subtitle
                            actionDescription: model.description
                        }
                    }
                }

                SectionCard {
                    Layout.fillWidth: true
                    title: "Visible progress rule"
                    subtitle: "Structure must produce perceivable movement"
                    body: "Each implementation pass should improve architecture and also create a useful visible change in the shell. If users cannot feel the progress, the system is still too inert."
                }
            }
        }
    }
}
