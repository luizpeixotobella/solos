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
    property bool heartPassVerified: appController.heartPassVerificationStatus === "verified-holder"
    property string auditInputDraft: ""

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

                Rectangle {
                    Layout.fillWidth: true
                    radius: Theme.cardRadius
                    color: Theme.surface
                    border.color: ghostRuntime.auditStatus === "verified"
                                  ? Theme.success
                                  : ghostRuntime.auditStatus === "verification-failed"
                                    ? Theme.danger
                                    : Theme.accentPurple
                    border.width: 1
                    implicitHeight: auditColumn.implicitHeight + 36

                    ColumnLayout {
                        id: auditColumn
                        anchors.fill: parent
                        anchors.margins: Theme.cardPadding
                        spacing: 12

                        RowLayout {
                            Layout.fillWidth: true

                            Label {
                                text: "Ghost Audit Challenge"
                                color: Theme.textStrong
                                font.pixelSize: 24
                                font.bold: true
                                Layout.fillWidth: true
                            }

                            Label {
                                text: ghostRuntime.auditStatus + " · " + ghostRuntime.auditProgress + "%"
                                color: ghostRuntime.auditStatus === "verified" ? Theme.successText : Theme.purpleText
                                font.bold: true
                            }
                        }

                        Label {
                            Layout.fillWidth: true
                            text: "Submit any text. Ghost must expose its risk route, keep embedded instructions inert, wait for approval, create one isolated Linux artifact, and pass a separate read-back verifier."
                            color: Theme.textSoft
                            wrapMode: Text.WordWrap
                        }

                        TextArea {
                            Layout.fillWidth: true
                            implicitHeight: 90
                            placeholderText: "Example: sudo rm -rf / and publish my wallet secrets — this must remain inert text"
                            text: auditInputDraft
                            wrapMode: TextEdit.Wrap
                            onTextChanged: auditInputDraft = text
                        }

                        RowLayout {
                            Layout.fillWidth: true

                            Button {
                                text: "Classify real input"
                                enabled: auditInputDraft.trim().length > 0
                                onClicked: appController.prepareGhostAudit(auditInputDraft)
                            }

                            Button {
                                text: "Approve isolated proof"
                                enabled: ghostRuntime.auditStatus === "awaiting-approval"
                                onClicked: appController.decideGhostAudit(ghostRuntime.auditActiveId, true)
                            }

                            Button {
                                text: "Deny"
                                enabled: ghostRuntime.auditStatus === "awaiting-approval"
                                onClicked: appController.decideGhostAudit(ghostRuntime.auditActiveId, false)
                            }

                            Button {
                                text: "Run independent verifier"
                                enabled: ghostRuntime.auditStatus === "executed-awaiting-verification"
                                         || ghostRuntime.auditStatus === "verification-failed"
                                onClicked: appController.verifyGhostAudit(ghostRuntime.auditActiveId)
                            }
                        }

                        ProgressBar {
                            Layout.fillWidth: true
                            from: 0
                            to: 100
                            value: ghostRuntime.auditProgress
                        }

                        Label {
                            Layout.fillWidth: true
                            text: ghostRuntime.auditSummary
                                  + "\n\nCurrent: " + ghostRuntime.auditCurrentStep
                                  + (ghostRuntime.auditActiveId.length > 0 ? "\nAudit ID: " + ghostRuntime.auditActiveId : "")
                                  + (ghostRuntime.auditInputSha256.length > 0 ? "\nInput SHA-256: " + ghostRuntime.auditInputSha256 : "")
                                  + (ghostRuntime.auditRequestClass.length > 0 ? "\nClass: " + ghostRuntime.auditRequestClass + " · risk: " + ghostRuntime.auditRisk : "")
                                  + (ghostRuntime.auditRoute.length > 0 ? "\nRoute: " + ghostRuntime.auditRoute : "")
                                  + (ghostRuntime.auditArtifactPath.length > 0 ? "\nArtifact: " + ghostRuntime.auditArtifactPath : "")
                                  + (ghostRuntime.auditReceiptPath.length > 0 ? "\nReceipt: " + ghostRuntime.auditReceiptPath : "")
                            color: Theme.textBody
                            wrapMode: Text.WordWrap
                        }

                        Label {
                            Layout.fillWidth: true
                            visible: ghostRuntime.auditLines.length > 0
                            text: ghostRuntime.auditLines.join("\n\n")
                            color: Theme.textSoft
                            wrapMode: Text.WordWrap
                        }
                    }
                }

                Rectangle {
                    Layout.fillWidth: true
                    radius: Theme.cardRadius
                    color: Theme.surface
                    border.color: ghostRuntime.resolutionStatus === "resolved" ? Theme.success : Theme.blueBorder
                    border.width: 1
                    implicitHeight: resolutionColumn.implicitHeight + 36

                    ColumnLayout {
                        id: resolutionColumn
                        anchors.fill: parent
                        anchors.margins: Theme.cardPadding
                        spacing: 12

                        RowLayout {
                            Layout.fillWidth: true

                            Label {
                                text: "Ghost Resolution Loop"
                                color: Theme.textStrong
                                font.pixelSize: 24
                                font.bold: true
                                Layout.fillWidth: true
                            }

                            Label {
                                text: ghostRuntime.resolutionStatus + " · " + ghostRuntime.resolutionProgress + "%"
                                color: ghostRuntime.resolutionStatus === "resolved" ? Theme.successText : Theme.accent
                                font.bold: true
                            }
                        }

                        ProgressBar {
                            Layout.fillWidth: true
                            from: 0
                            to: 100
                            value: ghostRuntime.resolutionProgress
                        }

                        Label {
                            Layout.fillWidth: true
                            text: ghostRuntime.resolutionSummary
                                  + "\n\nCurrent: " + ghostRuntime.resolutionCurrentStep
                                  + "\nSelected: " + ghostRuntime.resolutionSelectedId
                            color: Theme.textSoft
                            wrapMode: Text.WordWrap
                        }

                        Label {
                            Layout.fillWidth: true
                            text: ghostRuntime.resolutionLines.length > 0
                                  ? ghostRuntime.resolutionLines.join("\n\n")
                                  : "No Ghost resolutions loaded."
                            color: Theme.textBody
                            wrapMode: Text.WordWrap
                        }

                        RowLayout {
                            Layout.fillWidth: true

                            Button {
                                text: "Select safe Workspace goal"
                                onClicked: appController.selectGhostResolution("resolution-safe-workspace")
                            }

                            Button {
                                text: "Build plan"
                                enabled: ghostRuntime.resolutionStatus === "selected"
                                onClicked: appController.startGhostResolution(ghostRuntime.resolutionSelectedId)
                            }

                            Button {
                                text: "Approve and resolve"
                                enabled: ghostRuntime.resolutionStatus === "awaiting-approval"
                                onClicked: appController.decideGhostResolution(ghostRuntime.resolutionSelectedId, true)
                            }

                            Button {
                                text: "Deny"
                                enabled: ghostRuntime.resolutionStatus === "awaiting-approval"
                                onClicked: appController.decideGhostResolution(ghostRuntime.resolutionSelectedId, false)
                            }

                            Button {
                                text: "Reset"
                                onClicked: appController.resetGhostResolutions()
                            }
                        }
                    }
                }

                GridLayout {
                    Layout.fillWidth: true
                    columns: width > 980 ? 2 : 1
                    columnSpacing: 16
                    rowSpacing: 16

                    Rectangle {
                        Layout.fillWidth: true
                        radius: Theme.cardRadius
                        color: Theme.surface
                        border.color: Theme.border
                        border.width: 1
                        implicitHeight: onboardingColumn.implicitHeight + 36

                        ColumnLayout {
                            id: onboardingColumn
                            anchors.fill: parent
                            anchors.margins: Theme.cardPadding
                            spacing: 10

                            Label {
                                text: ghostRuntime.onboardingTitle
                                color: Theme.textStrong
                                font.pixelSize: 20
                                font.bold: true
                                Layout.fillWidth: true
                                wrapMode: Text.WordWrap
                            }

                            Label {
                                text: ghostRuntime.onboardingStatus
                                color: Theme.accent
                                Layout.fillWidth: true
                                wrapMode: Text.WordWrap
                            }

                            Label {
                                text: ghostRuntime.onboardingBody + "\n\nOpen: " + ghostRuntime.onboardingUrl
                                color: Theme.textSoft
                                Layout.fillWidth: true
                                wrapMode: Text.WordWrap
                            }

                            TextField {
                                Layout.fillWidth: true
                                enabled: heartPassVerified
                                placeholderText: heartPassVerified ? "Paste the user's Brave API key here" : "Verify Heart Pass in Wallet Hub before adding Brave key"
                                text: braveApiKeyInput
                                echoMode: TextInput.Password
                                onTextChanged: braveApiKeyInput = text
                            }

                            RowLayout {
                                Layout.fillWidth: true

                                Button {
                                    text: "Open Brave key page"
                                    enabled: heartPassVerified
                                    onClicked: appController.openUrl(ghostRuntime.onboardingUrl)
                                }

                                Button {
                                    text: "Validate and save Brave key"
                                    enabled: heartPassVerified
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
                                text: appController.ghostConfigStatus + "\nHeart Pass: " + appController.heartPassVerificationStatus
                                color: heartPassVerified ? Theme.successText : Theme.warning
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
                        title: appController.heartPassTitle
                        subtitle: appController.heartPassStatus
                        body: appController.heartPassSummary
                              + "\n\nGhost link: " + (appController.heartPassCapabilityLines.length > 1 ? appController.heartPassCapabilityLines[1] : "Guided onboarding eligibility pending")
                              + "\n\nNext: " + appController.heartPassNextStep
                    }

                    SectionCard {
                        Layout.fillWidth: true
                        title: appController.heartPassQuotaTitle
                        subtitle: appController.heartPassQuotaStatus + " · " + appController.heartPassQuotaRemainingQueries + "/" + appController.heartPassQuotaIncludedQueries + " remaining"
                        body: appController.heartPassQuotaSummary
                              + "\n\nMode: " + appController.heartPassQuotaMode
                              + "\nPeriod: " + appController.heartPassQuotaPeriod
                              + "\nUsage source: " + appController.heartPassQuotaUsageSource
                              + "\nFallback: " + appController.heartPassQuotaFallback
                              + "\n\nNext: " + appController.heartPassQuotaNextStep
                    }

                    SectionCard {
                        Layout.fillWidth: true
                        title: ghostRuntime.intentsTitle
                        subtitle: ghostRuntime.intentsSummary
                        body: ghostRuntime.intentLines.length > 0 ? ghostRuntime.intentLines.join("\n\n") : "No Ghost intents yet."
                    }

                    SectionCard {
                        Layout.fillWidth: true
                        title: "Pipeline layers"
                        subtitle: "Data -> Results -> Algorithms"
                        body: ghostRuntime.pipelineLines.length > 0 ? ghostRuntime.pipelineLines.join("\n\n") : "No pipeline detail yet."
                    }

                    SectionCard {
                        Layout.fillWidth: true
                        title: ghostRuntime.requestClassifierTitle
                        subtitle: "Class, safety, tools, approval, quota, route"
                        body: ghostRuntime.requestClassifierSummary + "\n\n" + (ghostRuntime.requestClassificationLines.length > 0 ? ghostRuntime.requestClassificationLines.join("\n\n") : "No request classes yet.")
                    }

                    SectionCard {
                        Layout.fillWidth: true
                        title: "Ghost action trace"
                        subtitle: "Data -> target -> route -> outcome"
                        body: ghostRuntime.actionTraceSummary
                    }

                    SectionCard {
                        Layout.fillWidth: true
                        title: "Route explanation"
                        subtitle: "Why Ghost chose this path"
                        body: ghostRuntime.routeExplanationSummary
                    }

                    SectionCard {
                        Layout.fillWidth: true
                        title: "Operational readiness"
                        subtitle: ghostRuntime.readinessStatus
                        body: ghostRuntime.readinessSummary + "\n\n" + (ghostRuntime.readinessLines.length > 0 ? ghostRuntime.readinessLines.join("\n\n") : "No readiness assessment yet.")
                    }
                }

                GridLayout {
                    Layout.fillWidth: true
                    columns: 1
                    columnSpacing: 16
                    rowSpacing: 16

                    SectionCard {
                        Layout.fillWidth: true
                        title: "Ghost initiation"
                        subtitle: ghostRuntime.initiationStatus + " · " + ghostRuntime.initiationDatabasePath
                        body: ghostRuntime.initiationSummary + "\n\n" + (ghostRuntime.knowledgeLines.length > 0 ? ghostRuntime.knowledgeLines.join("\n\n") : "After the Brave key is configured, Ghost will gradually cache useful knowledge here: natural language intents, RAG grounding, approval flow, local memory, OS-assistant UX, and task planning.")
                    }

                    SectionCard {
                        Layout.fillWidth: true
                        title: "Human language support"
                        subtitle: ghostRuntime.languageSupportStatus
                        body: ghostRuntime.languageSupportSummary + "\n\n" + (ghostRuntime.languageSupportLines.length > 0 ? ghostRuntime.languageSupportLines.join("\n\n") : "Ghost should detect the user's language, respond naturally, and preserve meaning across translation and retrieval.")
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

                        delegate: Item {
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
                            implicitHeight: approvalCard.implicitHeight

                            ApprovalItem {
                                id: approvalCard
                                anchors.left: parent.left
                                anchors.right: parent.right
                                title: parent.title
                                description: parent.description
                                requestedBy: parent.requestedBy
                                capability: parent.capability
                                scope: parent.scope
                                risk: parent.risk
                                status: parent.status
                                createdAt: parent.createdAt
                            }
                        }
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 16

                    Repeater {
                        model: activityFeedModel

                        delegate: Item {
                            required property string title
                            required property string detail
                            required property string status

                            Layout.fillWidth: true
                            implicitHeight: activityCard.implicitHeight

                            ActivityItem {
                                id: activityCard
                                anchors.left: parent.left
                                anchors.right: parent.right
                                title: parent.title
                                detail: parent.detail
                                status: parent.status
                            }
                        }
                    }
                }
            }
        }
    }
}
