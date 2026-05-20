#include "EvaluateHandler.h"

#include "../ScriptHandler.h"
#include "../file_tab.h"
#include "../models/EvaluateRequest.h"
#include "../models/EvaluateResponse.h"

#include <chrono>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

// -----------------------------------------------------------------------
//  generateSessionId — nanosecond timestamp + random 4-hex suffix
// -----------------------------------------------------------------------
static std::string generateSessionId()
{
    auto ns = std::chrono::duration_cast<std::chrono::nanoseconds>(
                  std::chrono::steady_clock::now().time_since_epoch())
                  .count();

    std::mt19937_64 rng(static_cast<uint64_t>(ns));
    std::uniform_int_distribution<unsigned> dist(0, 0xffff);
    unsigned rnd = dist(rng);

    char buf[64];
    snprintf(buf, sizeof(buf), "%lld_%04x", (long long)ns, rnd);
    return std::string(buf);
}

// -----------------------------------------------------------------------
//  parseRequest — extract fields from Crow JSON body
// -----------------------------------------------------------------------
static EvaluateRequest parseRequest(const crow::request &req)
{
    EvaluateRequest er;
    auto body = crow::json::load(req.body);
    if (!body) return er;

    if (body.has("x_dot"))          er.x_dot         = body["x_dot"].s();
    if (body.has("y_dot"))          er.y_dot         = body["y_dot"].s();
    if (body.has("gcf"))            er.gcf           = body["gcf"].s();
    if (body.has("precision"))      er.precision      = static_cast<int>(body["precision"].i());
    if (body.has("epsilon"))        er.epsilon        = body["epsilon"].d();
    if (body.has("taylor_level"))   er.taylor_level   = static_cast<int>(body["taylor_level"].i());
    if (body.has("numeric_level"))  er.numeric_level  = static_cast<int>(body["numeric_level"].i());
    if (body.has("max_level"))      er.max_level      = static_cast<int>(body["max_level"].i());
    if (body.has("weakness_level")) er.weakness_level = static_cast<int>(body["weakness_level"].i());
    if (body.has("p"))              er.p              = static_cast<int>(body["p"].i());
    if (body.has("q"))              er.q              = static_cast<int>(body["q"].i());
    if (body.has("numeric"))        er.numeric        = body["numeric"].b();
    if (body.has("test_sep"))       er.test_sep       = body["test_sep"].b();
    if (body.has("time_limit"))     er.time_limit     = static_cast<int>(body["time_limit"].i());

    return er;
}

// -----------------------------------------------------------------------
//  addCors — add CORS headers to every response
// -----------------------------------------------------------------------
static void addCors(crow::response &resp)
{
    resp.add_header("Access-Control-Allow-Origin",  "*");
    resp.add_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
    resp.add_header("Access-Control-Allow-Headers", "Content-Type");
}

// -----------------------------------------------------------------------
//  EvaluateHandler::handle
// -----------------------------------------------------------------------
crow::response EvaluateHandler::handle(const crow::request &req)
{
    EvaluateRequest er = parseRequest(req);

    // Validate
    if (er.x_dot.empty() || er.y_dot.empty()) {
        crow::response resp(400);
        resp.body = crow::json::dump(buildError("x_dot and y_dot are required"));
        resp.set_header("Content-Type", "application/json");
        addCors(resp);
        return resp;
    }

    // Configure ScriptHandler
    ScriptHandler sh;
    sh.str_xeq_          = er.x_dot;
    sh.str_yeq_          = er.y_dot;
    sh.str_gcf_          = er.gcf.empty() ? "1" : er.gcf;
    sh.str_numeric_      = er.numeric ? "true" : "false";
    sh.str_precision_    = std::to_string(er.precision);
    sh.str_precision0_   = "0";
    sh.str_epsilon_      = std::to_string(er.epsilon);
    sh.str_testsep_      = er.test_sep ? "true" : "false";
    sh.str_taylor_       = std::to_string(er.taylor_level);
    sh.str_numericlevel_ = std::to_string(er.numeric_level);
    sh.str_maxlevel_     = std::to_string(er.max_level);
    sh.str_weaklevel_    = std::to_string(er.weakness_level);
    sh.str_userp_        = std::to_string(er.p);
    sh.str_userq_        = std::to_string(er.q);
    sh.str_critpoints_   = "true";
    sh.str_saveall_      = "false";
    sh.time_limit_       = std::to_string(er.time_limit);

    // Build unique temp basename
    std::string tmpDir = sh.str_tmpdir_;
    // Ensure tmpDir ends with separator
    if (!tmpDir.empty() && tmpDir.back() != '/' && tmpDir.back() != '\\')
        tmpDir += '/';

    // Create tmp dir if needed
    try {
        fs::create_directories(tmpDir);
    } catch (const std::exception &ex) {
        std::cerr << "[WARN] Cannot create tmpDir: " << ex.what() << "\n";
    }

    std::string sessionId = generateSessionId();
    std::string basename  = tmpDir + "p4web_" + sessionId;

    // Write Maple script
    if (!sh.prepareMapleFile(basename)) {
        crow::response resp(500);
        resp.body = crow::json::dump(buildError("Failed to write Maple script"));
        resp.set_header("Content-Type", "application/json");
        addCors(resp);
        return resp;
    }

    // Run Maple
    bool mapleOk = sh.evaluateMapleScript(basename, er.time_limit);

    // Read raw output (either .res or error)
    std::string rawOutput;
    {
        std::string finres = basename + "_fin.res";
        std::ifstream rf(finres);
        if (rf.is_open()) {
            std::ostringstream ss;
            ss << rf.rdbuf();
            rawOutput = ss.str();
        }
    }

    if (!mapleOk) {
        crow::response resp(422);
        crow::json::wvalue errObj = buildError("Maple evaluation failed or timed out");
        errObj["maple_output"] = rawOutput;
        resp.body = crow::json::dump(errObj);
        resp.set_header("Content-Type", "application/json");
        addCors(resp);
        return resp;
    }

    // Parse results
    WVFStudy study;
    if (!study.readTables(basename)) {
        crow::response resp(422);
        crow::json::wvalue errObj = buildError("Failed to parse Maple result tables: "
                                               + study.lasterror_);
        errObj["maple_output"] = rawOutput;
        resp.body = crow::json::dump(errObj);
        resp.set_header("Content-Type", "application/json");
        addCors(resp);
        return resp;
    }

    // Cleanup temp files (best-effort)
    auto removeIfExists = [](const std::string &path) {
        try { fs::remove(path); } catch (...) {}
    };
    removeIfExists(basename + ".txt");
    removeIfExists(basename + "_vec.tab");
    removeIfExists(basename + "_fin.tab");
    removeIfExists(basename + "_inf.tab");
    removeIfExists(basename + "_fin.res");
    removeIfExists(basename + "_inf.res");

    // Build response
    crow::json::wvalue result = buildResponse(study, rawOutput);
    crow::response resp(200);
    resp.body = crow::json::dump(result);
    resp.set_header("Content-Type", "application/json");
    addCors(resp);
    return resp;
}
