import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

import SolOS.Shell 1.0

Item {
    required property var appController
    property string heartPassWalletInput: ""
    property string pulsoClaimInput: ""

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
                spacing: Theme.screenGap

        SectionCard {
            Layout.fillWidth: true
            title: "Wallet"
            subtitle: "Explicit ownership"
            body: "Balances, assets, connection state, and future signature requests should remain visible and deliberate."
        }

        Rectangle {
            Layout.fillWidth: true
            radius: Theme.cardRadius
            color: Theme.surface
            border.color: Theme.border
            border.width: 1
            implicitHeight: pulsoRewardColumn.implicitHeight + 36

            ColumnLayout {
                id: pulsoRewardColumn
                anchors.fill: parent
                anchors.margins: Theme.cardPadding
                spacing: 10

                Label {
                    text: "Pulso Founder Reward"
                    color: Theme.textStrong
                    font.pixelSize: 20
                    font.bold: true
                }

                Label {
                    text: "Paste the claim code shown in the authenticated Pulso rewards page. The claim is bound to the verified Polygon wallet and adds its Ghost-query allowance to this Wallet."
                    color: Theme.textSoft
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                }

                TextField {
                    Layout.fillWidth: true
                    placeholderText: "Pulso reward claim code"
                    text: pulsoClaimInput
                    onTextChanged: pulsoClaimInput = text
                }

                Button {
                    text: "Claim Ghost allowance"
                    enabled: appController.heartPassVerificationStatus === "verified-holder"
                    onClicked: appController.claimPulsoGhostReward(pulsoClaimInput)
                }
            }
        }

        SectionCard {
            Layout.fillWidth: true
            title: "Account Summary"
            subtitle: "Mock data"
            body: "Solana · 9xLu...Ghost · 12.84 SOL · 248.00 USDC"
        }

        SectionCard {
            Layout.fillWidth: true
            title: appController.heartPassTitle
            subtitle: appController.heartPassStatus
            body: appController.heartPassSummary
                  + "\n\nNetwork: " + appController.heartPassNetwork
                  + "\nToken standard: " + appController.heartPassTokenStandard
                  + "\nContract: " + appController.heartPassContract
                  + "\nToken ID: " + appController.heartPassTokenId
                  + "\nWallet: " + appController.heartPassWalletAddress
                  + "\nOn-chain evidence: " + appController.heartPassOwnerAddress
                  + "\nVerification: " + appController.heartPassVerificationStatus
                  + "\nLast checked: " + appController.heartPassLastCheckedAt
                  + "\nConfig: " + appController.heartPassConfigPath
                  + "\nOpenSea: " + appController.heartPassOpenSeaUrl
                  + "\n\nCapabilities:\n• " + appController.heartPassCapabilityLines.join("\n• ")
                  + "\n\nNext: " + appController.heartPassNextStep
        }

        SectionCard {
            Layout.fillWidth: true
            title: appController.heartPassQuotaTitle
            subtitle: appController.heartPassQuotaStatus + " · " + appController.heartPassQuotaMode
            body: appController.heartPassQuotaSummary
                  + "\n\nPeriod: " + appController.heartPassQuotaPeriod
                  + "\nIncluded queries: " + appController.heartPassQuotaIncludedQueries
                  + "\nUsed queries: " + appController.heartPassQuotaUsedQueries
                  + "\nRemaining queries: " + appController.heartPassQuotaRemainingQueries
                  + "\nUsage source: " + appController.heartPassQuotaUsageSource
                  + "\nFallback: " + appController.heartPassQuotaFallback
                  + "\nLast sync: " + appController.heartPassQuotaLastSync
                  + "\nReset policy: " + appController.heartPassQuotaResetPolicy
                  + "\n\nNext: " + appController.heartPassQuotaNextStep
        }

        Rectangle {
            Layout.fillWidth: true
            radius: Theme.cardRadius
            color: Theme.surface
            border.color: Theme.border
            border.width: 1
            implicitHeight: heartPassConfigColumn.implicitHeight + 36

            ColumnLayout {
                id: heartPassConfigColumn
                anchors.fill: parent
                anchors.margins: Theme.cardPadding
                spacing: 10

                Label {
                    text: "Heart Pass local wallet"
                    color: Theme.textStrong
                    font.pixelSize: 20
                    font.bold: true
                    Layout.fillWidth: true
                }

                Label {
                    text: "Store the Polygon wallet address locally. On-chain NFT ownership verification comes in the next stage."
                    color: Theme.textSoft
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                }

                TextField {
                    Layout.fillWidth: true
                    placeholderText: "0x... Polygon wallet address"
                    text: heartPassWalletInput
                    onTextChanged: heartPassWalletInput = text
                }

                RowLayout {
                    Layout.fillWidth: true

                    Button {
                        text: "Save wallet locally"
                        onClicked: appController.saveHeartPassWalletAddress(heartPassWalletInput)
                    }

                    Button {
                        text: "Clear wallet"
                        onClicked: {
                            heartPassWalletInput = ""
                            appController.clearHeartPassWalletAddress()
                        }
                    }

                    Button {
                        text: "Verify on Polygon"
                        onClicked: appController.verifyHeartPassOwnership()
                    }

                    Button {
                        text: "Open NFT"
                        onClicked: appController.openUrl(appController.heartPassOpenSeaUrl)
                    }
                }
            }
            }
        }
    }
}
}
