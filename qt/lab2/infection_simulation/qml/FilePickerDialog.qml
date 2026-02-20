import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import Qt.labs.folderlistmodel
import QtCore

Item {
    id: dlg
    anchors.fill: parent
    visible: false
    z: 1000

    signal accepted(string filePath)
    property string currentPath: ""

    function open() {
        folderModel.folder = StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
        currentPath = ""
        visible = true
    }

    // dim background
    Rectangle {
        anchors.fill: parent
        color: Qt.rgba(0, 0, 0, 0.6)
        MouseArea { anchors.fill: parent; onClicked: dlg.visible = false }
    }

    // dialog box
    Rectangle {
        id: box
        anchors.centerIn: parent
        width: 640; height: 460
        radius: 10
        color: "#111827"
        border.color: "#2a3a55"; border.width: 1

        layer.enabled: true

        ColumnLayout {
            anchors.fill: parent
            anchors.margins: 0
            spacing: 0

            // ── title bar ─────────────────────────────────────────────────
            Rectangle {
                Layout.fillWidth: true; height: 48
                color: "#1a2235"
                radius: 10
                // cover bottom radius
                Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 10; color: "#1a2235" }
                Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: "#2a3a55" }

                RowLayout {
                    anchors.fill: parent; anchors.margins: 16; spacing: 12
                    Text {
                        text: "📂"
                        font.pixelSize: 16
                    }
                    Text {
                        text: "Select graph file"
                        color: "#e2e8f0"; font.pixelSize: 14; font.bold: true
                        Layout.fillWidth: true
                    }
                    Rectangle {
                        width: 28; height: 28; radius: 4; color: closeMa.containsMouse ? "#ef4444" : "transparent"
                        Text { anchors.centerIn: parent; text: "✕"; color: "#64748b"; font.pixelSize: 13 }
                        MouseArea { id: closeMa; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: dlg.visible = false }
                    }
                }
            }

            Rectangle {
                Layout.fillWidth: true; height: 38
                color: "#0d1525"
                border.color: "#2a3a55"; border.width: 0
                Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: "#2a3a55" }

                Row {
                    anchors.verticalCenter: parent.verticalCenter
                    anchors.left: parent.left; anchors.leftMargin: 14
                    spacing: 4

                    // Up button
                    Rectangle {
                        width: 28; height: 28; radius: 4
                        color: upMa.containsMouse ? "#1e3050" : "transparent"
                        anchors.verticalCenter: parent.verticalCenter
                        Text { anchors.centerIn: parent; text: "↑"; color: "#3b82f6"; font.pixelSize: 14; font.bold: true }
                        MouseArea {
                            id: upMa; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                var cur = folderModel.folder.toString().replace("file://", "")
                                var parts = cur.split("/").filter(function(p){ return p !== "" })
                                if (parts.length > 0) {
                                    parts.pop()
                                    folderModel.folder = "file:///" + parts.join("/")
                                }
                            }
                        }
                    }

                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        text: folderModel.folder.toString().replace("file://", "")
                        color: "#64748b"; font.pixelSize: 11
                        elide: Text.ElideLeft; width: 520
                    }
                }
            }

            // file list
            Rectangle {
                Layout.fillWidth: true; Layout.fillHeight: true
                color: "#0a0e1a"

                FolderListModel {
                    id: folderModel
                    folder: StandardPaths.standardLocations(StandardPaths.HomeLocation)[0]
                    showDirsFirst: true
                    showDotAndDotDot: false
                    nameFilters: ["*.txt", "*.csv", "*"]
                }

                ListView {
                    id: fileList
                    anchors.fill: parent
                    anchors.margins: 8
                    model: folderModel
                    clip: true
                    spacing: 2

                    ScrollBar.vertical: ScrollBar {
                        policy: ScrollBar.AsNeeded
                        contentItem: Rectangle { radius: 3; color: "#2a3a55" }
                    }

                    delegate: Rectangle {
                        width: fileList.width - 8
                        height: 38; radius: 6
                        color: {
                            if (dlg.currentPath === filePath) return Qt.rgba(0.23, 0.51, 0.96, 0.2)
                            return rowMa.containsMouse ? "#1a2235" : "transparent"
                        }
                        border.color: dlg.currentPath === filePath ? "#3b82f6" : "transparent"
                        border.width: 1

                        Row {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left; anchors.leftMargin: 12
                            spacing: 10

                            Text {
                                text: fileIsDir ? "📁" : "📄"
                                font.pixelSize: 14
                                anchors.verticalCenter: parent.verticalCenter
                            }
                            Text {
                                text: fileName
                                color: fileIsDir ? "#93c5fd" : "#e2e8f0"
                                font.pixelSize: 13
                                font.bold: fileIsDir
                                anchors.verticalCenter: parent.verticalCenter
                            }
                        }

                        MouseArea {
                            id: rowMa; anchors.fill: parent; hoverEnabled: true
                            cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                if (fileIsDir) {
                                    folderModel.folder = "file://" + filePath
                                    dlg.currentPath = ""
                                } else {
                                    dlg.currentPath = filePath
                                }
                            }
                            onDoubleClicked: {
                                if (!fileIsDir && filePath !== "") {
                                    dlg.accepted(filePath)
                                    dlg.visible = false
                                }
                            }
                        }
                    }
                }
            }

            // path input + buttons
            Rectangle {
                Layout.fillWidth: true; height: 60
                color: "#1a2235"
                radius: 10
                Rectangle { anchors.top: parent.top; width: parent.width; height: 10; color: "#1a2235" }
                Rectangle { anchors.top: parent.top; width: parent.width; height: 1; color: "#2a3a55" }

                RowLayout {
                    anchors.fill: parent; anchors.margins: 12; spacing: 10

                    Rectangle {
                        Layout.fillWidth: true; height: 36; radius: 6
                        color: "#0d1525"; border.color: pathField.activeFocus ? "#3b82f6" : "#2a3a55"; border.width: 1

                        TextInput {
                            id: pathField
                            anchors.fill: parent; anchors.margins: 10
                            color: "#e2e8f0"; font.pixelSize: 12
                            verticalAlignment: TextInput.AlignVCenter
                            selectByMouse: true
                            clip: true
                            text: dlg.currentPath
                            onTextChanged: dlg.currentPath = text
                            Text {
                                anchors.fill: parent
                                text: "File path..."
                                color: "#3a4a65"; font.pixelSize: 12
                                verticalAlignment: Text.AlignVCenter
                                visible: dlg.currentPath === ""
                            }
                        }
                    }

                    // Cancel
                    Rectangle {
                        width: 80; height: 36; radius: 6
                        color: cancelBtnMa.containsMouse ? "#2a3a55" : "transparent"
                        border.color: "#2a3a55"; border.width: 1
                        Text { anchors.centerIn: parent; text: "Cancel"; color: "#64748b"; font.pixelSize: 13 }
                        MouseArea { id: cancelBtnMa; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: dlg.visible = false }
                    }

                    // Open
                    Rectangle {
                        width: 80; height: 36; radius: 6
                        color: openBtnMa.containsMouse ? "#2563eb" : "#3b82f6"
                        opacity: dlg.currentPath !== "" ? 1.0 : 0.4
                        Text { anchors.centerIn: parent; text: "Open"; color: "#fff"; font.pixelSize: 13; font.bold: true }
                        MouseArea {
                            id: openBtnMa; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor
                            onClicked: {
                                if (dlg.currentPath !== "") {
                                    dlg.accepted(dlg.currentPath)
                                    dlg.visible = false
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
