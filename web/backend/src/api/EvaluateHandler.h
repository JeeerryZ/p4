#pragma once

#include "crow_all.h"

struct EvaluateHandler {
    static crow::response handle(const crow::request &req);
};
