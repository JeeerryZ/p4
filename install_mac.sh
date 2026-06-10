#!/usr/bin/env bash
set -euo pipefail

# ---------------------------------------------------------------------------
# P4 macOS Installer
# Builds P4 from source and assembles a self-contained dist/ directory.
#
# Maple scripts and help files are embedded in the p4 binary via Qt resources
# and extracted automatically at runtime — no separate files needed.
#
# Usage:
#   ./install_mac.sh            # build + install to dist/
#   ./install_mac.sh --clean    # remove build/ and dist/
# ---------------------------------------------------------------------------

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INSTALL_DIR="$SCRIPT_DIR/dist"
BUILD_DIR="$SCRIPT_DIR/build"

CLEAN=0
for arg in "$@"; do
    [[ "$arg" == "--clean" ]] && CLEAN=1
done

if [[ "$CLEAN" -eq 1 ]]; then
    printf '\033[1;33mCleaning build/ and dist/ ...\033[0m\n'
    rm -rf "$BUILD_DIR" "$INSTALL_DIR"
    printf '\033[1;32mDone.\033[0m\n'
    exit 0
fi

BOLD=$'\033[1m';  NC=$'\033[0m'
YELLOW=$'\033[0;33m'
BRED=$'\033[1;31m'; BGREEN=$'\033[1;32m'; BCYAN=$'\033[1;36m'

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
    kill "$_SPIN_PID" 2>/dev/null; wait "$_SPIN_PID" 2>/dev/null || true
    _SPIN_PID=""
    printf "\r\033[K"
}

trap 'spinner_stop' EXIT

ok()    { spinner_stop; printf "  ${BGREEN}checkmark${NC}  %s\n" "$*"; }
fail()  { spinner_stop; printf "  ${BRED}x${NC}  %s\n" "$*"; }
die()   { fail "$*"; exit 1; }
phase() { echo; printf "${BCYAN}${BOLD}[ Phase %s/%s ]  %s${NC}\n" "$1" "$2" "$3"; }

ARCH="$(uname -m)"
BREW_PREFIX="$( [[ "$ARCH" == "arm64" ]] && echo "/opt/homebrew" || echo "/usr/local" )"
has_pkg() { pkg-config --exists "$1" 2>/dev/null; }

NEED_BREW=0
MISSING_PKGS=()

printf "\n${BCYAN}${BOLD}======================================${NC}\n"
printf   "${BCYAN}${BOLD}   P4  macOS  Installer               ${NC}\n"
printf   "${BCYAN}${BOLD}======================================${NC}\n\n"

phase 1 3 "Checking environment"

spinner_start "Checking Homebrew"
if command -v brew &>/dev/null; then
    ok "Homebrew  $(brew --version | head -1)"
else
    fail "Homebrew not found -- will install"
    NEED_BREW=1
fi

spinner_start "Checking pkg-config"
if command -v pkg-config &>/dev/null; then
    ok "pkg-config"
else
    fail "pkg-config missing -- will install"; MISSING_PKGS+=(pkg-config)
fi

spinner_start "Checking gmp"
if command -v pkg-config &>/dev/null && has_pkg gmp; then
    ok "gmp  ($(pkg-config --modversion gmp 2>/dev/null || echo found))"
else
    fail "gmp -- not installed"; MISSING_PKGS+=(gmp)
fi

spinner_start "Checking mpfr"
if command -v pkg-config &>/dev/null && has_pkg mpfr; then
    ok "mpfr  ($(pkg-config --modversion mpfr 2>/dev/null || echo found))"
else
    fail "mpfr -- not installed"; MISSING_PKGS+=(mpfr)
fi

spinner_start "Checking Qt"
QMAKE="$BREW_PREFIX/opt/qt/bin/qmake"
[[ ! -x "$QMAKE" ]] && QMAKE="$(command -v qmake 2>/dev/null || true)"
if [[ -x "$QMAKE" ]]; then
    ok "Qt / qmake  ($("$QMAKE" --version 2>&1 | head -1))"
else
    fail "Qt / qmake not found -- will install"; MISSING_PKGS+=(qt)
fi

phase 2 3 "Installing dependencies"

if [[ "$NEED_BREW" -eq 1 ]]; then
    spinner_start "Installing Homebrew"
    /bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)" \
        </dev/null >>"$SCRIPT_DIR/brew_install.log" 2>&1 \
        || die "Homebrew installation failed -- see brew_install.log"
    eval "$("$BREW_PREFIX/bin/brew" shellenv)"
    ok "Homebrew installed"
fi

