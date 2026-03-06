import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "."

ApplicationWindow {
    id: root
    visible: true
    width: 1100
    height: 720
    title: "Drunk Walk Simulation"
    color: Theme.bg

    property int activeTab: 0
    property bool autoRunning: false
    property int tps: 5
    property int batchN: 1000
    property var curveData: []

    // ── Auto-restart: reset and continue when finished ───────────────────────
    Timer {
        id: stepTimer
        interval: Math.round(1000 / root.tps)
        repeat: true
        running: root.autoRunning && root.activeTab === 1
        onTriggered: {
            if (walk_controller.walk_finished) {
                walk_controller.reset_walk()
            } else {
                walk_controller.step_walk()
            }
        }
    }

    Connections {
        target: walk_controller
        function onParams_changed() { root.curveData = [] }
        function onCurve_ready(data) { root.curveData = data }
    }

    RowLayout {
        anchors.fill: parent
        spacing: 0

        // ── LEFT SIDEBAR ─────────────────────────────────────────────────────
        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: 280
            color: Theme.surface
            Rectangle { anchors.right: parent.right; width: 1; height: parent.height; color: Theme.border }

            ColumnLayout {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 14

                Item {
                    Layout.fillWidth: true; height: 56
                    Text {
                        text: "DRUNK WALK\nSIMULATOR"
                        font.pixelSize: 18; font.letterSpacing: 3; font.bold: true
                        color: Theme.txt; lineHeight: 1.3
                    }
                    Rectangle { anchors.bottom: parent.bottom; width: 32; height: 3; color: Theme.accent; radius: 2 }
                }

                SectionLabel { Layout.fillWidth: true; text: "PARAMETERS" }

                ParamSlider {
                    Layout.fillWidth: true
                    label: "p  (step forward)"
                    value: walk_controller.p_forward
                    from: 0.01; to: 0.99; stepSize: 0.01
                    sliderColor: Theme.accent
                    onMoved: function(v) { walk_controller.set_p_forward(v) }
                }

                Rectangle {
                    Layout.fillWidth: true; height: 28; radius: 4
                    color: Theme.card; border.color: Theme.border; border.width: 1
                    RowLayout {
                        anchors.fill: parent; anchors.leftMargin: 10; anchors.rightMargin: 10
                        Text { text: "q  (step back)"; color: Theme.muted; font.pixelSize: 11; Layout.fillWidth: true }
                        Text {
                            text: (1.0 - walk_controller.p_forward).toFixed(2)
                            color: Theme.danger; font.pixelSize: 11; font.bold: true
                        }
                    }
                }

                ParamSlider {
                    Layout.fillWidth: true
                    label: "B  (café position)"
                    value: walk_controller.cafe_pos
                    from: 1; to: 20; stepSize: 1
                    sliderColor: Theme.warn
                    onMoved: function(v) { walk_controller.set_cafe_pos(Math.round(v)) }
                }

                Rectangle {
                    Layout.fillWidth: true; height: 28; radius: 4
                    color: Qt.rgba(Theme.danger.r, Theme.danger.g, Theme.danger.b, 0.08)
                    border.color: Qt.rgba(Theme.danger.r, Theme.danger.g, Theme.danger.b, 0.4); border.width: 1
                    RowLayout {
                        anchors.fill: parent; anchors.leftMargin: 10; anchors.rightMargin: 10
                        Text { text: "💀  Cliff at position"; color: Theme.muted; font.pixelSize: 11; Layout.fillWidth: true }
                        Text { text: walk_controller.cliff_pos; color: Theme.danger; font.pixelSize: 11; font.bold: true }
                    }
                }

                SectionLabel { Layout.fillWidth: true; text: "MODE" }

                RowLayout {
                    Layout.fillWidth: true; spacing: 8
                    ModeTab { Layout.fillWidth: true; label: "BATCH";  highlight: root.activeTab === 0; accent: Theme.accent; onClicked: { root.activeTab = 0; root.autoRunning = false } }
                    ModeTab { Layout.fillWidth: true; label: "VISUAL"; highlight: root.activeTab === 1; accent: Theme.warn;   onClicked: root.activeTab = 1 }
                }

                // ── BATCH PANEL ───────────────────────────────────────────────
                ColumnLayout {
                    Layout.fillWidth: true; spacing: 10
                    visible: root.activeTab === 0

                    ParamSlider {
                        Layout.fillWidth: true
                        label: "Simulation runs (N)"
                        value: root.batchN; from: 100; to: 10000; stepSize: 100
                        sliderColor: Theme.accent
                        onMoved: function(v) { root.batchN = Math.round(v) }
                    }

                    AppButton {
                        Layout.fillWidth: true
                        label: walk_controller.batch_running ? "⏳  Running..." : "▶  Run " + root.batchN + " simulations"
                        accent: Theme.accent
                        enabled: !walk_controller.batch_running
                        opacity: enabled ? 1.0 : 0.5
                        onClicked: walk_controller.run_batch(root.batchN)
                    }

                    AppButton {
                        Layout.fillWidth: true
                        label: walk_controller.batch_running ? "⏳  Computing..." : "📈  Build Probability Curve"
                        accent: Theme.success
                        enabled: !walk_controller.batch_running
                        opacity: enabled ? 1.0 : 0.5
                        onClicked: walk_controller.build_curve(500, 20)
                    }
                }

                // ── VISUAL PANEL ──────────────────────────────────────────────
                ColumnLayout {
                    Layout.fillWidth: true; spacing: 10
                    visible: root.activeTab === 1

                    RowLayout {
                        Layout.fillWidth: true; spacing: 8
                        ModeTab { Layout.fillWidth: true; label: "MANUAL"; highlight: !root.autoRunning; accent: Theme.accent; onClicked: root.autoRunning = false }
                        ModeTab { Layout.fillWidth: true; label: "AUTO";   highlight: root.autoRunning;  accent: Theme.warn;   onClicked: root.autoRunning = true }
                    }

                    AppButton {
                        Layout.fillWidth: true; label: "Step  →"; accent: Theme.accent
                        enabled: !root.autoRunning && !walk_controller.walk_finished
                        opacity: enabled ? 1.0 : 0.4
                        onClicked: walk_controller.step_walk()
                    }

                    ParamSlider {
                        Layout.fillWidth: true
                        label: "TPS  " + root.tps
                        value: root.tps; from: 1; to: 30; stepSize: 1
                        sliderColor: Theme.warn
                        opacity: root.autoRunning ? 1.0 : 0.5
                        onMoved: function(v) { root.tps = Math.round(v) }
                    }

                    AppButton {
                        Layout.fillWidth: true
                        label: root.autoRunning ? "⏹  Stop" : "▶  Start"
                        accent: Theme.warn
                        onClicked: root.autoRunning = !root.autoRunning
                    }

                    AppButton {
                        Layout.fillWidth: true; label: "Reset"; accent: Theme.muted
                        onClicked: { root.autoRunning = false; walk_controller.reset_walk() }
                    }
                }

                Item { Layout.fillHeight: true }

                Text {
                    text: "1D Random Walk  •  Gambler's Ruin"
                    color: Theme.muted; font.pixelSize: 10; font.letterSpacing: 1
                }
            }
        }

        // ── RIGHT AREA ────────────────────────────────────────────────────────
        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 0

            // ── Stats bar ─────────────────────────────────────────────────────
            Rectangle {
                Layout.fillWidth: true; height: 80; color: Theme.card
                Rectangle { anchors.bottom: parent.bottom; width: parent.width; height: 1; color: Theme.border }

                RowLayout {
                    anchors.fill: parent
                    anchors.leftMargin: 24; anchors.rightMargin: 24; spacing: 20

                    // Cliff prob — show as 0.xxx
                    ColumnLayout {
                        Layout.fillWidth: true; spacing: 2
                        Text { text: "P(CLIFF)"; color: Theme.muted; font.pixelSize: 10; font.letterSpacing: 2; font.bold: true }
                        Text {
                            text: walk_controller.batch_n > 0 ? walk_controller.cliff_probability.toFixed(3) : "—"
                            color: Theme.danger; font.pixelSize: 26; font.bold: true
                        }
                        Text {
                            text: walk_controller.batch_n > 0 ? "empirical" : "run batch first"
                            color: Qt.rgba(Theme.danger.r, Theme.danger.g, Theme.danger.b, 0.6); font.pixelSize: 11
                        }
                    }
                    StatDivider { Layout.fillHeight: true; Layout.topMargin: 12; Layout.bottomMargin: 12 }

                    ColumnLayout {
                        Layout.fillWidth: true; spacing: 2
                        Text { text: "P(CAFÉ)"; color: Theme.muted; font.pixelSize: 10; font.letterSpacing: 2; font.bold: true }
                        Text {
                            text: walk_controller.batch_n > 0 ? walk_controller.cafe_probability.toFixed(3) : "—"
                            color: Theme.success; font.pixelSize: 26; font.bold: true
                        }
                        Text {
                            text: walk_controller.batch_n > 0 ? "empirical" : ""
                            color: Qt.rgba(Theme.success.r, Theme.success.g, Theme.success.b, 0.6); font.pixelSize: 11
                        }
                    }
                    StatDivider { Layout.fillHeight: true; Layout.topMargin: 12; Layout.bottomMargin: 12 }

                    ColumnLayout {
                        Layout.fillWidth: true; spacing: 2
                        Text { text: "AVG STEPS"; color: Theme.muted; font.pixelSize: 10; font.letterSpacing: 2; font.bold: true }
                        Text {
                            text: walk_controller.batch_n > 0 ? walk_controller.avg_steps.toFixed(1) : "—"
                            color: Theme.accent; font.pixelSize: 26; font.bold: true
                        }
                        Text {
                            text: walk_controller.batch_n > 0 ? "over " + walk_controller.batch_n.toLocaleString() + " runs" : ""
                            color: Qt.rgba(Theme.accent.r, Theme.accent.g, Theme.accent.b, 0.6); font.pixelSize: 11
                        }
                    }
                    StatDivider { Layout.fillHeight: true; Layout.topMargin: 12; Layout.bottomMargin: 12 }

                    // Theoretical
                    ColumnLayout {
                        Layout.fillWidth: true; spacing: 2
                        Text { text: "P(CLIFF) THEORY"; color: Theme.muted; font.pixelSize: 10; font.letterSpacing: 2; font.bold: true }
                        Text {
                            property real pp: walk_controller.p_forward
                            property int bb: walk_controller.cafe_pos
                            // Gambler's Ruin, TWO barriers: 0 (cliff) and B (cafe).
                            text: {
                                var q = 1 - pp
                                var k = bb - 1
                                if (k <= 0) return "—"
                                var probK
                                if (Math.abs(pp - 0.5) < 1e-9) {
                                    probK = 1.0 - k / bb
                                } else {
                                    var r = q / pp
                                    var rB = Math.pow(r, bb)
                                    var denom = 1.0 - rB
                                    if (Math.abs(denom) < 1e-12) return "—"
                                    probK = (Math.pow(r, k) - rB) / denom
                                }
                                return (q * probK).toFixed(3)
                            }
                            color: Theme.warn; font.pixelSize: 26; font.bold: true
                        }
                        Text { text: "Theory: q·P(ruin|B-1), barriers {0,B}"; color: Qt.rgba(Theme.warn.r, Theme.warn.g, Theme.warn.b, 0.6); font.pixelSize: 11 }
                    }
                    StatDivider { Layout.fillHeight: true; Layout.topMargin: 12; Layout.bottomMargin: 12 }

                    // Walk status (visual mode)
                    ColumnLayout {
                        Layout.fillWidth: true; spacing: 2
                        Text { text: "WALK STATUS"; color: Theme.muted; font.pixelSize: 10; font.letterSpacing: 2; font.bold: true }
                        RowLayout {
                            spacing: 10
                            Text { text: "pos " + walk_controller.current_position; color: Theme.txt; font.pixelSize: 18; font.bold: true }
                            Text { text: "/ " + walk_controller.steps_taken + " steps"; color: Theme.muted; font.pixelSize: 12 }
                        }
                        Text {
                            text: walk_controller.walk_finished
                                ? (walk_controller.fell_off_cliff ? "💀 Cliff" : "☕ Returned")
                                : (root.autoRunning ? "🥴 Walking..." : "—")
                            color: walk_controller.walk_finished ? (walk_controller.fell_off_cliff ? Theme.danger : Theme.success) : Theme.muted
                            font.pixelSize: 11
                        }
                    }
                }
            }

            // ── Content: Batch or Visual (no tab bar, controlled from sidebar) ─
            StackLayout {
                Layout.fillWidth: true
                Layout.fillHeight: true
                currentIndex: root.activeTab

                // ── TAB 0: Batch Statistics ────────────────────────────────────
                Item {
                    ColumnLayout {
                        anchors.fill: parent
                        anchors.margins: 20
                        spacing: 16

                        Rectangle {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            color: Theme.card
                            border.color: Theme.border; border.width: 1
                            radius: 8

                            // Computing spinner
                            Column {
                                anchors.centerIn: parent; spacing: 12
                                visible: walk_controller.batch_running
                                Text { anchors.horizontalCenter: parent.horizontalCenter; text: "⏳"; font.pixelSize: 48 }
                                Text { anchors.horizontalCenter: parent.horizontalCenter; text: "Computing..."; color: Theme.muted; font.pixelSize: 14 }
                            }

                            // Placeholder
                            Column {
                                anchors.centerIn: parent; spacing: 12
                                visible: root.curveData.length === 0 && !walk_controller.batch_running
                                Text { anchors.horizontalCenter: parent.horizontalCenter; text: "📈"; font.pixelSize: 48 }
                                Text {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    text: "Click \"Build Probability Curve\" to generate chart"
                                    color: Theme.muted; font.pixelSize: 14
                                }
                                Text {
                                    anchors.horizontalCenter: parent.horizontalCenter
                                    text: "P(cliff) and P(café) vs p  for B=" + walk_controller.cafe_pos
                                    color: "#3a2f60"; font.pixelSize: 12
                                }
                            }

                            ProbabilityChart {
                                anchors.fill: parent
                                anchors.margins: 16
                                visible: root.curveData.length > 0 && !walk_controller.batch_running
                                dataPoints: root.curveData
                                currentP: walk_controller.p_forward
                                cliffProb: walk_controller.cliff_probability
                                cafeProb: walk_controller.cafe_probability
                            }
                        }
                    }
                }

                // ── TAB 1: Visualization ───────────────────────────────────────
                Item {
                    ColumnLayout {
                        anchors.fill: parent
                        spacing: 0

                        StreetView {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                        }

                        Rectangle {
                            Layout.fillWidth: true; height: 44; color: Theme.surface
                            Rectangle { anchors.top: parent.top; width: parent.width; height: 1; color: Theme.border }
                            RowLayout {
                                anchors.fill: parent; anchors.margins: 16; spacing: 16
                                Text { text: "p=" + walk_controller.p_forward.toFixed(2) + "  q=" + (1 - walk_controller.p_forward).toFixed(2); color: Theme.muted; font.pixelSize: 12 }
                                StatDivider { height: 20; Layout.alignment: Qt.AlignVCenter }
                                Text { text: "B=" + walk_controller.cafe_pos + "  cliff=0"; color: Theme.muted; font.pixelSize: 12 }
                                StatDivider { height: 20; Layout.alignment: Qt.AlignVCenter }
                                Text { text: "pos: " + walk_controller.current_position; color: Theme.accent; font.pixelSize: 12; font.bold: true }
                                StatDivider { height: 20; Layout.alignment: Qt.AlignVCenter }
                                Text { text: "steps: " + walk_controller.steps_taken; color: Theme.txt; font.pixelSize: 12 }
                                Item { Layout.fillWidth: true }
                                Text {
                                    visible: walk_controller.walk_finished && !root.autoRunning
                                    text: walk_controller.fell_off_cliff ? "💀 FELL OFF CLIFF" : "☕ RETURNED TO CAFÉ"
                                    color: walk_controller.fell_off_cliff ? Theme.danger : Theme.success
                                    font.pixelSize: 12; font.bold: true; font.letterSpacing: 1
                                }
                                Text {
                                    visible: root.autoRunning
                                    text: "● AUTO  " + root.tps + " TPS"
                                    color: Theme.warn; font.pixelSize: 11; font.bold: true; font.letterSpacing: 1
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}
