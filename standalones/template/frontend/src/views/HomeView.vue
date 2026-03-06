<template>
  <div class="home">
    <h1>Simulation Template</h1>
    <p class="subtitle">
      Backend status:
      <span class="badge" :class="statusClass">{{ status }}</span>
    </p>

    <div class="card">
      <h2>How to use this template</h2>
      <ol>
        <li>Add your C++ simulation logic in <code>backend/src/</code></li>
        <li>Expose it via a new route in <code>main.cpp</code></li>
        <li>Create a new view in <code>frontend/src/views/</code></li>
        <li>Add the route to <code>router/index.js</code></li>
      </ol>
    </div>
  </div>
</template>

<script setup>
import { ref, onMounted } from 'vue'
import { simulationApi } from '@/api/simulation'

const status      = ref('checking...')
const statusClass = ref('')

onMounted(async () => {
  try {
    await simulationApi.health()
    status.value      = 'connected ✓'
    statusClass.value = 'badge-success'
  } catch {
    status.value      = 'unreachable ✗'
    statusClass.value = 'badge-danger'
  }
})
</script>

<style scoped>
.home        { display: flex; flex-direction: column; gap: 1.5rem; }
.subtitle    { color: var(--muted); display: flex; align-items: center; gap: .5rem; }
.card h2     { margin-bottom: 1rem; }
.card ol     { padding-left: 1.2rem; }
.card li     { margin-bottom: .4rem; }
</style>