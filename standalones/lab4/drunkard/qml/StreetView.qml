import QtQuick
import QtQuick.Layouts
import "."

Item {
    id: sv

    property int cafePos: walk_controller.cafe_pos
    property int cliffPos: walk_controller.cliff_pos
    property int currentPos: walk_controller.current_position
    property bool finished: walk_controller.walk_finished
    property bool cliff: walk_controller.fell_off_cliff
    property int steps: walk_controller.steps_taken

    // viewRadius grows with cafePos so cliff and cafe both always fit on screen
    // We need at least cafePos tiles to the left (cliff side) + some padding right
    readonly property int viewRadius: Math.max(10, cafePos + 3)
    readonly property int tileCount: viewRadius * 2 + 1
    readonly property real tileW: width / tileCount

    // Camera centered on cafePos
    function posToX(pos) {
        return (pos - cafePos + viewRadius) * tileW + tileW / 2
    }

    // ── Background sky gradient ──────────────────────────────────────────────
    Rectangle {
        anchors.fill: parent
        gradient: Gradient {
            orientation: Gradient.Vertical
            GradientStop { position: 0.0; color: "#0a0718" }
            GradientStop { position: 1.0; color: "#0f0c1f" }
        }
    }

    // ── Stars ────────────────────────────────────────────────────────────────
    Repeater {
        model: 40
        Rectangle {
            width: 2; height: 2; radius: 1
            color: Qt.rgba(1, 1, 1, 0.3 + Math.random() * 0.4)
            x: Math.random() * sv.width
            y: Math.random() * sv.height * 0.5
        }
    }

    // ── Ground ───────────────────────────────────────────────────────────────
    Rectangle {
        id: ground
        anchors.bottom: parent.bottom
        width: parent.width
        height: parent.height * 0.38
        color: "#14102a"
        Rectangle {
            anchors.top: parent.top
            width: parent.width; height: 2
            color: Theme.border
        }
    }

    // ── Tile markers (position numbers) ─────────────────────────────────────
    Repeater {
        model: sv.tileCount
        Item {
            x: index * sv.tileW
            y: sv.height - ground.height - 24
            width: sv.tileW
            height: 24

            readonly property int tilePos: sv.cafePos - sv.viewRadius + index

            // Tick mark
            Rectangle {
                anchors.bottom: parent.bottom
                anchors.horizontalCenter: parent.horizontalCenter
                width: 1; height: 8
                color: tilePos === sv.cafePos ? Theme.warn :
                       tilePos <= sv.cliffPos ? Theme.danger : Theme.border
                opacity: 0.6
            }

            // Label
            Text {
                anchors.horizontalCenter: parent.horizontalCenter
                anchors.bottom: parent.bottom
                anchors.bottomMargin: 10
                text: tilePos
                font.pixelSize: 9
                color: tilePos === sv.cafePos ? Theme.warn :
                       tilePos <= sv.cliffPos ? Qt.rgba(Theme.danger.r, Theme.danger.g, Theme.danger.b, 0.7) :
                       Theme.muted
                opacity: 0.8
            }
        }
    }

    // ── Road surface with dashes ─────────────────────────────────────────────
    Rectangle {
        id: road
        anchors.bottom: parent.bottom
        anchors.bottomMargin: ground.height - 40
        width: parent.width; height: 40
        color: "#1e1836"
        Rectangle { anchors.top: parent.top; width: parent.width; height: 1; color: Theme.border; opacity: 0.5 }

        // Center dashes
        Repeater {
            model: Math.ceil(sv.width / 60) + 2
            Rectangle {
                x: index * 60 - 10
                anchors.verticalCenter: parent.verticalCenter
                width: 36; height: 2; radius: 1
                color: Qt.rgba(Theme.muted.r, Theme.muted.g, Theme.muted.b, 0.25)
            }
        }
    }

    // ── Cliff (left edge) ────────────────────────────────────────────────────
    Item {
        x: sv.posToX(sv.cliffPos) - sv.tileW / 2
        y: sv.height - ground.height - 60
        width: sv.tileW
        height: sv.height - y
        visible: x < sv.width && x + width > 0

        // Cliff face
        Rectangle {
            anchors.top: parent.top
            anchors.bottom: parent.bottom
            anchors.right: parent.right
            width: parent.width / 2
            color: "#2a0a0a"
            border.color: Theme.danger
            border.width: 1
            radius: 2
        }

        // Danger stripes
        Repeater {
            model: 5
            Rectangle {
                x: parent.width / 2 + 2
                y: 10 + index * 14
                width: parent.width / 2 - 4; height: 4; radius: 2
                color: Qt.rgba(Theme.danger.r, Theme.danger.g, Theme.danger.b, 0.4)
            }
        }

        Text {
            anchors.horizontalCenter: parent.right
            anchors.horizontalCenterOffset: -parent.width / 4
            y: 4
            text: "💀"
            font.pixelSize: 18
        }

        Text {
            anchors.horizontalCenter: parent.right
            anchors.horizontalCenterOffset: -parent.width / 4
            y: 26
            text: "CLIFF"
            font.pixelSize: 8
            color: Theme.danger
            font.bold: true
            font.letterSpacing: 1
        }
    }

    // ── Cafe ─────────────────────────────────────────────────────────────────
    Item {
        x: sv.posToX(sv.cafePos) - 24
        y: sv.height - ground.height - 72
        width: 48
        height: 72
        visible: x > -width && x < sv.width

        // Building
        Rectangle {
            anchors.bottom: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
            width: 42; height: 58
            color: "#2a1a4a"
            border.color: Theme.warn
            border.width: 1
            radius: 3

            // Windows
            Repeater {
                model: 4
                Rectangle {
                    x: (index % 2) * 18 + 4
                    y: Math.floor(index / 2) * 18 + 6
                    width: 12; height: 10; radius: 2
                    color: Qt.rgba(Theme.warn.r, Theme.warn.g, Theme.warn.b, 0.4)
                    border.color: Qt.rgba(Theme.warn.r, Theme.warn.g, Theme.warn.b, 0.6)
                    border.width: 1
                }
            }
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            y: 0
            text: "☕"
            font.pixelSize: 16
        }

        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            y: parent.height - 14
            text: "B=" + sv.cafePos
            font.pixelSize: 8
            color: Theme.warn
            font.bold: true
        }
    }

    Item {
        id: person
        x: sv.posToX(sv.currentPos) - 16
        y: sv.height - ground.height - 52
        width: 32; height: 52
        visible: !sv.finished || !sv.cliff

        Behavior on x { NumberAnimation { duration: 150; easing.type: Easing.OutCubic } }

        // Shadow
        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            width: 22; height: 8
            radius: 4
            color: Qt.rgba(0, 0, 0, 0.4)
        }

        // Body
        Text {
            anchors.horizontalCenter: parent.horizontalCenter
            y: 0
            text: sv.finished ? (sv.cliff ? "💀" : "😊") : "🥴"
            font.pixelSize: 28

            SequentialAnimation on y {
                running: !sv.finished
                loops: Animation.Infinite
                NumberAnimation { to: -4; duration: 300; easing.type: Easing.InOutSine }
                NumberAnimation { to: 0;  duration: 300; easing.type: Easing.InOutSine }
            }
        }

        // State label
        Rectangle {
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottom: parent.bottom
            width: stateText.implicitWidth + 8; height: 16; radius: 3
            color: sv.finished
                ? (sv.cliff ? Qt.rgba(Theme.danger.r, Theme.danger.g, Theme.danger.b, 0.3) : Qt.rgba(Theme.success.r, Theme.success.g, Theme.success.b, 0.3))
                : Qt.rgba(Theme.accent.r, Theme.accent.g, Theme.accent.b, 0.3)
            border.color: sv.finished ? (sv.cliff ? Theme.danger : Theme.success) : Theme.accent
            border.width: 1

            Text {
                id: stateText
                anchors.centerIn: parent
                text: sv.currentPos
                font.pixelSize: 9; font.bold: true
                color: sv.finished ? (sv.cliff ? Theme.danger : Theme.success) : Theme.accent
            }
        }
    }

    // ── Finish overlay when fell off cliff ───────────────────────────────────
    Rectangle {
        anchors.fill: parent
        color: Qt.rgba(Theme.danger.r, Theme.danger.g, Theme.danger.b, 0.12)
        visible: sv.finished && sv.cliff
        radius: 0

        Text {
            anchors.centerIn: parent
            text: "💀  FELL OFF CLIFF"
            font.pixelSize: 22; font.bold: true; font.letterSpacing: 3
            color: Theme.danger
            style: Text.Outline
            styleColor: Qt.rgba(0, 0, 0, 0.8)
        }
    }

    Rectangle {
        anchors.fill: parent
        color: Qt.rgba(Theme.success.r, Theme.success.g, Theme.success.b, 0.08)
        visible: sv.finished && !sv.cliff
        radius: 0

        Text {
            anchors.centerIn: parent
            text: "☕  RETURNED TO CAFÉ"
            font.pixelSize: 22; font.bold: true; font.letterSpacing: 3
            color: Theme.success
            style: Text.Outline
            styleColor: Qt.rgba(0, 0, 0, 0.8)
        }
    }
}
