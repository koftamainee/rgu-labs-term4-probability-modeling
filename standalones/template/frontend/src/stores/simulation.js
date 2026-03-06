import { defineStore } from 'pinia'
import { ref } from 'vue'
import { simulationApi } from '@/api/simulation'

// Template store — copy and rename for each task
export const useSimulationStore = defineStore('simulation', () => {

  const result   = ref(null)
  const loading  = ref(false)
  const error    = ref(null)

  async function runSimulation(params) {
    loading.value = true
    error.value   = null
    try {
      result.value = await simulationApi.run(params)
    } catch (e) {
      error.value = e.message
    } finally {
      loading.value = false
    }
  }

  function reset() {
    result.value  = null
    error.value   = null
    loading.value = false
  }

  return { result, loading, error, runSimulation, reset }
})
