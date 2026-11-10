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
            subtitle: "Unconfigured or read-only · never a signing wallet"
            body: "Set up a public address below. Balances remain unavailable until a valid Polygon RPC response is observed."
        }

        SectionCard {
            Layout.fillWidth: true
            title: "Pulso rewards"
            subtitle: "Unavailable in this recovery stage"
            body: "No usable Ghost allowance is asserted. Legacy reward records do not establish a funded balance or a working sponsored service."
        }

        Rectangle {
            Layout.fillWidth: true
            radius: Theme.cardRadius
            color: Theme.surface
            border.color: Theme.border
            implicitHeight: walletSetup.implicitHeight + 36
            ColumnLayout {
                id: walletSetup
                anchors.fill: parent
                anchors.margins: Theme.cardPadding
                spacing: 10
                Label { text: "Polygon · public address observer"; color: Theme.textStrong; font.pixelSize: 20 }
                Label {
                    text: appController.wallet.status
                    color: Theme.textSoft
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                }
                Label {
                    text: "Saving an address does not connect a signing wallet or prove control, Heart Pass entitlement or sponsored inference. No keys, seeds, tokens or transactions. Querying shares the public address with your selected RPC provider."
                    color: Theme.textSoft
                    Layout.fillWidth: true
                    wrapMode: Text.WordWrap
                }
                TextField {
                    id: readAddress
                    Layout.fillWidth: true
                    placeholderText: "Public Polygon address (0x…)"
                    text: appController.wallet.address
                    maximumLength: 42
                }
                TextField {
                    id: readEndpoint
                    Layout.fillWidth: true
                    placeholderText: "Your Polygon HTTPS RPC (localhost HTTP also allowed)"
                    text: appController.wallet.endpoint
                    maximumLength: 2048
                }
                RowLayout {
                    Button { text: "Save read-only setup"; onClicked: appController.wallet.configure(readAddress.text, readEndpoint.text) }
                    Button { text: "Read balance"; enabled: !appController.wallet.busy && appController.wallet.address !== ""; onClicked: appController.wallet.refresh() }
                    Button { text: "Clear"; onClicked: { if (appController.wallet.clear()) { readAddress.text = ""; readEndpoint.text = "" } } }
                }
                Label {
                    text: "Native balance (POL): " + appController.wallet.balance + "\nLast observed: " + appController.wallet.checkedAt + "\nToken/NFT balances: not queried. No estimated funds are displayed."
                    color: Theme.textSoft
                    Layout.fillWidth: true
                    wrapMode: Text.WrapAnywhere
                }
            }
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

                  + "\n\nNext: " + appController.heartPassNextStep
        }

        SectionCard {
            Layout.fillWidth: true
            title: "Sponsored inference"
            subtitle: "Unavailable · planned quota service"
            body: "Local quota/reward fields are not a spendable balance or a working sponsored service. A public address or observed NFT holding does not activate inference."
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
                    text: "Separate Heart Pass address configuration. Verify on Polygon to observe NFT holdings; this does not prove address control or activate sponsored inference."
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
