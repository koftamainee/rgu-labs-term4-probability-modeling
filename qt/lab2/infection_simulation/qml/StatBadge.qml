import QtQuick
import QtQuick.Layouts

ColumnLayout {
    id: sb
    property string label: ""
    property int value: 0
    property color accent: "#ffffff"
    property int fraction: 1

    spacing: 2

    Text {
        text: sb.label
        color: "#64748b"
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
        text: sb.fraction > 0 ? Math.round(sb.value * 100 / sb.fraction) + "%" : "—"
        color: Qt.rgba(sb.accent.r, sb.accent.g, sb.accent.b, 0.6)
        font.pixelSize: 11
    }
}
