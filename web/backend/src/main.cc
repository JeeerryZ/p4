#include "../third_party/crow/crow_all.h"
#include "api/EvaluateHandler.h"

#include <cstdlib>
#include <string>

int main()
{
    crow::SimpleApp app;

    // CORS preflight
    CROW_ROUTE(app, "/api/evaluate").methods("OPTIONS"_method)(
        [](const crow::request &) {
            crow::response resp(204);
            resp.add_header("Access-Control-Allow-Origin",  "*");
            resp.add_header("Access-Control-Allow-Methods", "POST, OPTIONS");
            resp.add_header("Access-Control-Allow-Headers", "Content-Type");
            return resp;
        });

    // Evaluate endpoint
    CROW_ROUTE(app, "/api/evaluate").methods("POST"_method)(EvaluateHandler::handle);

    // Health check
    CROW_ROUTE(app, "/api/health")([] {
        return crow::response(200, "ok");
    });

    const char *portEnv = std::getenv("PORT");
    int port = portEnv ? std::stoi(portEnv) : 8080;

    app.port(port).multithreaded().run();
    return 0;
}
