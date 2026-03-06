import axios from 'axios'

const http = axios.create({
  baseURL: '/api',
  headers: { 'Content-Type': 'application/json' }
})

// ── REST ───────────────────────────────────────────────────────
export const simulationApi = {
  async run(params) {
    const { data } = await http.post('/simulate', params)
    return data
  },

  async health() {
    const { data } = await http.get('/health')
    return data
  }
}

// ── WebSocket ──────────────────────────────────────────────────
// Usage:
//   const ws = createSimulationSocket(
//     (msg) => console.log('step:', msg),
//     () => console.log('done')
//   )
//   ws.send({ p: 0.5, n: 1000 })
//   ws.close()
export function createSimulationSocket(onMessage, onClose) {
  const protocol = location.protocol === 'https:' ? 'wss' : 'ws'
  const socket   = new WebSocket(`${protocol}://${location.host}/ws/simulate`)

  socket.onmessage = (event) => {
    const msg = JSON.parse(event.data)
    onMessage(msg)
  }

  socket.onclose = () => onClose?.()

  socket.onerror = (e) => console.error('[ws] error', e)

  return {
    send: (data) => socket.send(JSON.stringify(data)),
    close: () => socket.close()
  }
}
