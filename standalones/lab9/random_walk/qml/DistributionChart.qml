import QtQuick
import "."

Item {
    id: chart
    property var distribution: []

    function updateData() {
        distribution = walk_controller.finalDistributionPMF()
        canvas.requestPaint()
    }

    Connections { target: walk_controller; function onDistribution_ready() { chart.updateData() } }

    Canvas {
        id: canvas
        anchors.fill: parent
        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)
            if (chart.distribution.length === 0) {
                ctx.fillStyle = Theme.muted.toString()
                ctx.font = "12px monospace"
                ctx.textAlign = "center"
                ctx.fillText("No distribution computed", width / 2, height / 2)
                return
            }
            var padL = 40, padR = 20, padT = 20, padB = 40
            var w = width - padL - padR
            var h = height - padT - padB
            if (w <= 0 || h <= 0) return

            var maxProb = 0
            for (var i = 0; i < chart.distribution.length; i++)
                if (chart.distribution[i].probability > maxProb)
                    maxProb = chart.distribution[i].probability

            var values = chart.distribution.map(p => p.value).sort((a, b) => a - b)
            var minVal = values[0]
            var maxVal = values[values.length - 1]
            var range = maxVal - minVal || 1

            for (var j = 0; j < chart.distribution.length; j++) {
                var v = chart.distribution[j].value
                var prob = chart.distribution[j].probability
                var x = padL + ((v - minVal) / range) * w
                var barW = Math.max(2, w / chart.distribution.length * 0.7)
                var barH = (prob / maxProb) * h
                ctx.fillStyle = Theme.accent.toString()
                ctx.fillRect(x - barW / 2, padT + h - barH, barW, barH)
            }

            ctx.strokeStyle = Theme.border.toString()
            ctx.beginPath()
            ctx.moveTo(padL, padT)
            ctx.lineTo(padL, padT + h)
            ctx.lineTo(padL + w, padT + h)
            ctx.stroke()

            ctx.fillStyle = Theme.muted.toString()
            ctx.font = "10px monospace"
            ctx.textAlign = "center"
            ctx.fillText("Position", padL + w / 2, height - 8)
            ctx.save()
            ctx.translate(15, padT + h / 2)
            ctx.rotate(-Math.PI / 2)
            ctx.fillText("Probability", 0, 0)
            ctx.restore()
        }
    }
}
