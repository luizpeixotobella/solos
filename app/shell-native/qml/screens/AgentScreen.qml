import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import SolOS.Shell 1.0

Item {
    required property var activityFeedModel
    required property var approvalQueueModel
    required property var ghostRuntime
    required property var appController

    property string braveApiKeyInput: ""

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
                    body: ghostRuntime.thesisLabel + "\n\n" + ghostRuntime.intelligenceSummary + "\n" + ghostRuntime.webStatusLabel
                }

                RowLayout {
                    Layout.fillWidth: true

                    SectionCard {
                        Layout.fillWidth: true
                        title: "Ghost research"
                        subtitle: ghostRuntime.researchQuery.length > 0 ? ghostRuntime.researchQuery : "No query yet"
                        body: ghostRuntime.researchSummary
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

                    Rectangle {
                        Layout.fillWidth: true
                        radius: 20
                        color: "#121a2f"
                        border.color: "#263453"
                        border.width: 1
                        implicitHeight: onboardingColumn.implicitHeight + 36

                        ColumnLayout {
                            id: onboardingColumn
                            anchors.fill: parent
                            anchors.margins: 18
                            spacing: 10

                            Label {
                                text: ghostRuntime.onboardingTitle
                                color: "#eef3ff"
                                font.pixelSize: 20
                                font.bold: true
                                Layout.fillWidth: true
                                wrapMode: Text.WordWrap
                            }

                            Label {
                                text: ghostRuntime.onboardingStatus
                                color: "#73d0ff"
                                Layout.fillWidth: true
                                wrapMode: Text.WordWrap
                            }

                            Label {
                                text: ghostRuntime.onboardingBody + "\n\nOpen: " + ghostRuntime.onboardingUrl
                                color: "#9fb0d0"
                                Layout.fillWidth: true
                                wrapMode: Text.WordWrap
                            }

                            TextField {
                                Layout.fillWidth: true
                                placeholderText: "Paste the user's Brave API key here"
                                text: braveApiKeyInput
                                echoMode: TextInput.Password
                                onTextChanged: braveApiKeyInput = text
                            }

                            RowLayout {
                                Layout.fillWidth: true

                                Button {
                                    text: "Open Brave key page"
                                    onClicked: appController.openUrl(ghostRuntime.onboardingUrl)
                                }

                                Button {
                                    text: "Validate and save Brave key"
                                    onClicked: appController.validateAndSaveGhostBraveApiKey(braveApiKeyInput)
                                }

                                Button {
                                    text: "Clear key"
                                    onClicked: {
                                        braveApiKeyInput = ""
                                        appController.clearGhostBraveApiKey()
                                    }
                                }
                            }

                            Label {
                                text: appController.ghostConfigStatus
                                color: appController.ghostConfigStatus.indexOf("not configured") >= 0 ? "#ffd479" : "#8df0c2"
                                Layout.fillWidth: true
                                wrapMode: Text.WordWrap
                            }
                        }
                    }

                    SectionCard {
                        Layout.fillWidth: true
                        title: "Safe key policy"
                        subtitle: "Per-user Brave subscription"
                        body: "Do not ship the developer key in public builds. Each SolOS user should obtain their own Brave key, then return and complete repo-local configuration so usage stays isolated."
                    }
                }

                GridLayout {
                    Layout.fillWidth: true
                    columns: width > 980 ? 2 : 1
                    columnSpacing: 16
                    rowSpacing: 16

                    SectionCard {
                        Layout.fillWidth: true
                        title: "Pipeline layers"
                        subtitle: "Data -> Results -> Algorithms"
                        body: ghostRuntime.pipelineLines.length > 0 ? ghostRuntime.pipelineLines.join("\n\n") : "No pipeline detail yet."
                    }

                    SectionCard {
                        Layout.fillWidth: true
                        title: "Web citations"
                        subtitle: ghostRuntime.citationLines.length > 0 ? "Brave-grounded references" : "No citations yet"
                        body: ghostRuntime.citationLines.length > 0 ? ghostRuntime.citationLines.join("\n\n") : "Ghost will show sourced web references here when Brave research succeeds."
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
