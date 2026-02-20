import QtQuick

Item {
    id: graphView

    property bool simLoaded: false
    property color colHealthy:   "#10b981"
    property color colInfected:  "#ef4444"
    property color colRecovered: "#3b82f6"
    property color colBg:        "#0a0e1a"
    property color colEdge:      "#1e3050"
    property color colMuted:     "#64748b"
    property color colTxt:       "#e2e8f0"
    property color colCard:      "#1a2235"
    property color colBorder:    "#2a3a55"
    property color colAccent:    "#3b82f6"
    property color colWarn:      "#f59e0b"

    function loadGraph() {
        internal.fetchAndBuild(600)
    }

    function updateStates(nodes) {
        if (!nodes || nodes.length === 0) return
        var lookup = {}
        for (var i = 0; i < nodes.length; i++)
            lookup[nodes[i].id] = nodes[i].state
        for (var j = 0; j < internal.subNodes.length; j++) {
            var nd = internal.subNodes[j]
            if (lookup[nd.id] !== undefined)
                nd.state = lookup[nd.id]
        }
        canvas.requestPaint()
    }

    onSimLoadedChanged: {
        if (simLoaded) {
            internal.totalNodes = sim_controller.healthy_count
                + sim_controller.infected_count
                + sim_controller.recovered_count
            loadGraph()
        }
    }

    Connections {
        target: sim_controller
        function onSimulation_updated() {
            if (graphView.simLoaded)
                graphView.updateStates(sim_controller.get_node_states())
        }
        function onStats_changed() {
            if (graphView.simLoaded)
                graphView.updateStates(sim_controller.get_node_states())
        }
    }

    QtObject {
        id: internal

        property var subNodes: []
        property var subEdgeIndices: []
        property var nodePos: []
        property var nodeVel: []
        property int totalNodes: 0

        function fetchAndBuild(n) {
            var sub = sim_controller.get_bfs_subgraph(n)
            var nodes = sub["nodes"]
            var edges = sub["edges"]

            subNodes = []
            for (var i = 0; i < nodes.length; i++)
                subNodes.push({ id: nodes[i].id, state: nodes[i].state })

            var idxMap = {}
            for (var j = 0; j < subNodes.length; j++)
                idxMap[subNodes[j].id] = j

            subEdgeIndices = []
            for (var e = 0; e < edges.length; e++) {
                var ai = idxMap[edges[e].a]
                var bi = idxMap[edges[e].b]
                if (ai !== undefined && bi !== undefined)
                    subEdgeIndices.push({ ai: ai, bi: bi })
            }

            initPositions()
            runLayout()
            canvas.requestPaint()
            statusText.text = totalNodes + " nodes total  ·  showing "
                + subNodes.length + " nodes, " + subEdgeIndices.length + " edges"
        }

        function initPositions() {
            nodePos = []; nodeVel = []
            var n = subNodes.length
            for (var i = 0; i < n; i++) {
                var angle = (2 * Math.PI * i) / n
                nodePos.push({ x: 0.5 + 0.35 * Math.cos(angle), y: 0.5 + 0.35 * Math.sin(angle) })
                nodeVel.push({ vx: 0, vy: 0 })
            }
        }

        function runLayout() {
            var n = subNodes.length
            if (n === 0) return
            var k = Math.sqrt(1.0 / n)
            var temp = 0.1
            var cooling = 0.95
            var repStep = n > 300 ? 3 : 1

            for (var iter = 0; iter < 120; iter++) {
                for (var i = 0; i < n; i++) {
                    var fx = 0, fy = 0
                    for (var j = 0; j < n; j += repStep) {
                        if (i === j) continue
                        var dx = nodePos[i].x - nodePos[j].x
                        var dy = nodePos[i].y - nodePos[j].y
                        var dist = Math.sqrt(dx*dx + dy*dy) + 0.0001
                        var rep = (k * k) / dist
                        fx += (dx / dist) * rep
                        fy += (dy / dist) * rep
                    }
                    nodeVel[i].vx = (nodeVel[i].vx + fx) * 0.5
                    nodeVel[i].vy = (nodeVel[i].vy + fy) * 0.5
                }
                for (var e = 0; e < subEdgeIndices.length; e++) {
                    var ai = subEdgeIndices[e].ai, bi = subEdgeIndices[e].bi
                    var ex = nodePos[ai].x - nodePos[bi].x
                    var ey = nodePos[ai].y - nodePos[bi].y
                    var edist = Math.sqrt(ex*ex + ey*ey) + 0.0001
                    var att = (edist * edist) / k
                    var fax = (ex / edist) * att * 0.5
                    var fay = (ey / edist) * att * 0.5
                    nodeVel[ai].vx -= fax; nodeVel[ai].vy -= fay
                    nodeVel[bi].vx += fax; nodeVel[bi].vy += fay
                }
                for (var vi = 0; vi < n; vi++) {
                    var vmag = Math.sqrt(nodeVel[vi].vx*nodeVel[vi].vx + nodeVel[vi].vy*nodeVel[vi].vy) + 0.0001
                    var scale = Math.min(vmag, temp) / vmag
                    nodePos[vi].x = Math.max(0.02, Math.min(0.98, nodePos[vi].x + nodeVel[vi].vx * scale))
                    nodePos[vi].y = Math.max(0.02, Math.min(0.98, nodePos[vi].y + nodeVel[vi].vy * scale))
                }
                temp *= cooling
            }
        }
    }

    property real viewScale: 1.0
    property real viewOffX:  0.0
    property real viewOffY:  0.0

    Canvas {
        id: canvas
        anchors.fill: parent

        onPaint: {
            var ctx = getContext("2d")
            var W = width, H = height
            ctx.clearRect(0, 0, W, H)
            ctx.fillStyle = graphView.colBg
            ctx.fillRect(0, 0, W, H)

            var nodes = internal.subNodes
            var edges = internal.subEdgeIndices
            var pos   = internal.nodePos
            if (nodes.length === 0) return

            var sc = graphView.viewScale
            var ox = graphView.viewOffX
            var oy = graphView.viewOffY
            var nr = Math.max(2, 5 * sc)

            function px(nx) { return ox + nx * W * sc }
            function py(ny) { return oy + ny * H * sc }

            ctx.strokeStyle = graphView.colEdge
            ctx.lineWidth = Math.max(0.3, 0.6 * sc)
            ctx.globalAlpha = 0.45
            ctx.beginPath()
            for (var e = 0; e < edges.length; e++) {
                var ai = edges[e].ai, bi = edges[e].bi
                ctx.moveTo(px(pos[ai].x), py(pos[ai].y))
                ctx.lineTo(px(pos[bi].x), py(pos[bi].y))
            }
            ctx.stroke()
            ctx.globalAlpha = 1.0

            for (var i = 0; i < nodes.length; i++) {
                var s = nodes[i].state
                var col = s === 1 ? graphView.colInfected
                        : s === 2 ? graphView.colRecovered
                        : graphView.colHealthy
                var nx = px(pos[i].x), ny = py(pos[i].y)
                if (s === 1) {
                    ctx.beginPath()
                    ctx.arc(nx, ny, nr * 2.5, 0, 2 * Math.PI)
                    ctx.fillStyle = Qt.rgba(0.94, 0.27, 0.27, 0.12)
                    ctx.fill()
                }
                ctx.beginPath()
                ctx.arc(nx, ny, nr, 0, 2 * Math.PI)
                ctx.fillStyle = col
                ctx.fill()
            }
        }
    }

    MouseArea {
        anchors.fill: parent
        acceptedButtons: Qt.LeftButton
        property real lastX: 0; property real lastY: 0
        onPressed:  function(m) { lastX = m.x; lastY = m.y }
        onPositionChanged: function(m) {
            graphView.viewOffX += m.x - lastX
            graphView.viewOffY += m.y - lastY
            lastX = m.x; lastY = m.y
            canvas.requestPaint()
        }
        onWheel: function(w) {
            var f = w.angleDelta.y > 0 ? 1.15 : 0.87
            var ns = Math.max(0.1, Math.min(12.0, graphView.viewScale * f))
            graphView.viewOffX = w.x - (w.x - graphView.viewOffX) * (ns / graphView.viewScale)
            graphView.viewOffY = w.y - (w.y - graphView.viewOffY) * (ns / graphView.viewScale)
            graphView.viewScale = ns
            canvas.requestPaint()
        }
        cursorShape: Qt.OpenHandCursor
    }

    // zoom controls overlay
    Rectangle {
        anchors.left: parent.left; anchors.bottom: parent.bottom; anchors.margins: 16
        width: zoomRow.implicitWidth + 24; height: 48
        color: Qt.rgba(0.07, 0.09, 0.13, 0.88); radius: 8
        border.color: graphView.colBorder; border.width: 1

        Row {
            id: zoomRow
            anchors.centerIn: parent; spacing: 6
            Repeater {
                model: [
                    { lbl: "+", act: function() { graphView.viewScale = Math.min(12, graphView.viewScale * 1.25); canvas.requestPaint() } },
                    { lbl: "−", act: function() { graphView.viewScale = Math.max(0.1, graphView.viewScale * 0.8);  canvas.requestPaint() } },
                    { lbl: "⌂", act: function() { graphView.viewScale = 1; graphView.viewOffX = 0; graphView.viewOffY = 0; canvas.requestPaint() } }
                ]
                Rectangle {
                    width: 32; height: 32; radius: 4
                    color: zoomMa.containsMouse ? "#2a3a55" : "transparent"
                    border.color: graphView.colBorder; border.width: 1
                    Text { anchors.centerIn: parent; text: modelData.lbl; color: graphView.colTxt; font.pixelSize: 16; font.bold: true }
                    MouseArea { id: zoomMa; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor; onClicked: modelData.act() }
                }
            }
        }
    }

    // legend
    Rectangle {
        anchors.right: parent.right; anchors.bottom: parent.bottom; anchors.margins: 16
        width: 130; height: 86
        color: Qt.rgba(0.07, 0.09, 0.13, 0.88); radius: 8
        border.color: graphView.colBorder; border.width: 1
        Column {
            anchors.centerIn: parent; spacing: 10
            Repeater {
                model: [
                    { col: graphView.colHealthy,   lbl: "Healthy"   },
                    { col: graphView.colInfected,  lbl: "Infected"  },
                    { col: graphView.colRecovered, lbl: "Recovered" }
                ]
                Row {
                    spacing: 8
                    Rectangle { width: 10; height: 10; radius: 5; color: modelData.col; anchors.verticalCenter: parent.verticalCenter }
                    Text { text: modelData.lbl; color: graphView.colTxt; font.pixelSize: 12 }
                }
            }
        }
    }

    Text {
        id: statusText
        anchors.top: parent.top; anchors.horizontalCenter: parent.horizontalCenter; anchors.topMargin: 12
        text: "Load a graph to visualize"; color: graphView.colMuted; font.pixelSize: 11
    }

    // placeholder
    Column {
        anchors.centerIn: parent; spacing: 14; visible: !graphView.simLoaded
        Text { anchors.horizontalCenter: parent.horizontalCenter; text: "🕸️"; font.pixelSize: 52 }
        Text { anchors.horizontalCenter: parent.horizontalCenter; text: "Graph will appear here after loading"; color: graphView.colMuted; font.pixelSize: 14 }
        Text { anchors.horizontalCenter: parent.horizontalCenter; text: "Pan: drag  ·  Zoom: scroll or buttons"; color: "#3a4a65"; font.pixelSize: 11 }
    }
}
