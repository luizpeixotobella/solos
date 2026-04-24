import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import SolOS.Shell 1.0

Item {
    required property var activityFeedModel
    required property var approvalQueueModel
    required property var ghostRuntime
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

                SectionCard {
                    Layout.fillWidth: true
                    title: ghostRuntime.presenceLabel
                    subtitle: ghostRuntime.modeLabel
                    body: ghostRuntime.thesisLabel
                }

                RowLayout {
                    Layout.fillWidth: true

                    SectionCard {
                        Layout.fillWidth: true
                        title: "Approval Surface"
                        subtitle: "Pending system boundaries"
                        body: "Approvals should become a native, legible queue instead of random interruptions."
                    }

                    Button {
                        text: "Refresh runtime"
                        onClicked: appController.refreshRuntime()
                    }
                }

                GridLayout {
                    Layout.fillWidth: true
                    columns: width > 980 ? 2 : 1
                    columnSpacing: 16
                    rowSpacing: 16

                    Repeater {
                        model: approvalQueueModel

                        delegate: ApprovalItem {
                            required property string id
                            required property string title
                            required property string description
                            required property string requestedBy
                            required property string capability
                            required property string scope
                            required property string risk
                            required property string status
                            required property string createdAt

                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignTop
                            title: model.title
                            description: model.description
                            requestedBy: model.requestedBy
                            capability: model.capability
                            scope: model.scope
                            risk: model.risk
                            status: model.status
                            createdAt: model.createdAt
                        }
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 16

                    Repeater {
                        model: activityFeedModel

                        delegate: ActivityItem {
                            required property string title
                            required property string detail
                            required property string status

                            Layout.fillWidth: true
                            title: model.title
                            detail: model.detail
                            status: model.status
                        }
                    }
                }
            }
        }
    }
}
