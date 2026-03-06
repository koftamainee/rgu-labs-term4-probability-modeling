import QtQuick
import "."

Item {
    id: chart

    property var  dataPoints: []
    property real currentP:   walk_controller.p_forward
    property int  cafePos:    walk_controller.cafe_pos
    property real cliffProb:  walk_controller.cliff_probability
    property real cafeProb:   walk_controller.cafe_probability

    onDataPointsChanged: canvas.requestPaint()
    onCurrentPChanged:   canvas.requestPaint()
    onCafePosChanged:    canvas.requestPaint()
    onCliffProbChanged:  canvas.requestPaint()

    Canvas {
        id: canvas
        anchors.fill: parent

        onPaint: {
            var ctx = getContext("2d")
            ctx.clearRect(0, 0, width, height)

            var padL = 44, padR = 24, padT = 24, padB = 44
            var W = width  - padL - padR
            var H = height - padT - padB
            if (W <= 0 || H <= 0) return

            var pts = chart.dataPoints
            var bMax = (pts.length > 0) ? pts[pts.length - 1].p : 20

            // ── Grid ─────────────────────────────────────────────────────────
            ctx.strokeStyle = "#2a2048"
            ctx.lineWidth = 1

            // Horizontal — 0.0 to 1.0
            for (var i = 0; i <= 5; i++) {
                var val = i / 5.0
                var yg = padT + H - val * H
                ctx.beginPath(); ctx.moveTo(padL, yg); ctx.lineTo(padL + W, yg); ctx.stroke()
                ctx.fillStyle = "#a5a1b8"; ctx.font = "10px monospace"
                ctx.textAlign = "right"
                ctx.fillText(val.toFixed(1), padL - 6, yg + 4)
            }

            // Vertical — B values
            var bStep = Math.max(1, Math.floor(bMax / 8))
            for (var b = 1; b <= bMax; b += bStep) {
                var xg = padL + ((b - 1) / (bMax - 1)) * W
                ctx.beginPath(); ctx.moveTo(xg, padT); ctx.lineTo(xg, padT + H); ctx.stroke()
                ctx.fillStyle = "#a5a1b8"; ctx.font = "10px monospace"
                ctx.textAlign = "center"
                ctx.fillText(b, xg, padT + H + 16)
            }

            // Axis labels
            ctx.fillStyle = "#a5a1b8"; ctx.font = "11px monospace"
            ctx.textAlign = "center"
            ctx.fillText("B  (café distance from cliff)", padL + W / 2, height - 4)
            ctx.save()
            ctx.translate(11, padT + H / 2)
            ctx.rotate(-Math.PI / 2)
            ctx.fillText("probability", 0, 0)
            ctx.restore()

            // p label top right
            ctx.fillStyle = "#fbbf24"; ctx.font = "bold 12px monospace"
            ctx.textAlign = "right"
            ctx.fillText("p = " + chart.currentP.toFixed(2) + "   q = " + (1 - chart.currentP).toFixed(2),
                         padL + W, padT - 6)

            if (pts.length < 2) {
                ctx.fillStyle = "#a5a1b8"; ctx.font = "13px monospace"; ctx.textAlign = "center"
                ctx.fillText("Click \"Build Probability Curve\"", padL + W / 2, padT + H / 2)
                return
            }

            function bToX(b) { return padL + ((b - 1) / (bMax - 1)) * W }

            // ── Theoretical curve: Gambler's Ruin, barriers 0 and B, start=B ──
            // P(cliff|start=B) = q * P(cliff|k=B-1)
            // P(cliff|k) = [(q/p)^k - (q/p)^B] / [1-(q/p)^B]  p≠0.5
            // P(cliff|k) = 1-k/B                                 p=0.5
            ctx.beginPath()
            ctx.strokeStyle = "rgba(251,191,36,0.55)"
            ctx.lineWidth = 1.5
            ctx.setLineDash([4, 3])
            var p = chart.currentP, q = 1 - p
            var firstT = true
            for (var bt = 1; bt <= bMax; bt += 0.2) {
                var k = bt - 1
                var cliffT
                if (k <= 0) {
                    cliffT = 0
                } else if (Math.abs(p - 0.5) < 1e-9) {
                    cliffT = q * (1.0 - k / bt)
                } else {
                    var r = q / p
                    var rB = Math.pow(r, bt)
                    var denom = 1.0 - rB
                    var probK = Math.abs(denom) < 1e-12 ? 0 : (Math.pow(r, k) - rB) / denom
                    cliffT = q * probK
                }
                cliffT = Math.max(0, Math.min(1, cliffT))
                var xt = bToX(bt), yt = padT + H - cliffT * H
                if (firstT) { ctx.moveTo(xt, yt); firstT = false }
                else ctx.lineTo(xt, yt)
            }
            ctx.stroke()
            ctx.setLineDash([])

            // ── Cliff fill ────────────────────────────────────────────────────
            ctx.beginPath()
            for (var k = 0; k < pts.length; k++) {
                var dp = pts[k]
                var px = bToX(dp.p), py = padT + H - dp.cliff_prob * H
                if (k === 0) ctx.moveTo(px, py); else ctx.lineTo(px, py)
            }
            ctx.lineTo(bToX(pts[pts.length-1].p), padT + H)
            ctx.lineTo(bToX(pts[0].p), padT + H)
            ctx.closePath()
            ctx.fillStyle = "rgba(248,113,113,0.07)"; ctx.fill()

            // ── Cliff line ────────────────────────────────────────────────────
            ctx.beginPath()
            ctx.strokeStyle = "#f87171"; ctx.lineWidth = 2.5; ctx.lineJoin = "round"
            for (var k2 = 0; k2 < pts.length; k2++) {
                var dp2 = pts[k2]
                var px2 = bToX(dp2.p), py2 = padT + H - dp2.cliff_prob * H
                if (k2 === 0) ctx.moveTo(px2, py2); else ctx.lineTo(px2, py2)
            }
            ctx.stroke()

            // ── Café line ─────────────────────────────────────────────────────
            ctx.beginPath()
            ctx.strokeStyle = "#34d399"; ctx.lineWidth = 2.5; ctx.lineJoin = "round"
            for (var m2 = 0; m2 < pts.length; m2++) {
                var dpm = pts[m2]
                var pxm = bToX(dpm.p), pym = padT + H - dpm.cafe_prob * H
                if (m2 === 0) ctx.moveTo(pxm, pym); else ctx.lineTo(pxm, pym)
            }
            ctx.stroke()

            // ── Current B marker ─────────────────────────────────────────────
            var curB = chart.cafePos
            if (curB >= 1 && curB <= bMax) {
                var curX = bToX(curB)
                ctx.beginPath(); ctx.strokeStyle = "#fbbf24"; ctx.lineWidth = 1.5
                ctx.setLineDash([5, 4])
                ctx.moveTo(curX, padT); ctx.lineTo(curX, padT + H); ctx.stroke()
                ctx.setLineDash([])

                // Dots at current B
                if (chart.cliffProb >= 0) {
                    var dy = padT + H - chart.cliffProb * H
                    ctx.beginPath(); ctx.arc(curX, dy, 5, 0, Math.PI * 2)
                    ctx.fillStyle = "#f87171"; ctx.fill()
                    ctx.strokeStyle = "#0f0c1f"; ctx.lineWidth = 1.5; ctx.stroke()
                    ctx.fillStyle = "#f87171"; ctx.font = "bold 10px monospace"; ctx.textAlign = "left"
                    ctx.fillText(chart.cliffProb.toFixed(3), curX + 8, dy + 4)
                }
                if (chart.cafeProb >= 0) {
                    var dy2 = padT + H - chart.cafeProb * H
                    ctx.beginPath(); ctx.arc(curX, dy2, 5, 0, Math.PI * 2)
                    ctx.fillStyle = "#34d399"; ctx.fill()
                    ctx.strokeStyle = "#0f0c1f"; ctx.lineWidth = 1.5; ctx.stroke()
                    ctx.fillStyle = "#34d399"; ctx.font = "bold 10px monospace"; ctx.textAlign = "left"
                    ctx.fillText(chart.cafeProb.toFixed(3), curX + 8, dy2 + 4)
                }
                ctx.fillStyle = "#fbbf24"; ctx.font = "bold 11px monospace"; ctx.textAlign = "center"
                ctx.fillText("B=" + curB, curX, padT - 6)
            }

            // ── Legend ────────────────────────────────────────────────────────
            var lx = padL + 10, ly = padT + 10
            ctx.fillStyle = "#f87171"; ctx.fillRect(lx, ly, 18, 3)
            ctx.fillStyle = "#e0d9ff"; ctx.font = "11px monospace"; ctx.textAlign = "left"
            ctx.fillText("P(cliff) empirical", lx + 22, ly + 4)
            ctx.fillStyle = "#34d399"; ctx.fillRect(lx, ly + 16, 18, 3)
            ctx.fillStyle = "#e0d9ff"
            ctx.fillText("P(café) empirical", lx + 22, ly + 20)
            ctx.strokeStyle = "rgba(251,191,36,0.7)"; ctx.lineWidth = 1.5
            ctx.setLineDash([3,3])
            ctx.beginPath(); ctx.moveTo(lx, ly + 32); ctx.lineTo(lx + 18, ly + 32); ctx.stroke()
            ctx.setLineDash([])
            ctx.fillStyle = "#e0d9ff"
            ctx.fillText("P(cliff) theory (Gambler's Ruin)", lx + 22, ly + 36)
        }
    }
}
