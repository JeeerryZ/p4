# P4 — Polynomial Planar Phase Portraits

P4 draws the planar phase portrait of any polynomial differential system on the Poincaré or Poincaré-Lyapunov compactified plane.

> Fork of [oscarsaleta/P4](https://github.com/oscarsaleta/P4)

---

## Dependencies

| Dependency | Purpose |
|---|---|
| Qt 5 or Qt 6 | GUI framework |
| GMP | Arbitrary-precision integers |
| MPFR | Arbitrary-precision floating point |
| Maple | Symbolic algebra backend (**required at runtime**) |

Library paths for GMP and MPFR are detected automatically by `P4.pri` on all platforms. Manual editing of `.pro` files is not needed unless auto-detection fails.

---

## Building

### macOS (one command)

```bash
bash install_mac.sh
```

The script installs Homebrew (if absent), installs all missing dependencies, builds, and deploys to `p4/` in the repo root. Use `--clean` to wipe a previous build:

```bash
bash install_mac.sh --clean
```

### Linux

```bash
sudo apt install qtbase5-dev libgmp-dev libmpfr-dev   # Debian/Ubuntu
qmake -r P4.pro
make -j$(nproc)
sudo make install
```

### Windows (MSYS2 / MinGW)

```bash
pacman -S mingw-w64-x86_64-qt6-base mingw-w64-x86_64-gmp mingw-w64-x86_64-mpfr
qmake -r P4.pro
mingw32-make -j$(nproc)
mingw32-make install
```

---

## Install Layout

After `make install` (or `install_mac.sh`), P4 is deployed to `p4/` in the repo root:

```
p4/
├── bin/        p4, lyapunov, lyapunov_mpf, separatrice, p4.m, p4gcf.m
├── help/       HTML documentation and images
└── sumtables/  Cache directory (must be writable)
```

Launch with:

```bash
./p4/bin/p4
```

---

## Components

| Directory | Description |
|---|---|
| `src-gui/p4` | Main Qt GUI application |
| `src-gui/lyapunov` | Lyapunov constants calculator (double precision) |
| `src-gui/lyapunov_mpf` | Lyapunov constants calculator (arbitrary precision) |
| `src-gui/separatrice` | Separatrix numerical integrator |
| `src-mpl/` | Maple scripts (`p4.m`, `p4gcf.m`) — symbolic algebra backend |
| `QtCreator/` | Legacy standalone project files (kept for reference) |

---

## Troubleshooting

**GMP/MPFR not found on macOS**
```bash
brew install gmp mpfr
```

**GMP/MPFR not found on Windows**
```bash
pacman -S mingw-w64-x86_64-gmp mingw-w64-x86_64-mpfr
```

**qmake picks up the wrong Qt version**
Set `PATH` to the correct Qt bin directory before running qmake, or invoke it with its full path (e.g. `/opt/homebrew/opt/qt/bin/qmake`).

**Build log locations** (after running `install_mac.sh`)
- `build/qmake.log` — qmake configuration output
- `build/make.log` — compiler output
- `brew_install.log` — Homebrew install output

---

## License

GNU Lesser General Public License v3 — see [LICENSE](LICENSE).
