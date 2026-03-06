# Simulation App Template

C++ backend (Crow) + Vue 3 frontend, fully dockerized.

## Stack

| Layer    | Technology |
|----------|-----------|
| Backend  | C++17, [Crow](https://crowcpp.org/), nlohmann/json |
| Frontend | Vue 3, Vite, Pinia, Vue Router, Chart.js |
| Proxy    | nginx (production), Vite dev server (development) |
| Infra    | Docker, docker-compose |

## Project structure

```
.
├── backend/
│   ├── CMakeLists.txt          # Crow + nlohmann/json via FetchContent
│   ├── Dockerfile              # multi-stage: build → slim runtime
│   ├── config.json             # simulation parameters (mounted as volume)
│   └── src/
│       └── main.cpp            # routes: GET /api/health, POST /api/simulate, WS /ws/simulate
│
├── frontend/
│   ├── Dockerfile              # multi-stage: npm build → nginx
│   ├── nginx.conf              # proxies /api/ and /ws/ to backend
│   ├── vite.config.js          # dev proxy to backend:8080
│   └── src/
│       ├── main.js             # app entry — Vue + Pinia + Router
│       ├── App.vue             # root layout with navbar
│       ├── router/index.js     # Vue Router routes
│       ├── stores/simulation.js # Pinia store template
│       ├── api/simulation.js   # axios REST + WebSocket wrappers
│       ├── views/HomeView.vue  # home page with health check
│       └── components/
│           └── LineChart.vue   # reusable Chart.js wrapper
│
└── docker-compose.yml          # wires frontend + backend on sim_net
```

## Quick start

### Production (Docker)
```bash
docker-compose up --build
# → open http://localhost
```

### Development (hot reload)
```bash
# Terminal 1 — backend
cd backend
cmake -B build && cmake --build build
./build/backend config.json

# Terminal 2 — frontend
cd frontend
npm install
npm run dev
# → open http://localhost:5173
```

## Adding a new simulation (per task)

1. **Backend**: add `backend/src/MySimulation.cpp` + `include/MySimulation.h`
2. **Backend**: add a new `CROW_ROUTE` in `main.cpp`
3. **Frontend**: create `frontend/src/views/TaskNView.vue`
4. **Frontend**: register route in `router/index.js`
5. **Config**: add parameters to `backend/config.json`

## Communication patterns

### REST (request/response)
```js
// frontend
const result = await simulationApi.run({ p: 0.5, n: 1000 })
```
```cpp
// backend — main.cpp
CROW_ROUTE(app, "/api/simulate").methods("POST"_method)
([](const crow::request& req) { ... });
```

### WebSocket (streaming steps)
```js
// frontend
const ws = createSimulationSocket(
  (msg) => steps.value.push(msg),
  ()    => console.log('done')
)
ws.send({ p: 0.5, n: 1000 })
```
```cpp
// backend — main.cpp
CROW_WEBSOCKET_ROUTE(app, "/ws/simulate")
  .onmessage([](auto& conn, const std::string& data, bool) {
      // send steps one by one
      conn.send_text(json{{"step", 42}}.dump());
  });
```
