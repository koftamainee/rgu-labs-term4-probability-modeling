import QtQuick
import "."

Item {
    id: root
    property var histData: []

    Canvas {
        id: cv
        anchors.fill: parent
        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            if (!root.histData || root.histData.length === 0) {
                ctx.fillStyle = "#a5a1b8"
                ctx.font = "12px sans-serif"
                ctx.textAlign = "center"
                ctx.textBaseline = "middle"
                ctx.fillText("No data yet", width / 2, height / 2)
                return
            }

            var data = root.histData.filter(function (d) {
                return d.length >= 1
            })
            if (data.length === 0) return

            var maxCount = 0
            for (var i = 0; i < data.length; i++)
                if (data[i].count > maxCount) maxCount = data[i].count

            if (maxCount === 0) return

            var n = data.length
            var pad = 28
            var chartW = width - pad * 2
            var chartH = height - pad * 2
            var barW = Math.max(4, chartW / n - 2)

            ctx.strokeStyle = "#3a2f60"
            ctx.lineWidth = 1
            ctx.beginPath()
            ctx.moveTo(pad, pad)
            ctx.lineTo(pad, pad + chartH)
            ctx.lineTo(pad + chartW, pad + chartH)
            ctx.stroke()

            for (var j = 0; j < n; j++) {
                var item = data[j]
                var bh = (item.count / maxCount) * chartH
                var bx = pad + j * (chartW / n)
                var by = pad + chartH - bh

                ctx.fillStyle = "#8b5cf6"
                ctx.globalAlpha = 0.7
                ctx.fillRect(bx + 1, by, barW, bh)
                ctx.globalAlpha = 1.0

                if (barW >= 14) {
                    ctx.fillStyle = "#a5a1b8"
                    ctx.font = "9px sans-serif"
                    ctx.textAlign = "center"
                    ctx.textBaseline = "top"
                    ctx.fillText(item.length, bx + barW / 2, pad + chartH + 4)
                }
            }

            ctx.fillStyle = "#a5a1b8"
            ctx.font = "9px sans-serif"
            ctx.textAlign = "right"
            ctx.textBaseline = "top"
            ctx.fillText(maxCount, pad - 2, pad)

            ctx.fillStyle = "#a5a1b8"
            ctx.font = "10px sans-serif"
            ctx.textAlign = "center"
            ctx.textBaseline = "bottom"
            ctx.fillText("Path length (vertices)", width / 2, height - 2)
        }
    }

    onHistDataChanged: cv.requestPaint()
    onWidthChanged: cv.requestPaint()
    onHeightChanged: cv.requestPaint()
}