if [[ ${#MISSING_PKGS[@]} -eq 0 ]]; then
    ok "All packages already installed"
else
    for pkg in "${MISSING_PKGS[@]}"; do
        spinner_start "Installing $pkg"
        brew install "$pkg" >>"$SCRIPT_DIR/brew_install.log" 2>&1 \
            || die "Failed to install $pkg -- see brew_install.log"
        ok "$pkg installed"
    done
fi

[[ ! -x "$QMAKE" ]] && QMAKE="$BREW_PREFIX/opt/qt/bin/qmake"
[[ -x "$QMAKE" ]] || die "qmake still not found after install"

phase 3 3 "Building and installing"

JOBS="$(sysctl -n hw.logicalcpu 2>/dev/null || echo 4)"
mkdir -p "$BUILD_DIR"

spinner_start "Configuring with qmake"
( cd "$BUILD_DIR" && "$QMAKE" -r "$SCRIPT_DIR/P4.pro" ) \
    >"$BUILD_DIR/qmake.log" 2>&1 \
    || die "qmake failed -- see build/qmake.log"
ok "qmake configured"

spinner_start "Compiling ($JOBS jobs)"
make -C "$BUILD_DIR" -j"$JOBS" \
    >"$BUILD_DIR/make.log" 2>&1 \
    || die "Build failed -- see build/make.log"
ok "Build complete"

spinner_start "Assembling dist/"
mkdir -p "$INSTALL_DIR"
for bin in p4 lyapunov lyapunov_mpf separatrice; do
    src="$BUILD_DIR/$bin/$bin"
    [[ -f "$src" ]] || die "$bin not found in build/"
    cp "$src" "$INSTALL_DIR/"
done
ok "Binaries copied to dist/"

MACDEPLOYQT="$BREW_PREFIX/opt/qt/bin/macdeployqt"
if [[ -x "$MACDEPLOYQT" ]]; then
    spinner_start "Running macdeployqt"
    "$MACDEPLOYQT" "$INSTALL_DIR/p4" >"$BUILD_DIR/macdeployqt.log" 2>&1 || true
    ok "Qt frameworks bundled"
fi

echo
printf "${BGREEN}${BOLD}======================================${NC}\n"
printf "${BGREEN}${BOLD}  P4 installed -> %s/${NC}\n" "$INSTALL_DIR"
printf "${BGREEN}${BOLD}  p4  lyapunov  lyapunov_mpf  separatrice${NC}\n"
printf "${BGREEN}${BOLD}======================================${NC}\n\n"

# ---------------------------------------------------------------------------
# Optional: build a .app bundle + DMG, same as the GitHub Actions workflow
# ---------------------------------------------------------------------------
read -rp "Create a .app bundle and DMG (like the GitHub Actions build)? [y/N] " MAKE_APP
if [[ "$MAKE_APP" =~ ^[Yy]$ ]]; then
    echo
    printf "${BCYAN}${BOLD}[ Optional ]  Creating .app bundle and DMG${NC}\n"

    VERSION="$(sed -n 's/#define VERSION "\(.*\)"/\1/p' "$SCRIPT_DIR/src-gui/version.h")"
    APP_DIR="$SCRIPT_DIR/P4.app"

    spinner_start "Assembling P4.app"
    rm -rf "$APP_DIR"
    mkdir -p "$APP_DIR/Contents/MacOS"
    cp "$BUILD_DIR/p4/p4" "$APP_DIR/Contents/MacOS/"

    cat > "$APP_DIR/Contents/Info.plist" << PLIST
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN"
  "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
  <key>CFBundleExecutable</key>     <string>p4</string>
  <key>CFBundleIdentifier</key>     <string>cat.gsd.p4</string>
  <key>CFBundleName</key>           <string>P4</string>
  <key>CFBundleDisplayName</key>    <string>P4</string>
  <key>CFBundleVersion</key>        <string>$VERSION</string>
  <key>CFBundleShortVersionString</key> <string>$VERSION</string>
  <key>CFBundlePackageType</key>    <string>APPL</string>
  <key>LSMinimumSystemVersion</key> <string>12.0</string>
  <key>NSHighResolutionCapable</key><true/>
</dict>
</plist>
PLIST
    ok "P4.app assembled"

    if [[ -x "$MACDEPLOYQT" ]]; then
        spinner_start "Running macdeployqt on P4.app"
        "$MACDEPLOYQT" "$APP_DIR" >"$BUILD_DIR/macdeployqt_app.log" 2>&1 || true
        ok "Qt frameworks bundled into P4.app"
    fi

    spinner_start "Copying helper binaries into P4.app"
    cp "$BUILD_DIR/lyapunov/lyapunov" \
       "$BUILD_DIR/lyapunov_mpf/lyapunov_mpf" \
       "$BUILD_DIR/separatrice/separatrice" \
       "$APP_DIR/Contents/MacOS/"
    ok "Helper binaries copied"

    if ! command -v create-dmg &>/dev/null; then
        spinner_start "Installing create-dmg"
        brew install create-dmg >>"$SCRIPT_DIR/brew_install.log" 2>&1 \
            || die "Failed to install create-dmg -- see brew_install.log"
        ok "create-dmg installed"
    fi

    DMG_NAME="P4-macOS-$ARCH.dmg"
    spinner_start "Creating $DMG_NAME"
    rm -f "$SCRIPT_DIR/$DMG_NAME"
    create-dmg \
        --volname "P4" \
        --volicon "$SCRIPT_DIR/help/p4smallicon.png" \
        --window-size 540 360 \
        --icon-size 128 \
        --icon "P4.app" 180 160 \
        --app-drop-link 360 160 \
        --hide-extension "P4.app" \
        "$SCRIPT_DIR/$DMG_NAME" \
        "$APP_DIR" \
        >"$BUILD_DIR/create-dmg.log" 2>&1 \
        || die "create-dmg failed -- see build/create-dmg.log"
    ok "DMG created -> $DMG_NAME"

    echo
    printf "${BGREEN}${BOLD}======================================${NC}\n"
    printf "${BGREEN}${BOLD}  P4.app -> %s/${NC}\n" "$APP_DIR"
    printf "${BGREEN}${BOLD}  DMG    -> %s/%s${NC}\n" "$SCRIPT_DIR" "$DMG_NAME"
    printf "${BGREEN}${BOLD}======================================${NC}\n\n"
fi
