import QtQuick
import QtQuick.Layouts

import SolOS.Shell 1.0

Item {
    required property var quickActionsModel
    required property var homeState

    ColumnLayout {
        anchors.fill: parent
        spacing: 16

        SectionCard {
            Layout.fillWidth: true
            title: homeState.summaryTitle
            subtitle: homeState.summarySubtitle
            body: homeState.summaryBody
        }

        SectionCard {
            Layout.fillWidth: true
            title: homeState.nextActionTitle
            subtitle: homeState.nextActionSubtitle
            body: homeState.nextActionBody
        }

        Repeater {
            model: parent.parent.quickActionsModel

            delegate: QuickActionTile {
                required property string title
                required property string subtitle
                required property string description

                Layout.fillWidth: true
                actionTitle: model.title
                actionSubtitle: model.subtitle
                actionDescription: model.description
            }
        }

        SectionCard {
            Layout.fillWidth: true
            title: "Migration note"
            subtitle: "Web shell as reference"
            body: "The React prototype defines UX semantics. Qt/QML should inherit the behavior, not the browser assumptions."
        }
    }
}
