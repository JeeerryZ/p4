#pragma once
#include <string>

struct EvaluateRequest {
    std::string x_dot;
    std::string y_dot;
    std::string gcf;          // default: "1"
    int    precision      = 8;
    double epsilon        = 0.01;
    int    taylor_level   = 6;
    int    numeric_level  = 10;
    int    max_level      = 20;
    int    weakness_level = 4;
    int    p              = 1;
    int    q              = 1;
    bool   numeric        = false;    // symbolic by default
    bool   test_sep       = false;
    int    time_limit     = 60;       // seconds for Maple
};
