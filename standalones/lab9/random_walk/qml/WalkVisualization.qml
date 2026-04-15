import QtQuick
import "."

Item {
    id: root

    property double currentPos: walk_controller.current_position
    property double startPos: walk_controller.start_position
    property int stepsDone: walk_controller.steps_done
    property int totalSteps: walk_controller.total_steps
    property bool finished: walk_controller.simulation_finished

    property double viewMin: -20
    property double viewMax: 20
    property double viewWidth: viewMax - viewMin

    readonly property double posX:
        (currentPos - viewMin) / viewWidth * width

    Rectangle {
        anchors.fill: parent
        color: Theme.bg
    }

    Rectangle {
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 40
        width: parent.width
        height: 2
        color: Theme.border
    }

    Repeater {
        model: 11

        delegate: Item {
            readonly property double step: root.viewWidth / 10.0
            readonly property double tickVal: root.viewMin + index * step
            readonly property double tickX:
                (tickVal - root.viewMin) / root.viewWidth * root.width

            x: tickX - 1
            y: root.height - 50
            width: 2
            height: 10

            Rectangle {
                width: 1
                height: 8
                color: Theme.muted
                anchors.bottom: parent.bottom
            }

            Text {
                text: tickVal.toFixed(1)
                color: Theme.muted
                font.pixelSize: 10
                anchors.top: parent.bottom
                anchors.topMargin: 2
                anchors.horizontalCenter: parent.horizontalCenter
            }
        }
    }

    Rectangle {
        x: (startPos - root.viewMin) / root.viewWidth * root.width - 12
        y: root.height - 80
        width: 24
        height: 24
        radius: 12
        color: Qt.rgba(Theme.accent.r, Theme.accent.g, Theme.accent.b, 0.3)
        border.color: Theme.accent

        Text {
            anchors.centerIn: parent
            text: "S"
            color: Theme.accent
            font.bold: true
        }

        Text {
            text: "start"
            color: Theme.accent
            font.pixelSize: 8
            anchors.top: parent.bottom
            anchors.horizontalCenter: parent.horizontalCenter
        }
    }

    Item {
        id: point

        x: root.posX - 16
        y: root.height - 80 - 16
        width: 32
        height: 32

        Behavior on x {
            NumberAnimation {
                duration: 400
                easing.type: Easing.InOutCubic
            }
        }

        Rectangle {
            anchors.fill: parent
            radius: 16
            color: finished ? Theme.success : Theme.warn
            border.color: "white"
            border.width: 2

            Text {
                anchors.centerIn: parent
                text: finished ? "🏁" : "●"
                color: "white"
                font.pixelSize: 18
            }
        }
    }

    Text {
        anchors.bottom: parent.bottom
        anchors.right: parent.right
        text: stepsDone + " / " + totalSteps + " steps"
        color: Theme.muted
        font.pixelSize: 12
    }
}