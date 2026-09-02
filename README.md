# P4 — Polynomial Planar Phase Portraits

P4 computes and draws the phase portrait of polynomial planar differential systems on the Poincaré sphere and Poincaré-Lyapunov compactified plane.

> Fork of [oscarsaleta/P4](https://github.com/oscarsaleta/P4) · LGPL v3

---

## How it works

P4 is a Qt 6 desktop application. When you evaluate a vector field, it invokes **Maple** as a subprocess to perform symbolic algebra. The Maple scripts are embedded directly inside the `p4` binary — no separate installation directory is needed. On first launch, P4 will ask for the path to your Maple executable and save it; that is the only configuration required.

---

## Installing a release

Prebuilt packages are attached to every [release](https://github.com/JeeerryZ/p4/releases).
They are self-contained: Qt and the other runtime libraries are bundled, so
nothing needs to be installed beforehand. Maple is the one exception — it is
licensed separately and P4 asks for the path to it on first launch.

| Platform | File | How to install |
|---|---|---|
| Windows | `P4-windows-x86_64-setup.exe` | Run it. Adds a Start menu entry and an uninstaller. |
| Windows (no installer) | `P4-windows-x86_64.zip` | Unpack anywhere and run `p4.exe`. Keep all files together. |
| macOS (Apple Silicon) | `P4-macOS-arm64.dmg` | Open and drag P4 to Applications. |
| macOS (Intel) | `P4-macOS-x86_64.dmg` | Open and drag P4 to Applications. |

The `sumtables` folder shipped next to `p4.exe` must stay there and must stay
writable: `lyapunov` caches its summation tables in it, and if it cannot,
studying a weak focus fails with *"Error (readlyapunovresult) invalid subscript
selector"*. The installer grants write access to that folder automatically; if
you unpack the zip under `C:\Program Files` by hand, you have to do it yourself
— or unpack it somewhere you can write to.

The sections below are for building from source.

---

## Dependencies

| Dependency | macOS | Windows | Linux |
|---|---|---|---|
| Qt 6 (Widgets, PrintSupport, Svg) | `brew install qt` | Qt installer (MinGW kit) | `apt install qt6-base-dev` |
| GMP | `brew install gmp` | bundled with MinGW Qt | `apt install libgmp-dev` |
| MPFR | `brew install mpfr` | bundled with MinGW Qt | `apt install libmpfr-dev` |
| Maple | separate license | separate license | separate license |

GMP/MPFR paths are auto-detected by `P4.pri` — no manual `.pro` editing needed.

---

## Building & installing

### macOS (recommended — ARM and Intel)

```bash
./install_mac.sh
```

The script checks for and installs Homebrew, Qt, GMP, and MPFR if missing, then builds and copies all binaries to `dist/`. Pass `--clean` to wipe a previous build first.

Build logs are written to `build/qmake.log`, `build/make.log`, and `brew_install.log`.

### Windows (MinGW)

Set your Qt and MinGW tools on `PATH`, then run the installer script:

```powershell
$env:PATH = "E:\Qt\Tools\mingw1310_64\bin;E:\Qt\6.9.3\mingw_64\bin;$env:PATH"
.\install_win.ps1
```

Adjust the paths to match your Qt installation. The script runs qmake, compiles, copies the four binaries to `dist\`, and runs `windeployqt` to bundle all required Qt and MinGW DLLs.

Pass `-Clean` to wipe a previous build:

```powershell
.\install_win.ps1 -Clean
```

### Linux

```bash
sudo apt install qt6-base-dev qt6-svg libgmp-dev libmpfr-dev   # Debian/Ubuntu
qmake -r P4.pro
make -j$(nproc)
make install
```

---

## Output layout

After building, `dist/` contains everything needed to run P4:

```
dist/
├── p4(.exe)              main application  ← Maple scripts + help embedded inside
├── lyapunov(.exe)        Lyapunov constants (double precision)
├── lyapunov_mpf(.exe)    Lyapunov constants (arbitrary precision)
├── separatrice(.exe)     separatrix integrator
└── Qt*.dll / *.dylib     Qt runtime (Windows: added by windeployqt)
```

All four binaries must be in the same directory — `p4` locates the helper executables next to itself at runtime.

---

## First launch

On first run, P4 opens a settings dialog and asks for the **Maple executable path** (e.g. `/Library/Frameworks/Maple.framework/Versions/2024/bin/maple` on macOS, or `C:\Program Files\Maple 2024\bin\cmaple.exe` on Windows). This path is saved and never asked for again unless you reset settings.

Maple scripts are extracted automatically to a writable per-user directory at startup and updated whenever you install a new version of P4.

---

## Repository layout

```
P4.pro / P4.pri         root qmake project + shared variables / GMP detection
src-gui/
  p4/                   main Qt GUI application
  lyapunov/             Lyapunov constants (double precision)
  lyapunov_mpf/         Lyapunov constants (arbitrary precision, GMP/MPFR)
  separatrice/          separatrix numerical integrator
src-mpl/                Maple script sources (.mpl) and compiled scripts (.m)
help/                   HTML documentation and images (embedded in binary)
QtCreator/              standalone .pro files for opening sub-projects in Qt Creator
install_mac.sh          macOS build + install script
install_win.ps1         Windows build + install script
```

---

## Troubleshooting

**Maple not found on first launch**
Set the full path to your Maple command-line executable in Settings → Maple Executable. On macOS this is typically the `maple` binary inside the Maple framework; on Windows it is `cmaple.exe`.

**GMP/MPFR not found (macOS)**
```bash
brew install gmp mpfr
```

**GMP/MPFR not found (Windows)**
Install the MinGW-bundled packages via MSYS2:
```bash
pacman -S mingw-w64-x86_64-gmp mingw-w64-x86_64-mpfr
```

**Wrong Qt version picked up by qmake**
Prefix `PATH` with the correct Qt bin directory before running qmake, or call it by full path (e.g. `/opt/homebrew/opt/qt/bin/qmake`).

---

## License

GNU Lesser General Public License v3 — see [LICENSE](LICENSE).
