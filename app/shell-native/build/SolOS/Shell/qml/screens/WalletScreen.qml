import QtQuick
import QtQuick.Layouts

import SolOS.Shell 1.0

Item {
    ColumnLayout {
        anchors.fill: parent
        spacing: 16

        SectionCard {
            Layout.fillWidth: true
            title: "Wallet"
            subtitle: "Explicit ownership"
            body: "Balances, assets, connection state, and future signature requests should remain visible and deliberate."
        }

        SectionCard {
            Layout.fillWidth: true
            title: "Account Summary"
            subtitle: "Mock data"
            body: "Solana · 9xLu...Ghost · 12.84 SOL · 248.00 USDC"
        }
    }
}
