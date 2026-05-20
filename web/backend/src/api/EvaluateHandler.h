#pragma once

#include "../third_party/crow/crow_all.h"

struct EvaluateHandler {
    static crow::response handle(const crow::request &req);
};
