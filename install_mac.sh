#!/usr/bin/env bash
set -euo pipefail

# ---------------------------------------------------------------------------
# P4 macOS installer
# Installs dependencies via Homebrew, builds with qmake, and copies the app
# to <repo-root>/p4/ (matching the layout defined in P4.pro).
# ---------------------------------------------------------------------------

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
INSTALL_DIR="$SCRIPT_DIR/p4"
BUILD_DIR="$SCRIPT_DIR/build"

# ---- Helpers ---------------------------------------------------------------

info()    { echo "[info]  $*"; }
success() { echo "[ok]    $*"; }
warn()    { echo "[warn]  $*"; }
die()     { echo "[error] $*" >&2; exit 1; }

require_confirm() {
    read -r -p "$1 [y/N] " ans
    [[ "$ans" =~ ^[Yy]$ ]] || die "Aborted."
}

# ---- Detect architecture / Homebrew prefix ---------------------------------

ARCH="$(uname -m)"
if [[ "$ARCH" == "arm64" ]]; then
    BREW_PREFIX="/opt/homebrew"
else
    BREW_PREFIX="/usr/local"
fi

# ---- Check Homebrew --------------------------------------------------------

if ! command -v brew &>/dev/null; then
    die "Homebrew not found. Install it from https://brew.sh and re-run this script."
fi
info "Homebrew found at $(brew --prefix)"

# ---- Install required formulae ---------------------------------------------

MISSING=()
for formula in qt gmp mpfr; do
    if ! brew list --formula "$formula" &>/dev/null; then
        MISSING+=("$formula")
    fi
done

if [[ ${#MISSING[@]} -gt 0 ]]; then
    warn "The following Homebrew packages are not installed: ${MISSING[*]}"
    require_confirm "Install them now?"
    brew install "${MISSING[@]}"
fi
success "All Homebrew dependencies satisfied (qt, gmp, mpfr)"

# ---- Locate qmake ----------------------------------------------------------

# Prefer Homebrew Qt's qmake over any system one
QMAKE="$BREW_PREFIX/opt/qt/bin/qmake"
if [[ ! -x "$QMAKE" ]]; then
    # Fallback: search PATH
    QMAKE="$(command -v qmake 2>/dev/null || true)"
    [[ -x "$QMAKE" ]] || die "qmake not found. Make sure Homebrew Qt is installed: brew install qt"
fi
info "Using qmake: $QMAKE  ($(\"$QMAKE\" --version | head -1))"

# ---- Detect CPU count for parallel make ------------------------------------

JOBS="$(sysctl -n hw.logicalcpu 2>/dev/null || echo 4)"

# ---- Build -----------------------------------------------------------------

info "Configuring build..."
mkdir -p "$BUILD_DIR"
cd "$SCRIPT_DIR"

"$QMAKE" P4.pro -o "$BUILD_DIR/Makefile"
cd "$BUILD_DIR"

info "Building with $JOBS parallel jobs..."
make -j"$JOBS"
success "Build complete"

# ---- Install ---------------------------------------------------------------

info "Installing to $INSTALL_DIR ..."
mkdir -p "$INSTALL_DIR/bin"
mkdir -p "$INSTALL_DIR/help"
mkdir -p "$INSTALL_DIR/sumtables"
chmod 777 "$INSTALL_DIR/sumtables"

# Binaries
for bin in p4/p4 lyapunov/lyapunov lyapunov_mpf/lyapunov_mpf separatrice/separatrice; do
    src="$BUILD_DIR/$bin"
    if [[ -f "$src" ]]; then
        cp "$src" "$INSTALL_DIR/bin/"
        success "Copied $(basename "$src")"
    else
        warn "$src not found — skipping"
    fi
done

# Maple scripts
cp "$SCRIPT_DIR/src-mpl/p4.m"    "$INSTALL_DIR/bin/"
cp "$SCRIPT_DIR/src-mpl/p4gcf.m" "$INSTALL_DIR/bin/"

# Help files
cp -r "$SCRIPT_DIR/help/"* "$INSTALL_DIR/help/"

success "Installation complete: $INSTALL_DIR"
echo
echo "To launch P4:"
echo "  $INSTALL_DIR/bin/p4"
