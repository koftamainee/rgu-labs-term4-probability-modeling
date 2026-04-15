import QtQuick
import QtQuick.Controls
import QtQuick.Layouts
import "."

ApplicationWindow {
    id: root
    visible: true
    width: 1100
    height: 720
    title: "Random Walk Simulation"
    color: Theme.bg

    RowLayout {
        anchors.fill: parent
        spacing: 0

        Rectangle {
            Layout.fillHeight: true
            Layout.preferredWidth: 320
            color: Theme.surface
            Rectangle { anchors.right: parent.right; width: 1; height: parent.height; color: Theme.border }

            Column {
                anchors.fill: parent
                anchors.margins: 20
                spacing: 16

                Item {
                    width: parent.width; height: 56
                    Text {
                        text: "RANDOM WALK\nSIMULATOR"
                        font.pixelSize: 18; font.letterSpacing: 3; font.bold: true
                        color: Theme.txt; lineHeight: 1.3
                    }
                    Rectangle { anchors.bottom: parent.bottom; width: 32; height: 3; color: Theme.accent; radius: 2 }
                }

                SectionLabel { text: "PARAMETERS" }

                ParamSlider {
                    width: parent.width
                    label: "Start position"
                    value: walk_controller.start_position
                    from: -10; to: 10; stepSize: 0.5
                    sliderColor: Theme.accent
                    onMoved: function(v) { walk_controller.start_position = v }
                }

                ParamSlider {
                    width: parent.width
                    label: "Number of steps (n)"
                    value: walk_controller.total_steps
                    from: 1; to: 50; stepSize: 1
                    sliderColor: Theme.warn
                    onMoved: function(v) { walk_controller.total_steps = Math.round(v) }
                }

                SectionLabel { text: "STEP DISTRIBUTION" }

                Rectangle {
                    width: parent.width; height: 60; color: Theme.card; radius: 6; border.color: Theme.border
                    Column {
                        anchors.fill: parent; anchors.margins: 8
                        Text { text: "Current distribution:"; color: Theme.muted; font.pixelSize: 10 }
                        Text {
                            text: walk_controller.step_distribution_info
                            color: Theme.accent; font.pixelSize: 11; wrapMode: Text.WordWrap; width: parent.width
                        }
                    }
                }


                SectionLabel { text: "SIMULATION CONTROL" }

                RowLayout {
                    width: parent.width; spacing: 10
                    AppButton {
                        Layout.fillWidth: true
                        label: walk_controller.simulation_running ? "⏹ Stop" : "▶ Start"
                        accent: Theme.success
                        onClicked: walk_controller.simulation_running ? walk_controller.stopSimulation() : walk_controller.startSimulation()
                    }
                    AppButton {
                        Layout.fillWidth: true
                        label: "⟳ Reset"
                        accent: Theme.muted
                        onClicked: walk_controller.resetSimulation()
                    }
                }

                SectionLabel { text: "DISTRIBUTION" }

                AppButton {
                    width: parent.width
                    label: "📊 Compute final distribution"
                    accent: Theme.accent
                    onClicked: walk_controller.computeDistribution()
                }

                AppButton { width: parent.width; label: "📂 Load from file"; accent: Theme.accent; onClicked: walk_controller.loadDistributionFromFile() }



                Item { height: 20; width: 1 }
            }
        }

        ColumnLayout {
            Layout.fillWidth: true
            Layout.fillHeight: true
            spacing: 8

            Rectangle {
                Layout.fillWidth: true
                Layout.preferredHeight: parent.height * 0.5
                color: Theme.card
                border.color: Theme.border
                radius: 8
                WalkVisualization { anchors.fill: parent; anchors.margins: 16 }
            }

            Rectangle {
                Layout.fillWidth: true
                Layout.fillHeight: true
                color: Theme.card
                border.color: Theme.border
                radius: 8
                DistributionChart { anchors.fill: parent; anchors.margins: 16 }
            }

            Rectangle {
                Layout.fillWidth: true
                height: 40
                color: Theme.surface
                radius: 6
                RowLayout {
                    anchors.fill: parent
                    anchors.margins: 12
                    Text { text: "Position: " + walk_controller.current_position.toFixed(2); color: Theme.accent; font.bold: true }
                    Text { text: "Steps: " + walk_controller.steps_done + " / " + walk_controller.total_steps; color: Theme.muted }
                    Item { Layout.fillWidth: true }
                    Text {
                        text: walk_controller.simulation_finished ? "FINISHED" : (walk_controller.simulation_running ? "RUNNING" : "STOPPED")
                        color: walk_controller.simulation_finished ? Theme.success : (walk_controller.simulation_running ? Theme.warn : Theme.muted)
                        font.bold: true
                    }
                }
            }
        }
    }
}
