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
if [[ "$ARCH" == "arm64" ]]; then
    BREW_PREFIX="/opt/homebrew"
else
    BREW_PREFIX="/usr/local"
fi

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
if command -v pkg-config &>/dev/null && has_pkg gmp; then
    ok "gmp  ($(pkg-config --modversion gmp 2>/dev/null || echo 'found'))"
else
    fail "gmp — not installed"
    MISSING_PKGS+=(gmp)
fi

# MPFR
spinner_start "Checking mpfr"
if command -v pkg-config &>/dev/null && has_pkg mpfr; then
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

# ═══════════════════════════════════════════════════════════════════════════════
phase 2 4 "Installing dependencies"

# Install Homebrew itself if missing
if [[ "$NEED_BREW" -eq 1 ]]; then
    spinner_start "Installing Homebrew"
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)" \
        </dev/null >>"$SCRIPT_DIR/brew_install.log" 2>&1 \
        || die "Homebrew installation failed — see brew_install.log"
    eval "$("$BREW_PREFIX/bin/brew" shellenv)"
    ok "Homebrew installed"
fi

# Install missing formulae
if [[ ${#MISSING_PKGS[@]} -eq 0 ]]; then
    ok "All packages already installed — nothing to do"
else
    for pkg in "${MISSING_PKGS[@]}"; do
        spinner_start "Installing $pkg"
        brew install "$pkg" >>"$SCRIPT_DIR/brew_install.log" 2>&1 \
            || die "Failed to install $pkg — see brew_install.log"
        ok "$pkg installed"
    done
fi

# Re-detect qmake after potential Qt install
if [[ ! -x "$QMAKE" ]]; then
    QMAKE="$BREW_PREFIX/opt/qt/bin/qmake"
    [[ ! -x "$QMAKE" ]] && QMAKE="$(command -v qmake 2>/dev/null || true)"
    [[ -x "$QMAKE" ]] || die "qmake still not found after install — is Qt installed?"
fi

# ═══════════════════════════════════════════════════════════════════════════════
phase 3 4 "Building P4"

JOBS="$(sysctl -n hw.logicalcpu 2>/dev/null || echo 4)"
mkdir -p "$BUILD_DIR"

spinner_start "Configuring with qmake"
( cd "$BUILD_DIR" && "$QMAKE" -r "$SCRIPT_DIR/P4.pro" ) \
    >"$BUILD_DIR/qmake.log" 2>&1 \
    || die "qmake failed — see build/qmake.log"
ok "qmake configured"

spinner_start "Compiling ($JOBS jobs)"
make -C "$BUILD_DIR" -j"$JOBS" \
    >"$BUILD_DIR/make.log" 2>&1 \
    || die "Build failed — see build/make.log"
ok "Build complete  ($JOBS parallel jobs)"
