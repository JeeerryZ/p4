/*  P4 REST backend — ScriptHandler implementation
 *
 *  Writes a Maple input script replicating QInputVF::prepareFile() logic,
 *  then forks Maple as a child process and waits for completion.
 *
 *  Qt and Wt dependencies have been removed.  Paths are read from env vars.
 */

#include "ScriptHandler.h"

#include <cerrno>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

#ifdef _WIN32
#  include <windows.h>
#else
#  include <sys/types.h>
#  include <sys/wait.h>
#  include <unistd.h>
#  include <signal.h>
#endif

// The Maple simplify proc — inlined from custom.h
static const char *MAPLE_SIMPLIFY_EXPR =
    "proc(expr) try radsimp(simplify(expr),ratdenom) catch: "
    "simplify(expr) end try end";

// -----------------------------------------------------------------------
//  Constructor — read paths from environment with sensible defaults
// -----------------------------------------------------------------------
ScriptHandler::ScriptHandler()
{
    auto env = [](const char *name, const char *fallback) -> std::string {
        const char *v = std::getenv(name);
        return v ? std::string(v) : std::string(fallback);
    };

    str_maple_exe_ = env("MAPLE_PATH",   "/usr/local/maple/bin/maple");
    str_bindir_    = env("P4_BIN_DIR",   "/usr/local/p4/bin/");
    str_tmpdir_    = env("P4_TMP_DIR",   "/tmp/p4/");
    str_mapledir_  = env("P4_MAPLE_DIR", "/usr/local/p4/bin/");

    // Defaults
    str_xeq_         = "0";
    str_yeq_         = "0";
    str_gcf_         = "1";
    str_numeric_     = "false";
    str_precision_   = "8";
    str_precision0_  = "0";
    str_epsilon_     = "0.01";
    str_testsep_     = "false";
    str_taylor_      = "6";
    str_numericlevel_= "10";
    str_maxlevel_    = "20";
    str_weaklevel_   = "4";
    str_userp_       = "1";
    str_userq_       = "1";
    str_critpoints_  = "true";
    str_saveall_     = "false";
    time_limit_      = "60";
}

// -----------------------------------------------------------------------
//  maplePathEscape — double backslashes for Maple string literals
// -----------------------------------------------------------------------
std::string ScriptHandler::maplePathEscape(const std::string &path)
{
    std::string out;
    out.reserve(path.size() * 2);
    for (char c : path) {
        if (c == '\\') out += "\\\\";
        else           out += c;
    }
    return out;
}

// -----------------------------------------------------------------------
//  prepareMapleFile — writes <basename>.txt
// -----------------------------------------------------------------------
bool ScriptHandler::prepareMapleFile(const std::string &basename)
{
    std::string txtfile = basename + ".txt";
    std::ofstream fp(txtfile);
    if (!fp.is_open()) {
        std::cerr << "[ERROR] Cannot open " << txtfile << " for writing\n";
        return false;
    }

    // Escape paths for Maple string literals
    std::string e_mapledir  = maplePathEscape(str_mapledir_);
    std::string e_bindir    = maplePathEscape(str_bindir_);
    std::string e_tmpdir    = maplePathEscape(str_tmpdir_);
    std::string e_vectab    = maplePathEscape(basename + "_vec.tab");
    std::string e_fintab    = maplePathEscape(basename + "_fin.tab");
    std::string e_inftab    = maplePathEscape(basename + "_inf.tab");
    std::string e_finres    = maplePathEscape(basename + "_fin.res");
    std::string e_infres    = maplePathEscape(basename + "_inf.res");

    std::string mainmaple   = e_mapledir + "p4.m";

#ifdef _WIN32
    std::string user_removecmd = "cmd /c del";
    std::string user_exeprefix = "cmd /c ";
    std::string user_lypexe    = "lyapunov.exe";
    std::string user_lypexe_mpf= "lyapunov_mpf.exe";
    std::string user_sepexe    = "separatrice.exe";
    std::string user_platform  = "windows";
#else
    std::string user_removecmd = "rm";
    std::string user_exeprefix = "";
    std::string user_lypexe    = "lyapunov";
    std::string user_lypexe_mpf= "lyapunov_mpf";
    std::string user_sepexe    = "separatrice";
    std::string user_platform  = "linux";
#endif

    bool is_numeric = (str_numeric_ == "true");
    std::string user_simplify = is_numeric ? "false" : "true";

    fp << "restart;\n";
    fp << "read( \"" << mainmaple << "\" );\n";
    fp << "user_bindir := \"" << e_bindir << "\":\n";
    fp << "user_tmpdir := \"" << e_tmpdir << "\":\n";
    fp << "user_lypexe := \"" << user_lypexe << "\":\n";
    fp << "user_lypexe_mpf := \"" << user_lypexe_mpf << "\":\n";
    fp << "user_sepexe := \"" << user_sepexe << "\":\n";
    fp << "user_exeprefix := \"" << user_exeprefix << "\":\n";
    fp << "user_platform := \"" << user_platform << "\":\n";
    fp << "user_sumtablepath := \"" << e_tmpdir << "\":\n";
    fp << "user_removecmd := \"" << user_removecmd << "\":\n";
    fp << "user_simplify := " << user_simplify << ":\n";
    fp << "user_simplifycmd := " << MAPLE_SIMPLIFY_EXPR << ":\n";

    fp << "save_all := " << str_saveall_ << ":\n";
    fp << "all_crit_points := 0:\n";  // TYPEOFSTUDY_ALL

    fp << "vec_table := \"" << e_vectab << "\":\n";
    fp << "finite_table := \"" << e_fintab << "\":\n";
    fp << "finite_res := \"" << e_finres << "\":\n";
    fp << "infinite_table := \"" << e_inftab << "\":\n";
    fp << "infinite_res := \"" << e_infres << "\":\n";

    // Vector field
    fp << "user_f := [ " << str_xeq_ << ", " << str_yeq_ << " ]:\n";
    fp << "user_gcf := " << str_gcf_ << ":\n";

    // Parameters
    fp << "user_numeric := " << str_numeric_ << ":\n";
    fp << "epsilon := " << str_epsilon_ << ":\n";
    fp << "test_sep := " << str_testsep_ << ":\n";
    fp << "user_precision := " << str_precision_ << ":\n";
    fp << "user_precision0 := " << str_precision0_ << ":\n";
    fp << "taylor_level := " << str_taylor_ << ":\n";
    fp << "numeric_level := " << str_numericlevel_ << ":\n";
    fp << "max_level := " << str_maxlevel_ << ":\n";
    fp << "weakness_level := " << str_weaklevel_ << ":\n";
    fp << "user_p := " << str_userp_ << ":\n";
    fp << "user_q := " << str_userq_ << ":\n";

    // Main entry
    fp << "try p4main() catch:\n"
       << "printf( \"! Error (%a) %a\\n\", lastexception[1], lastexception[2] );\n"
       << "finally: closeallfiles();\n"
       << "if normalexit=0 then `quit`(0); else `quit(1)` end if: end try:\n";

    fp.close();
    return true;
}

