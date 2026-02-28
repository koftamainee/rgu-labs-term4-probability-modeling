import QtQuick
import "."

Canvas {
    id: root

    property var pathNodes: []
    property int highlightTarget: -1
    property int branchFactor: 3
    property int treeDepth: 3

    property var  nodePos: []
    property int  nodeCount: 0
    property int  cachedR: 8

    property bool showNodeLabels: cachedR >= 9 && nodeCount <= 150
    property bool showEdgeLabels: cachedR >= 10 && nodeCount <= 80
    property bool showStuckLabels: cachedR >= 8 && nodeCount <= 120

    function computeR(nc) {
        if (width <= 0 || nc <= 0) return 8
        var leaves = Math.pow(branchFactor, treeDepth)
        return Math.max(5, Math.min(18, Math.floor(width / (leaves * 2.4))))
    }

    function rebuild() {
        if (width <= 0 || height <= 0) return

        var nc = treeCtrl.nodeCount
        nodeCount = nc
        if (nc === 0) {
            nodePos = [];
            requestPaint();
            return
        }

        var r = computeR(nc)
        cachedR = r
        var depth = treeDepth
        var m = branchFactor
        var leaves = Math.pow(m, depth)
        var leafSpacing = Math.max(r * 2.5, width / (leaves + 1))
        var levelH = (height - r * 4) / (depth + 1)

        function span(idx) {
            var lv = 0, base = 1, start = 0
            while (start + base <= idx) {
                start += base;
                base *= m;
                lv++
            }
            var pos = idx - start
            var sz = Math.pow(m, depth - lv)
            return {lo: pos * sz, hi: (pos + 1) * sz, level: lv}
        }

        var pos = new Array(nc)
        for (var i = 0; i < nc; i++) {
            var s = span(i)
            pos[i] = {
                x: leafSpacing * (s.lo + s.hi) / 2 + leafSpacing / 2,
                y: r * 2 + levelH * s.level
            }
        }
        nodePos = pos
        requestPaint()
    }

    onWidthChanged: Qt.callLater(rebuild)
    onHeightChanged: Qt.callLater(rebuild)

    onPaint: {
        var ctx = getContext("2d")
        ctx.clearRect(0, 0, width, height)
        if (!nodePos || nodePos.length === 0) return

        var nc = nodePos.length
        var r = cachedR
        var pathSet = {}
        var pathEdges = {}

        for (var i = 0; i < pathNodes.length; i++) {
            var isLast = (i === pathNodes.length - 1)
            pathSet[pathNodes[i]] = isLast
                ? (pathNodes[i] === highlightTarget ? "target" : "stuck")
                : "path"
        }
        for (var j = 0; j < pathNodes.length - 1; j++) {
            var a = pathNodes[j], b = pathNodes[j + 1]
            pathEdges[Math.min(a, b) + "_" + Math.max(a, b)] = true
        }

        for (var ni = 1; ni < nc; ni++) {
            var par = treeCtrl.parentOf(ni)
            if (par < 0 || par >= nodePos.length) continue

            var px = nodePos[par].x, py = nodePos[par].y
            var cx = nodePos[ni].x, cy = nodePos[ni].y
            var onPath = !!pathEdges[Math.min(par, ni) + "_" + Math.max(par, ni)]

            ctx.strokeStyle = onPath ? "#8b5cf6" : "#3a2f60"
            ctx.lineWidth = onPath ? 2.5 : 1
            ctx.setLineDash([])
            ctx.shadowBlur = 0
            ctx.beginPath()
            ctx.moveTo(px, py)
            ctx.lineTo(cx, cy)
            ctx.stroke()

            // Edge weight label
            if (showEdgeLabels) {
                var weights = treeCtrl.nodeChildWeights(par)
                var firstChild = treeCtrl.childStart(par)
                var childIdx = ni - firstChild
                if (childIdx >= 0 && childIdx < weights.length) {
                    var w = weights[childIdx]
                    var ex = (px + cx) / 2
                    var ey = (py + cy) / 2
                    var lbl = w >= 0.001 ? w.toFixed(2) : "~0"
                    ctx.save()
                    ctx.font = "bold 9px sans-serif"
                    var tw = ctx.measureText(lbl).width
                    ctx.fillStyle = "#0f0c1f"
                    ctx.fillRect(ex - tw / 2 - 2, ey - 7, tw + 4, 13)
                    ctx.fillStyle = onPath ? "#c4b5fd" : "#6d6a8a"
                    ctx.textAlign = "center"
                    ctx.textBaseline = "middle"
                    ctx.fillText(lbl, ex, ey)
                    ctx.restore()
                }
            }
        }

        for (var k = 0; k < nc; k++) {
            var x = nodePos[k].x, y = nodePos[k].y
            var isLeafNode = treeCtrl.isLeaf(k)
            var isTarget = (k === highlightTarget)
            var pstate = pathSet[k] || ""

            var fillCol
            if (pstate === "target" || isTarget) fillCol = "#fbbf24"
            else if (pstate === "stuck") fillCol = "#f87171"
            else if (pstate === "path") fillCol = "#8b5cf6"
            else if (isLeafNode) fillCol = "#1b1733"
            else fillCol = "#2a244a"

            ctx.shadowColor = pstate ? fillCol : "transparent"
            ctx.shadowBlur = pstate ? 10 : 0

            ctx.beginPath()
            ctx.arc(x, y, r, 0, Math.PI * 2)
            ctx.fillStyle = fillCol
            ctx.fill()
            ctx.strokeStyle = isTarget ? "#fbbf24" : (pstate ? "#e0d9ff" : "#3a2f60")
            ctx.lineWidth = isTarget ? 2 : 1
            ctx.stroke()
            ctx.shadowBlur = 0

            if (showNodeLabels) {
                ctx.fillStyle = "#e0d9ff"
                ctx.font = "bold " + Math.max(7, r - 4) + "px sans-serif"
                ctx.textAlign = "center"
                ctx.textBaseline = "middle"
                ctx.fillText(k, x, y)
            }

            if (showStuckLabels && !isLeafNode) {
                var sp = treeCtrl.nodeStuckProb(k)
                if (sp > 0) {
                    var spLbl = "p=" + sp.toFixed(2)
                    ctx.save()
                    ctx.font = "9px sans-serif"
                    var spW = ctx.measureText(spLbl).width
                    ctx.fillStyle = "#0f0c1f"
                    ctx.fillRect(x - spW / 2 - 2, y + r + 1, spW + 4, 12)
                    ctx.fillStyle = pstate === "stuck" ? "#f87171" : "#a5a1b8"
                    ctx.textAlign = "center"
                    ctx.textBaseline = "top"
                    ctx.fillText(spLbl, x, y + r + 2)
                    ctx.restore()
                }
            }
        }
    }
}
