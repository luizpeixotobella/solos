import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import QtQuick.Window

ApplicationWindow {
    id: window
    width: 1280
    height: 820
    minimumWidth: 960
    minimumHeight: 640
    visibility: Window.Maximized
    visible: true
    title: "SolOS Explorer"
    color: "#101421"

    property string activeView: "Home"
    property string searchText: ""

    component NavButton: Button {
        id: navButton
        property bool selected: false
        implicitHeight: 44
        background: Rectangle {
            radius: 12
            color: navButton.selected ? "#253858" : (navButton.hovered ? "#1c273b" : "transparent")
        }
        contentItem: RowLayout {
            spacing: 12
            Text {
                text: navButton.text.slice(0, 1)
                color: navButton.selected ? "#8dc5ff" : "#96a3b8"
                font.pixelSize: 15
                font.weight: Font.DemiBold
                Layout.leftMargin: 9
            }
            Text {
                text: navButton.text
                color: navButton.selected ? "#f2f6ff" : "#b6c0d1"
                font.pixelSize: 14
                Layout.fillWidth: true
            }
        }
    }

    component PlaceButton: Button {
        id: placeButton
        property string folderPath: ""
        implicitHeight: 40
        background: Rectangle {
            radius: 10
            color: placeButton.hovered ? "#1c273b" : "transparent"
        }
        contentItem: RowLayout {
            spacing: 10
            Text { text: "▱"; color: "#7faef2"; font.pixelSize: 17; Layout.leftMargin: 9 }
            Text { text: placeButton.text; color: "#aeb9cc"; font.pixelSize: 13; Layout.fillWidth: true }
        }
        onClicked: {
            window.activeView = "Files"
            explorer.openFolder(folderPath)
        }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.preferredWidth: 224
            Layout.fillHeight: true
            color: "#0b0f19"

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 18
                spacing: 20

                RowLayout {
                    spacing: 11
                    Rectangle {
                        width: 36; height: 36; radius: 12
                        gradient: Gradient {
                            GradientStop { position: 0; color: "#73d7bd" }
                            GradientStop { position: 1; color: "#559bda" }
                        }
                        Text { anchors.centerIn: parent; text: "S"; color: "#07151b"; font.pixelSize: 20; font.weight: Font.Bold }
                    }
                    Column {
                        spacing: 2
                        Text { text: "SolOS"; color: "#f3f6fb"; font.pixelSize: 16; font.weight: Font.DemiBold }
                        Text { text: "EXPLORER · PROTOTYPE"; color: "#738198"; font.pixelSize: 8; font.letterSpacing: 1.2 }
                    }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 4
                    Text { text: "WORKSPACE"; color: "#637088"; font.pixelSize: 10; font.letterSpacing: 1.5; leftPadding: 9; bottomPadding: 5 }
                    NavButton { text: "Home"; selected: window.activeView === "Home"; Layout.fillWidth: true; onClicked: window.activeView = "Home" }
                    NavButton { text: "Files"; selected: window.activeView === "Files"; Layout.fillWidth: true; onClicked: window.activeView = "Files" }
                    NavButton { text: "Apps"; selected: window.activeView === "Apps"; Layout.fillWidth: true; onClicked: window.activeView = "Apps" }
                }

                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 4
                    Text { text: "YOUR PLACES"; color: "#637088"; font.pixelSize: 10; font.letterSpacing: 1.5; leftPadding: 9; bottomPadding: 5 }
                    Repeater {
                        model: explorer.places
                        delegate: PlaceButton {
                            required property var modelData
                            text: modelData.name
                            folderPath: modelData.path
                            Layout.fillWidth: true
                        }
                    }
                }

                Item { Layout.fillHeight: true }

                Rectangle {
                    Layout.fillWidth: true
                    implicitHeight: 94
                    radius: 16
                    color: "#141c2b"
                    border.color: "#202d41"
                    Column {
                        anchors.fill: parent
                        anchors.margins: 13
                        spacing: 7
                        Text { text: "SOLos RUNTIME"; color: "#77869f"; font.pixelSize: 9; font.letterSpacing: 1.2 }
                        Row {
                            spacing: 7
                            Rectangle {
                                width: 7; height: 7; radius: 4
                                anchors.verticalCenter: parent.verticalCenter
                                color: explorer.runtimeStatus === "SolOS runtime is running" ? "#58d6a5" : "#e4b86a"
                            }
                            Text {
                                width: 158
                                text: explorer.runtimeStatus
                                color: "#c4cfdf"
                                font.pixelSize: 11
                                elide: Text.ElideRight
                            }
                        }
                        Text { text: "Local · owner controlled"; color: "#718098"; font.pixelSize: 10 }
                    }
                }
            }
        }

        Rectangle {
            Layout.fillWidth: true
            Layout.fillHeight: true
            color: "#101421"

            ColumnLayout {
                anchors.fill: parent
                anchors.leftMargin: 34
                anchors.rightMargin: 34
                anchors.topMargin: 25
                anchors.bottomMargin: 26
                spacing: 24

                RowLayout {
                    Layout.fillWidth: true
                    spacing: 12
                    TextField {
                        Layout.fillWidth: true
                        Layout.preferredWidth: 380
                        Layout.maximumWidth: 560
                        placeholderText: "Search files and apps"
                        leftPadding: 42
                        color: "#e6ecf6"
                        placeholderTextColor: "#738098"
                        background: Rectangle { radius: 12; color: "#171e2d"; border.color: "#252f43" }
                        onTextChanged: window.searchText = text
                        Text { anchors.left: parent.left; anchors.leftMargin: 15; anchors.verticalCenter: parent.verticalCenter; text: "⌕"; color: "#8997ad"; font.pixelSize: 20 }
                    }
                    Item { Layout.fillWidth: true }
                    Rectangle {
                        width: 36; height: 36; radius: 18; color: "#26334a"
                        Text { anchors.centerIn: parent; text: explorer.displayName.slice(0, 1); color: "#bbd7fb"; font.weight: Font.DemiBold }
                    }
                    Text { text: explorer.displayName; color: "#c7d0df"; font.pixelSize: 13 }
                }

                Loader {
                    Layout.fillWidth: true
                    Layout.fillHeight: true
                    sourceComponent: window.activeView === "Files" ? filesView : (window.activeView === "Apps" ? appsView : homeView)
                }
            }
        }
    }

    Component {
        id: homeView
        Flickable {
            clip: true
            contentHeight: homeContent.implicitHeight
            ColumnLayout {
                id: homeContent
                width: parent.width
                spacing: 24

                Column {
                    spacing: 8
                    Text { text: "Welcome back, " + explorer.displayName; color: "#f0f4fb"; font.pixelSize: 30; font.weight: Font.DemiBold }
                    Text { text: "Your desktop, files, and apps — all in one calm space."; color: "#8e9bb0"; font.pixelSize: 14 }
                }

                Rectangle {
                    Layout.fillWidth: true
                    implicitHeight: 164
                    radius: 20
                    gradient: Gradient {
                        GradientStop { position: 0; color: "#1c3550" }
                        GradientStop { position: 0.55; color: "#1d3145" }
                        GradientStop { position: 1; color: "#283149" }
                    }
                    RowLayout {
                        anchors.fill: parent
                        anchors.margins: 25
                        ColumnLayout {
                            Layout.fillWidth: true
                            spacing: 9
                            Text { text: "A good place to begin"; color: "#9fc7df"; font.pixelSize: 12; font.letterSpacing: 0.7 }
                            Text { text: "Make room for what matters."; color: "#f1f5fb"; font.pixelSize: 23; font.weight: Font.Medium }
                            Text { text: "Browse your files or jump straight into an app."; color: "#bdcbd9"; font.pixelSize: 13 }
                        }
                        Button {
                            text: "Open files"
                            onClicked: window.activeView = "Files"
                            contentItem: Text { text: parent.text; color: "#101b24"; font.pixelSize: 12; font.weight: Font.DemiBold; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter }
                            background: Rectangle { radius: 10; color: "#95d8c4" }
                            Layout.preferredWidth: 110
                            Layout.preferredHeight: 40
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Text { text: "Quick access"; color: "#ecf1f8"; font.pixelSize: 17; font.weight: Font.DemiBold }
                    Item { Layout.fillWidth: true }
                    Button { text: "See all"; flat: true; onClicked: window.activeView = "Files" }
                }

                GridLayout {
                    Layout.fillWidth: true
                    columns: 2
                    columnSpacing: 14
                    rowSpacing: 14
                    Repeater {
                        model: explorer.places
                        delegate: Rectangle {
                            required property var modelData
                            Layout.fillWidth: true
                            implicitHeight: 82
                            radius: 15
                            color: quickArea.containsMouse ? "#1c2638" : "#171e2d"
                            border.color: "#252f43"
                            RowLayout {
                                anchors.fill: parent
                                anchors.margins: 16
                                spacing: 13
                                Rectangle {
                                    width: 38; height: 38; radius: 12
                                    color: "#22344b"
                                    Text { anchors.centerIn: parent; text: "▱"; color: "#91bdea"; font.pixelSize: 20 }
                                }
                                ColumnLayout {
                                    Layout.fillWidth: true
                                    spacing: 4
                                    Text { text: modelData.name; color: "#e1e8f2"; font.pixelSize: 13; font.weight: Font.Medium }
                                    Text { text: "Open folder"; color: "#7e8ba1"; font.pixelSize: 11 }
                                }
                            }
                            MouseArea {
                                id: quickArea
                                anchors.fill: parent
                                hoverEnabled: true
                                cursorShape: Qt.PointingHandCursor
                                onClicked: { explorer.openFolder(modelData.path); window.activeView = "Files" }
                            }
                        }
                    }
                }

                RowLayout {
                    Layout.fillWidth: true
                    Text { text: "Apps"; color: "#ecf1f8"; font.pixelSize: 17; font.weight: Font.DemiBold }
                    Item { Layout.fillWidth: true }
                    Button { text: "Browse apps"; flat: true; onClicked: window.activeView = "Apps" }
                }

                Flow {
                    Layout.fillWidth: true
                    spacing: 10
                    Repeater {
                        model: explorer.apps.slice(0, 8)
                        delegate: Button {
                            required property var modelData
                            text: modelData.name
                            onClicked: explorer.launchApp(modelData.desktopFile)
                            contentItem: Text { text: parent.text; color: "#cad4e3"; font.pixelSize: 12; horizontalAlignment: Text.AlignHCenter; verticalAlignment: Text.AlignVCenter; elide: Text.ElideRight }
                            background: Rectangle { radius: 10; color: parent.hovered ? "#26344b" : "#1a2232"; border.color: "#2a354a" }
                            implicitWidth: Math.max(94, Math.min(170, contentItem.implicitWidth + 28))
                            implicitHeight: 40
                        }
                    }
                }
            }
        }
    }

    Component {
        id: filesView
        ColumnLayout {
            spacing: 16
            RowLayout {
                Layout.fillWidth: true
                ColumnLayout {
                    Layout.fillWidth: true
                    spacing: 5
                    Text { text: "Files"; color: "#f0f4fb"; font.pixelSize: 26; font.weight: Font.DemiBold }
                    Text { text: explorer.currentPath; color: "#8491a6"; font.pixelSize: 12; elide: Text.ElideMiddle; Layout.fillWidth: true }
                }
                Button { text: "↑  Up"; onClicked: explorer.openParentFolder() }
            }
            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                radius: 16
                color: "#151c2a"
                border.color: "#252f43"
                ListView {
                    anchors.fill: parent
                    anchors.margins: 8
                    clip: true
                    model: explorer.files
                    delegate: ItemDelegate {
                        required property var modelData
                        width: ListView.view.width
                        height: 58
                        visible: window.searchText.length === 0 || modelData.name.toLowerCase().includes(window.searchText.toLowerCase())
                        onClicked: explorer.openItem(modelData.path, modelData.isDirectory)
                        background: Rectangle { radius: 10; color: parent.hovered ? "#202b3d" : "transparent" }
                        contentItem: RowLayout {
                            spacing: 13
                            Rectangle {
                                width: 36; height: 36; radius: 11
                                color: modelData.isDirectory ? "#233650" : "#242b3b"
                                Text { anchors.centerIn: parent; text: modelData.isDirectory ? "▱" : "◫"; color: modelData.isDirectory ? "#94bce9" : "#9ca8bc"; font.pixelSize: 17 }
                            }
                            Text { text: modelData.name; color: "#e0e6ef"; font.pixelSize: 13; Layout.fillWidth: true; elide: Text.ElideRight }
                            Text { text: modelData.detail; color: "#8290a5"; font.pixelSize: 11; Layout.preferredWidth: 95; horizontalAlignment: Text.AlignRight }
                        }
                    }
                    Text {
                        anchors.centerIn: parent
                        visible: explorer.files.length === 0
                        text: "This folder is empty"
                        color: "#7e8ba1"
                        font.pixelSize: 13
                    }
                }
            }
        }
    }

    Component {
        id: appsView
        ColumnLayout {
            spacing: 18
            Column {
                spacing: 6
                Text { text: "Your apps"; color: "#f0f4fb"; font.pixelSize: 26; font.weight: Font.DemiBold }
                Text { text: "Installed applications ready when you are."; color: "#8e9bb0"; font.pixelSize: 13 }
            }
            GridView {
                Layout.fillWidth: true
                Layout.fillHeight: true
                clip: true
                cellWidth: 176
                cellHeight: 154
                model: explorer.apps
                delegate: Rectangle {
                    required property var modelData
                    width: 160; height: 138; radius: 16
                    color: appCard.containsMouse ? "#202b3d" : "#171e2d"
                    border.color: "#283348"
                    Column {
                        anchors.centerIn: parent
                        width: parent.width - 20
                        spacing: 10
                        Rectangle {
                            width: 48; height: 48; radius: 15
                            anchors.horizontalCenter: parent.horizontalCenter
                            color: "#253751"
                            Text { anchors.centerIn: parent; text: modelData.name.slice(0, 1).toUpperCase(); color: "#a8d3eb"; font.pixelSize: 21; font.weight: Font.Medium }
                        }
                        Text { width: parent.width; text: modelData.name; color: "#e0e6ef"; font.pixelSize: 12; horizontalAlignment: Text.AlignHCenter; elide: Text.ElideRight }
                    }
                    MouseArea {
                        id: appCard
                        anchors.fill: parent
                        hoverEnabled: true
                        cursorShape: Qt.PointingHandCursor
                        onClicked: explorer.launchApp(modelData.desktopFile)
                    }
                }
            }
        }
    }
}
