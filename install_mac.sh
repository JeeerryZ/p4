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
    ( local i=0
      while true; do
          printf "\r  ${YELLOW}%s${NC}  %s" "${_SPIN_FRAMES[$i]}" "$label"
          i=$(( (i+1) % ${#_SPIN_FRAMES[@]} ))
          sleep 0.08
      done ) &
    _SPIN_PID=$!
}

spinner_stop() {
    [[ -z "$_SPIN_PID" ]] && return
    kill "$_SPIN_PID" 2>/dev/null; wait "$_SPIN_PID" 2>/dev/null || true
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
