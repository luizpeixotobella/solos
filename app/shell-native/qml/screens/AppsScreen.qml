import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import SolOS.Shell 1.0

Item {
    required property var appRegistryModel
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

                RowLayout {
                    Layout.fillWidth: true

                    SectionCard {
                        Layout.fillWidth: true
                        title: "Apps"
                        subtitle: "Modules inside one environment"
                        body: "App registry, capability display, and launch flow should move behind a native app model and launcher bridge."
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
                        model: appRegistryModel

                        delegate: AppTile {
                            required property string name
                            required property string subtitle
                            required property string description

                            Layout.fillWidth: true
                            Layout.alignment: Qt.AlignTop
                            appName: name
                            appSubtitle: subtitle
                            appDescription: description
                        }
                    }
                }
            }
        }
    }
}
