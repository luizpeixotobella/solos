import QtQuick
import QtQuick.Layouts

import SolOS.Shell 1.0

Item {
    required property var activityFeedModel
    required property var approvalQueueModel
    required property var ghostRuntime

    ColumnLayout {
        anchors.fill: parent
        spacing: 16

        SectionCard {
            Layout.fillWidth: true
            title: ghostRuntime.presenceLabel
            subtitle: ghostRuntime.modeLabel
            body: ghostRuntime.thesisLabel
        }

        SectionCard {
            Layout.fillWidth: true
            title: "Approval Surface"
            subtitle: "Pending system boundaries"
            body: "Approvals should become a native, legible queue instead of random interruptions."
        }

        Repeater {
            model: parent.parent.approvalQueueModel

            delegate: ApprovalItem {
                required property string title
                required property string scope
                required property string risk

                Layout.fillWidth: true
                title: model.title
                scope: model.scope
                risk: model.risk
            }
        }

        Repeater {
            model: parent.parent.activityFeedModel

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
