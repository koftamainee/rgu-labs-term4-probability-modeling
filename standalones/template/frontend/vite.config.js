import { defineConfig } from 'vite'
import vue from '@vitejs/plugin-vue'
import { fileURLToPath, URL } from 'node:url'

export default defineConfig({
  plugins: [vue()],

  resolve: {
    alias: {
      '@': fileURLToPath(new URL('./src', import.meta.url))
    }
  },

  server: {
    port: 5173,
    proxy: {
      // REST API requests → backend
      '/api': {
        target: 'http://backend:8080',
        changeOrigin: true
      },
      // WebSocket → backend
      '/ws': {
        target: 'ws://backend:8080',
        ws: true
      }
    }
  }
})
