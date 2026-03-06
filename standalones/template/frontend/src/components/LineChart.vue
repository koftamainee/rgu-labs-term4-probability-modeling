<template>
  <div class="chart-wrapper">
    <canvas ref="canvasRef"></canvas>
  </div>
</template>

<script setup>
import { ref, watch, onMounted, onUnmounted } from 'vue'
import { Chart, registerables } from 'chart.js'

Chart.register(...registerables)

const props = defineProps({
  // { labels: [...], datasets: [{ label, data, borderColor, ... }] }
  chartData: { type: Object, required: true },
  options:   { type: Object, default: () => ({}) }
})

const canvasRef = ref(null)
let chart = null

const defaultOptions = {
  responsive: true,
  maintainAspectRatio: false,
  animation: { duration: 300 },
  plugins: { legend: { position: 'top' } }
}

function buildChart() {
  if (chart) chart.destroy()
  chart = new Chart(canvasRef.value, {
    type: 'line',
    data: props.chartData,
    options: { ...defaultOptions, ...props.options }
  })
}

onMounted(buildChart)

watch(() => props.chartData, () => {
  if (chart) {
    chart.data = props.chartData
    chart.update()
  }
}, { deep: true })

onUnmounted(() => chart?.destroy())
</script>

<style scoped>
.chart-wrapper { position: relative; height: 400px; width: 100%; }
</style>