// -----------------------------------------------------------------------
//  evaluateMapleScript — forks Maple and waits
// -----------------------------------------------------------------------
bool ScriptHandler::evaluateMapleScript(const std::string &basename,
                                        int timeout_sec)
{
    std::string txtfile = basename + ".txt";

#ifdef _WIN32
    // Windows: CreateProcess
    std::string cmd = "\"" + str_maple_exe_ + "\" -q \"" + txtfile + "\"";
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si)); si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessA(nullptr, const_cast<char*>(cmd.c_str()),
                        nullptr, nullptr, FALSE, 0, nullptr, nullptr, &si, &pi)) {
        std::cerr << "[ERROR] CreateProcess failed: " << GetLastError() << "\n";
        return false;
    }
    DWORD waitMs = (timeout_sec > 0) ? (DWORD)(timeout_sec * 1000) : INFINITE;
    DWORD result = WaitForSingleObject(pi.hProcess, waitMs);
    DWORD exitCode = 1;
    GetExitCodeProcess(pi.hProcess, &exitCode);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    if (result == WAIT_TIMEOUT) {
        std::cerr << "[ERROR] Maple timed out after " << timeout_sec << "s\n";
        return false;
    }
    return exitCode == 0;

#else
    // POSIX: fork/exec
    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "[ERROR] fork failed: " << strerror(errno) << "\n";
        return false;
    }

    if (pid == 0) {
        // Child
        const char *args[] = {
            str_maple_exe_.c_str(),
            "-q",
            txtfile.c_str(),
            nullptr
        };
        execv(str_maple_exe_.c_str(), const_cast<char* const*>(args));
        // If execv returns, it failed
        std::cerr << "[ERROR] execv failed: " << strerror(errno) << "\n";
        _exit(127);
    }

    // Parent: wait with optional timeout via alarm
    if (timeout_sec > 0) alarm((unsigned)timeout_sec);

    int status = 0;
    pid_t waited = waitpid(pid, &status, 0);
    if (timeout_sec > 0) alarm(0); // cancel alarm

    if (waited < 0) {
        if (errno == EINTR) {
            // Timed out — kill child
            kill(pid, SIGKILL);
            waitpid(pid, nullptr, 0);
            std::cerr << "[ERROR] Maple timed out after " << timeout_sec << "s\n";
        } else {
            std::cerr << "[ERROR] waitpid error: " << strerror(errno) << "\n";
        }
        return false;
    }

    if (WIFEXITED(status)) return WEXITSTATUS(status) == 0;
    // Signaled or other
    return false;
#endif
}
