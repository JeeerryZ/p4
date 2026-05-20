#!/usr/bin/env bash
set -euo pipefail

# ---------------------------------------------------------------------------
# P4 macOS Installer — animated
# ---------------------------------------------------------------------------

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INSTALL_DIR="$SCRIPT_DIR/p4"
BUILD_DIR="$SCRIPT_DIR/build"

# ── Colors ──────────────────────────────────────────────────────────────────
BOLD=$'\033[1m';  DIM=$'\033[2m';   NC=$'\033[0m'
RED=$'\033[0;31m';  GREEN=$'\033[0;32m'; YELLOW=$'\033[0;33m'; CYAN=$'\033[0;36m'
BRED=$'\033[1;31m'; BGREEN=$'\033[1;32m'; BCYAN=$'\033[1;36m'

# ── Spinner ──────────────────────────────────────────────────────────────────
_SPIN_PID=""
_SPIN_FRAMES=('⠋' '⠙' '⠹' '⠸' '⠼' '⠴' '⠦' '⠧' '⠇' '⠏')

spinner_start() {
    local label="$1"
    ( i=0
      while true; do
          printf "\r  ${YELLOW}%s${NC}  %s" "${_SPIN_FRAMES[$i]}" "$label"
          i=$(( (i+1) % ${#_SPIN_FRAMES[@]} ))
          sleep 0.08
      done ) &
    _SPIN_PID=$!
}

spinner_stop() {
    [[ -z "$_SPIN_PID" ]] && return
    kill "$_SPIN_PID" 2>/dev/null; wait "$_SPIN_PID" 2>/dev/null || true  # || true: SIGTERM gives non-zero exit under set -e
    _SPIN_PID=""
    printf "\r\033[K"
}

trap 'spinner_stop' EXIT

# ── Step result helpers ───────────────────────────────────────────────────────
ok()   { spinner_stop; printf "  ${BGREEN}✓${NC}  %s\n" "$*"; }
fail() { spinner_stop; printf "  ${BRED}✗${NC}  %s\n" "$*"; }
die()  { fail "$*"; exit 1; }

phase() {   # phase <n> <total> <name>
    echo
    printf "${BCYAN}${BOLD}[ Phase %s/%s ]  %s${NC}\n" "$1" "$2" "$3"
}

welcome() {
    printf "\n${BCYAN}${BOLD}╔══════════════════════════════════════╗${NC}\n"
    printf   "${BCYAN}${BOLD}║        P4  macOS  Installer          ║${NC}\n"
    printf   "${BCYAN}${BOLD}╚══════════════════════════════════════╝${NC}\n\n"
}

finish() {
    echo
    printf "${BGREEN}${BOLD}══════════════════════════════════════${NC}\n"
    printf "${BGREEN}${BOLD}  P4 installed → %s/bin/p4${NC}\n" "$INSTALL_DIR"
    printf "${BGREEN}${BOLD}══════════════════════════════════════${NC}\n\n"
}

welcome

# ── Architecture / Homebrew prefix ───────────────────────────────────────────
ARCH="$(uname -m)"
[[ "$ARCH" == "arm64" ]] && BREW_PREFIX="/opt/homebrew" || BREW_PREFIX="/usr/local"

# ── Detection helpers ─────────────────────────────────────────────────────────
has_brew_formula() { brew list --formula "$1" &>/dev/null; }
has_pkg()          { pkg-config --exists "$1" 2>/dev/null; }

# ── State filled by Phase 1 ───────────────────────────────────────────────────
NEED_BREW=0        # 1 = homebrew itself is missing
MISSING_PKGS=()    # brew formulae to install

# ═══════════════════════════════════════════════════════════════════════════════
phase 1 4 "Checking environment"

# Homebrew
spinner_start "Checking Homebrew"
if command -v brew &>/dev/null; then
    ok "Homebrew  $(brew --version | head -1)"
else
    fail "Homebrew not found — will install"
    NEED_BREW=1
fi

# pkg-config
spinner_start "Checking pkg-config"
if command -v pkg-config &>/dev/null; then
    ok "pkg-config found"
else
    fail "pkg-config missing — will install"
    MISSING_PKGS+=(pkg-config)
fi

# GMP
spinner_start "Checking gmp"
if has_pkg gmp; then
    ok "gmp  ($(pkg-config --modversion gmp 2>/dev/null || echo 'found'))"
else
    fail "gmp — not installed"
    MISSING_PKGS+=(gmp)
fi

# MPFR
spinner_start "Checking mpfr"
if has_pkg mpfr; then
    ok "mpfr  ($(pkg-config --modversion mpfr 2>/dev/null || echo 'found'))"
else
    fail "mpfr — not installed"
    MISSING_PKGS+=(mpfr)
fi

# Qt / qmake
spinner_start "Checking Qt"
QMAKE="$BREW_PREFIX/opt/qt/bin/qmake"
if [[ ! -x "$QMAKE" ]]; then
    QMAKE="$(command -v qmake 2>/dev/null || true)"
fi
if [[ -x "$QMAKE" ]]; then
    ok "Qt / qmake  ($("$QMAKE" --version 2>&1 | head -1))"
else
    fail "Qt / qmake not found — will install"
    MISSING_PKGS+=(qt)
fi
