#include "crow.h"
#include "crow/middlewares/cors.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>

// ── Include your simulation headers here ──────────────────────


int main(int argc, char* argv[]) {
    std::string config_path = (argc > 1) ? argv[1] : "config.json";
    json config = load_config(config_path);

    int port = config.value("port", 8080);

    crow::App<crow::CORSHandler> app;

    auto& cors = app.get_middleware<crow::CORSHandler>();
    cors
        .global()
        .headers("Content-Type", "Authorization")
        .methods("GET"_method, "POST"_method, "OPTIONS"_method)
        .origin("*");

    CROW_ROUTE(app, "/api/health")
    ([]() {
        return crow::response(200, R"({"status":"ok"})");
    });

    // ── Example: POST /api/simulate ───────────────────────────
    // Replace body with your actual simulation logic
    CROW_ROUTE(app, "/api/simulate")
        .methods("POST"_method)
    ([&config](const crow::request& req) {
        auto body = json::parse(req.body, nullptr, /*throw=*/false);
        if (body.is_discarded()) {
            return crow::response(400, R"({"error":"invalid JSON"})");
        }

        json response = {
            {"result", "replace_with_real_simulation"},
            {"params", body}
        };

        return crow::response(200, response.dump());
    });

    CROW_WEBSOCKET_ROUTE(app, "/ws/simulate")
        .onopen([](crow::websocket::connection& conn) {
            CROW_LOG_INFO << "WS opened: " << &conn;
        })
        .onmessage([](crow::websocket::connection& conn,
                      const std::string& data, bool is_binary) {
            if (is_binary) return;

            auto msg = json::parse(data, nullptr, false);
            if (msg.is_discarded()) {
                conn.send_text(R"({"error":"invalid JSON"})");
                return;
            }

            conn.send_text(R"({"status":"done"})");
        })
        .onclose([](crow::websocket::connection& conn, const std::string& reason) {
            CROW_LOG_INFO << "WS closed: " << reason;
        });

    std::cout << "[backend] starting on port " << port << "\n";
    app.port(port).multithreaded().run();

    return 0;
}
