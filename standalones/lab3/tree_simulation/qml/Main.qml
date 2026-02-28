import QtQuick
import QtQuick.Layouts
import QtQuick.Controls.Basic
import "."

Window {
    id: root
    width: 1340
    height: 820
    minimumWidth: 1000
    minimumHeight: 640
    visible: true
    title: "Perfect M-ary Tree Walk Simulation"
    color: Theme.bg

    function rebuildTree() {
        treeView.branchFactor = treeCtrl.M
        treeView.treeDepth = treeCtrl.treeDepth
        treeView.pathNodes = []
        treeView.highlightTarget = treeCtrl.targetLeafIndex
        Qt.callLater(treeView.rebuild)
    }

    function refreshPath() {
        var raw = treeCtrl.lastPath
        var nodes = []
        for (var i = 0; i < raw.length; i++) nodes.push(raw[i].node)
        treeView.pathNodes = nodes
        treeView.highlightTarget = treeCtrl.targetLeafIndex
        treeView.requestPaint()
        histView.histData = treeCtrl.pathLengthHistogram()
    }

    Connections {
        target: treeCtrl

        function onTreeChanged() {
            rebuildTree()
        }

        function onPathChanged() {
            refreshPath()
        }

        function onStatsChanged() {
            histView.histData = treeCtrl.pathLengthHistogram()
        }

        function onInlineParamsChanged() {
            rebuildTree()
        }
    }

    Component.onCompleted: Qt.callLater(rebuildTree)

    // ── File picker ───────────────────────────────────────────────────────────
    FilePickerDialog {
        id: filePicker
        onAccepted: function (p) {
            treeCtrl.loadFile(p)
        }
    }

    RowLayout {
        anchors.fill: parent
        anchors.margins: 12
        spacing: 12

        Rectangle {
            Layout.preferredWidth: 280
            Layout.fillHeight: true
            color: Theme.surface; radius: 10
            border.color: Theme.border; border.width: 1

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 14
                spacing: 8

                Text {
                    text: "M-ARY TREE WALK"; color: Theme.accent; font.pixelSize: 13; font.bold: true; font.letterSpacing: 2
                }
                Text {
                    text: "Perfect tree — point motion simulator"; color: Theme.muted; font.pixelSize: 10; wrapMode: Text.Wrap; Layout.fillWidth: true
                }

                Rectangle {
                    height: 1; color: Theme.border; Layout.fillWidth: true
                }
                SectionLabel {
                    text: "CONFIGURATION"
                }

                AppButton {
                    visible: !treeCtrl.configLoaded
                    label: "Load File"; icon: "📂"
                    Layout.fillWidth: true
                    onClicked: filePicker.open()
                }
                RowLayout {
                    visible: treeCtrl.configLoaded
                    Layout.fillWidth: true
                    spacing: 6
                    AppButton {
                        label: "Load…"; icon: "📂"; Layout.fillWidth: true; onClicked: filePicker.open()
                    }
                    AppButton {
                        label: "↺"; accent: Theme.warn; Layout.preferredWidth: 36; onClicked: treeCtrl.reloadFile()
                    }
                    AppButton {
                        label: "Manual"; accent: Theme.muted; Layout.preferredWidth: 68; onClicked: treeCtrl.unloadFile()
                    }
                }

                Rectangle {
                    Layout.fillWidth: true; height: 30; radius: 6
                    color: treeCtrl.configError !== ""
                        ? Qt.rgba(Theme.danger.r, Theme.danger.g, Theme.danger.b, 0.15)
                        : treeCtrl.configLoaded
                            ? Qt.rgba(Theme.success.r, Theme.success.g, Theme.success.b, 0.12)
                            : Qt.rgba(Theme.warn.r, Theme.warn.g, Theme.warn.b, 0.10)
                    border.color: treeCtrl.configError !== "" ? Theme.danger : treeCtrl.configLoaded ? Theme.success : Theme.warn
                    border.width: 1
                    Text {
                        anchors.verticalCenter: parent.verticalCenter
                        anchors.left: parent.left; anchors.leftMargin: 10
                        anchors.right: parent.right; anchors.rightMargin: 6
                        font.pixelSize: 10; elide: Text.ElideLeft
                        color: treeCtrl.configError !== "" ? Theme.danger : treeCtrl.configLoaded ? Theme.success : Theme.warn
                        text: treeCtrl.configError !== "" ? "✕  " + treeCtrl.configError
                            : treeCtrl.configLoaded ? "✓  File loaded"
                                : "ℹ  Manual mode"
                    }
                }

                Rectangle {
                    height: 1; color: Theme.border; Layout.fillWidth: true
                }
                SectionLabel {
                    text: "TREE PARAMETERS"
                }

                ParamSlider {
                    label: "Branching factor M"
                    from: 2;
                    to: 6; stepSize: 1
                    value: treeCtrl.inlineM
                    Layout.fillWidth: true
                    enabled: !treeCtrl.configLoaded
                    opacity: treeCtrl.configLoaded ? 0.45 : 1.0
                    onMoved: function (v) {
                        treeCtrl.inlineM = Math.round(v)
                    }
                }
                ParamSlider {
                    label: "Tree depth"
                    from: 1;
                    to: 5; stepSize: 1
                    value: treeCtrl.inlineDepth
                    Layout.fillWidth: true
                    enabled: !treeCtrl.configLoaded
                    opacity: treeCtrl.configLoaded ? 0.45 : 1.0
                    onMoved: function (v) {
                        treeCtrl.inlineDepth = Math.round(v)
                    }
                }

                Rectangle {
                    height: 1; color: Theme.border; Layout.fillWidth: true
                }
                SectionLabel {
                    text: "MOTION PARAMETERS"
                }

                ParamSlider {
                    label: "Stuck probability p"
                    from: 0;
                    to: 0.99; stepSize: 0.01
                    value: treeCtrl.inlineStuckProb
                    sliderColor: Theme.danger
                    Layout.fillWidth: true
                    enabled: !treeCtrl.configLoaded
                    opacity: treeCtrl.configLoaded ? 0.45 : 1.0
                    onMoved: function (v) {
                        treeCtrl.inlineStuckProb = v
                    }
                }

                SectionLabel {
                    text: "CHILD SELECTION DISTRIBUTION"
                }

                Repeater {
                    model: ["Uniform", "Geometric", "Left-biased"]
                    delegate: Rectangle {
                        Layout.fillWidth: true; height: 28; radius: 5
                        enabled: !treeCtrl.configLoaded
                        opacity: treeCtrl.configLoaded ? 0.45 : 1.0
                        color: treeCtrl.inlineDist === index
                            ? Qt.rgba(Theme.accent.r, Theme.accent.g, Theme.accent.b, 0.2)
                            : "transparent"
                        border.color: treeCtrl.inlineDist === index ? Theme.accent : Theme.border
                        border.width: 1
                        Row {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left; anchors.leftMargin: 10; spacing: 6
                            Rectangle {
                                width: 7; height: 7; radius: 4; anchors.verticalCenter: parent.verticalCenter; color: treeCtrl.inlineDist === index ? Theme.accent : Theme.border
                            }
                            Text {
                                text: modelData; color: Theme.txt; font.pixelSize: 11; anchors.verticalCenter: parent.verticalCenter
                            }
                        }
                        MouseArea {
                            anchors.fill: parent; cursorShape: Qt.PointingHandCursor; onClicked: if (!treeCtrl.configLoaded) treeCtrl.inlineDist = index
                        }
                    }
                }

                ParamSlider {
                    visible: treeCtrl.inlineDist === 1
                    label: "Geometric r"
                    from: 0.05;
                    to: 0.95; stepSize: 0.05
                    value: treeCtrl.inlineGeomR
                    sliderColor: Theme.warn
                    Layout.fillWidth: true
                    enabled: !treeCtrl.configLoaded
                    opacity: treeCtrl.configLoaded ? 0.45 : 1.0
                    onMoved: function (v) {
                        treeCtrl.inlineGeomR = v
                    }
                }
                ParamSlider {
                    visible: treeCtrl.inlineDist === 2
                    label: "First child weight"
                    from: 0.1;
                    to: 0.9; stepSize: 0.05
                    value: treeCtrl.inlineLeftBias
                    sliderColor: Theme.warn
                    Layout.fillWidth: true
                    enabled: !treeCtrl.configLoaded
                    opacity: treeCtrl.configLoaded ? 0.45 : 1.0
                    onMoved: function (v) {
                        treeCtrl.inlineLeftBias = v
                    }
                }

                Rectangle {
                    height: 1; color: Theme.border; Layout.fillWidth: true
                }
                SectionLabel {
                    text: "TARGET LEAF"
                }

                RowLayout {
                    spacing: 8; Layout.fillWidth: true

                    Text {
                        text: "Leaf #"; color: Theme.muted; font.pixelSize: 12
                    }

                    Rectangle {
                        width: 72; height: 36; radius: 6
                        color: Theme.bg
                        border.color: leafInput.activeFocus ? Theme.accent : Theme.border
                        border.width: 1

                        TextInput {
                            id: leafInput
                            anchors.fill: parent; anchors.margins: 8
                            color: Theme.txt; font.pixelSize: 16; font.bold: true
                            verticalAlignment: TextInput.AlignVCenter
                            horizontalAlignment: TextInput.AlignHCenter
                            selectByMouse: true
                            text: treeCtrl.currentTargetLeaf.toString()
                            Connections {
                                target: treeCtrl

                                function onTreeChanged() {
                                    if (!leafInput.activeFocus)
                                        leafInput.text = treeCtrl.currentTargetLeaf.toString()
                                }

                                function onInlineParamsChanged() {
                                    if (!leafInput.activeFocus)
                                        leafInput.text = treeCtrl.currentTargetLeaf.toString()
                                }
                            }
                            onEditingFinished: {
                                var v = parseInt(text)
                                if (!isNaN(v) && v >= 0 && v < treeCtrl.leafCount)
                                    treeCtrl.inlineTargetLeaf = v
                                else
                                    text = treeCtrl.currentTargetLeaf.toString()
                            }
                        }
                    }

                    ColumnLayout {
                        spacing: 2
                        Rectangle {
                            width: 26; height: 17; radius: 4
                            color: upLeafMa.containsMouse ? Theme.card : "transparent"
                            border.color: Theme.border; border.width: 1
                            Text {
                                anchors.centerIn: parent; text: "▲"; color: Theme.accent; font.pixelSize: 8
                            }
                            MouseArea {
                                id:
                                    upLeafMa; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    var v = treeCtrl.inlineTargetLeaf + 1
                                    if (v < treeCtrl.leafCount) treeCtrl.inlineTargetLeaf = v
                                }
                            }
                        }
                        Rectangle {
                            width: 26; height: 17; radius: 4
                            color: dnLeafMa.containsMouse ? Theme.card : "transparent"
                            border.color: Theme.border; border.width: 1
                            Text {
                                anchors.centerIn: parent; text: "▼"; color: Theme.accent; font.pixelSize: 8
                            }
                            MouseArea {
                                id:
                                    dnLeafMa; anchors.fill: parent; hoverEnabled: true; cursorShape: Qt.PointingHandCursor
                                onClicked: {
                                    var v = treeCtrl.inlineTargetLeaf - 1
                                    if (v >= 0) treeCtrl.inlineTargetLeaf = v
                                }
                            }
                        }
                    }

                    Text {
                        text: "/ " + Math.max(0, treeCtrl.leafCount - 1); color: Theme.muted; font.pixelSize: 12
                    }
                }

                Item {
                    Layout.fillHeight: true
                }
                Rectangle {
                    height: 1; color: Theme.border; Layout.fillWidth: true
                }
                SectionLabel {
                    text: "SIMULATION"
                }

                AppButton {
                    label: "Step Once"; icon: "▶"; Layout.fillWidth: true; onClicked: treeCtrl.runOnce()
                }
                RowLayout {
                    spacing: 6
                    AppButton {
                        label: "× 100"; Layout.fillWidth: true; accent: Theme.warn; onClicked: treeCtrl.runBatch(100)
                    }
                    AppButton {
                        label: "× 1000"; Layout.fillWidth: true; accent: Theme.warn; onClicked: treeCtrl.runBatch(1000)
                    }
                    AppButton {
                        label: "× 10k"; Layout.fillWidth: true; accent: Theme.warn; onClicked: treeCtrl.runBatch(10000)
                    }
                }
                AppButton {
                    label: "Reset Stats"; icon: "↺"; accent: Theme.danger; Layout.fillWidth: true; onClicked: treeCtrl.resetStats()
                }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true; Layout.fillHeight: true; spacing: 8

            RowLayout {
                spacing: 14
                Repeater {
                    model: [
                        {col: "#2a244a", label: "Node"},
                        {col: "#8b5cf6", label: "Path"},
                        {col: "#fbbf24", label: "Target"},
                        {col: "#f87171", label: "Stuck"},
                        {col: "#1b1733", label: "Leaf"}
                    ]
                    delegate: RowLayout {
                        spacing: 5
                        Rectangle {
                            width: 11; height: 11; radius: 6; color: modelData.col; border.color: Theme.border; border.width: 1
                        }
                        Text {
                            text: modelData.label; color: Theme.muted; font.pixelSize: 11
                        }
                    }
                }
                Item {
                    Layout.fillWidth: true
                }
                Text {
                    text: "Nodes: " + treeCtrl.nodeCount + "   Leaves: " + treeCtrl.leafCount; color: Theme.muted; font.pixelSize: 11
                }
            }

            Rectangle {
                Layout.fillWidth: true; Layout.fillHeight: true
                color: Theme.card; radius: 10
                border.color: Theme.border; border.width: 1; clip: true

                TreeView {
                    id: treeView
                    anchors.fill: parent; anchors.margins: 8
                }
            }
        }

        Rectangle {
            Layout.preferredWidth: 230; Layout.fillHeight: true
            color: Theme.surface; radius: 10
            border.color: Theme.border; border.width: 1

            ColumnLayout {
                anchors.fill: parent; anchors.margins: 14; spacing: 10

                SectionLabel {
                    text: "STATISTICS"
                }
                StatBadge {
                    label: "TOTAL RUNS"; value: treeCtrl.totalRuns; accent: Theme.accent; fraction: 0
                }
                StatDivider {
                }
                StatBadge {
                    label: "TARGET HITS"; value: treeCtrl.targetHits; accent: Theme.success; fraction: 0
                }
                StatDivider {
                }

                ColumnLayout {
                    spacing: 3
                    Text {
                        text: "HIT PROBABILITY"; color: Theme.muted; font.pixelSize: 10; font.letterSpacing: 2; font.bold: true
                    }
                    Text {
                        text: treeCtrl.totalRuns > 0 ? treeCtrl.hitProbability.toFixed(4) : "—"
                        color: Theme.warn; font.pixelSize: 26; font.bold: true
                    }
                    Text {
                        text: "P(reach leaf " + treeCtrl.currentTargetLeaf + ")"
                        color: Theme.muted; font.pixelSize: 10
                    }
                }

                StatDivider {
                }
                SectionLabel {
                    text: "LAST WALK"
                }
                ColumnLayout {
                    spacing: 4
                    RowLayout {
                        Text {
                            text: "Length:"; color: Theme.muted; font.pixelSize: 11; Layout.fillWidth: true
                        }
                        Text {
                            text: treeCtrl.lastPathLength > 0 ? treeCtrl.lastPathLength : "—"; color: Theme.accent; font.pixelSize: 13; font.bold: true
                        }
                    }
                    RowLayout {
                        Text {
                            text: "Reached leaf:"; color: Theme.muted; font.pixelSize: 11; Layout.fillWidth: true
                        }
                        Text {
                            text: treeCtrl.lastPathLength > 0 ? (treeCtrl.lastReachedLeaf ? "✓" : "✗") : "—"
                            color: treeCtrl.lastReachedLeaf ? Theme.success : Theme.danger; font.pixelSize: 13; font.bold: true
                        }
                    }
                    RowLayout {
                        Text {
                            text: "Hit target:"; color: Theme.muted; font.pixelSize: 11; Layout.fillWidth: true
                        }
                        Text {
                            text: treeCtrl.lastPathLength > 0 ? (treeCtrl.lastHitTarget ? "✓" : "✗") : "—"
                            color: treeCtrl.lastHitTarget ? Theme.success : Theme.danger; font.pixelSize: 13; font.bold: true
                        }
                    }
                }

                StatDivider {
                }
                SectionLabel {
                    text: "PATH LENGTH DISTRIBUTION"
                }
                HistogramView {
                    id: histView
                    Layout.fillWidth: true; Layout.fillHeight: true
                }
            }
        }
    }
}
