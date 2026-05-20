/*  P4 REST backend — ScriptHandler
 *  Writes a Maple input script and executes Maple synchronously.
 *  All paths are configured via environment variables at runtime.
 */

#pragma once

#include <string>

class ScriptHandler
{
public:
    ScriptHandler();

    // ---- Vector field inputs (set by EvaluateHandler before calling) ----
    std::string str_xeq_;       // x-dot expression
    std::string str_yeq_;       // y-dot expression
    std::string str_gcf_;       // GCF expression ("1" if none)

    // ---- Parameters ----
    std::string str_numeric_;      // "true" or "false"
    std::string str_precision_;    // digits
    std::string str_precision0_;
    std::string str_epsilon_;
    std::string str_testsep_;
    std::string str_taylor_;
    std::string str_numericlevel_;
    std::string str_maxlevel_;
    std::string str_weaklevel_;
    std::string str_userp_;
    std::string str_userq_;
    std::string str_critpoints_;   // "true"
    std::string str_saveall_;      // "false"
    std::string time_limit_;       // seconds as string

    // ---- Paths (read from env in constructor) ----
    std::string str_maple_exe_;
    std::string str_bindir_;
    std::string str_tmpdir_;
    std::string str_mapledir_;     // directory containing p4.m

    // ---- I/O ----
    // Writes the Maple .txt script to <basename>.txt
    bool prepareMapleFile(const std::string &basename);

    // Executes Maple on <basename>.txt; returns true on success
    // timeout_sec: maximum wall time in seconds
    bool evaluateMapleScript(const std::string &basename, int timeout_sec);

private:
    // Escape backslashes for Maple string literals
    static std::string maplePathEscape(const std::string &path);
};
