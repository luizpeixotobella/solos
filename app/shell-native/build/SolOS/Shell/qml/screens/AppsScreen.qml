import QtQuick
import QtQuick.Layouts

import SolOS.Shell 1.0

Item {
    required property var appRegistryModel

    ColumnLayout {
        anchors.fill: parent
        spacing: 16

        SectionCard {
            Layout.fillWidth: true
            title: "Apps"
            subtitle: "Modules inside one environment"
            body: "App registry, capability display, and launch flow should move behind a native app model and launcher bridge."
        }

        Repeater {
            model: parent.parent.appRegistryModel

            delegate: AppTile {
                required property string name
                required property string subtitle
                required property string description

                Layout.fillWidth: true
                appName: name
                appSubtitle: subtitle
                appDescription: description
            }
        }
    }
}
