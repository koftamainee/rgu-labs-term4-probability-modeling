import QtQuick
import QtQuick.Layouts

Rectangle {
    id: tab
    property string label: ""
    property bool highlight: false
    property color accent: "#3b82f6"
    signal clicked()

    height: 32
    radius: 4
    color: highlight ? accent : "transparent"
    border.color: highlight ? accent : "#2a3a55"
    border.width: 1

    Text {
        anchors.centerIn: parent
        text: tab.label
        color: "#e2e8f0"
        font.pixelSize: 11
        font.bold: true
        font.letterSpacing: 1
    }

    MouseArea {
        anchors.fill: parent
        cursorShape: Qt.PointingHandCursor
        onClicked: tab.clicked()
    }
}
