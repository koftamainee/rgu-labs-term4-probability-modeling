import QtQuick
import QtQuick.Layouts
import "."

ColumnLayout {
    id: sb
    property string label: ""
    property int value: 0
    property color accent: Theme.accent
    property int fraction: 0   // 0 = hide sub-label; >0 = show value/fraction ratio

    spacing: 2

    Text {
        text: sb.label
        color: Theme.muted
        font.pixelSize: 10
        font.letterSpacing: 2
        font.bold: true
    }
    Text {
        text: sb.value.toLocaleString()
        color: sb.accent
        font.pixelSize: 26
        font.bold: true
    }
    Text {
        visible: sb.fraction > 0
        text: sb.fraction > 0 ? (sb.value / sb.fraction).toFixed(4) : ""
        color: Qt.rgba(sb.accent.r, sb.accent.g, sb.accent.b, 0.6)
        font.pixelSize: 11
    }
}
