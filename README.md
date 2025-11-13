# P4 Compilation Guide

## This is a fork of https://github.com/oscarsaleta/P4

## Prerequisites

- Qt Framework (Qt5 or Qt6)
- GMP and MPFR libraries (for arbitrary precision arithmetic)
- Appropriate compiler/toolchain:
  - Windows: MinGW (suggested), MSYS2, or Visual Studio Qt kit
  - Linux: GCC or Clang
  - macOS: Xcode command line tools, Homebrew, or MacPorts

---

## 1. Configuring Library Paths

Before compilation, you must edit two project files:

### Files to Edit

- p4.pro
- lyapunov.pro

### What to Change

In each file, find the lines that set the library/include directories for GMP and MPFR.
Set them according to your operating system and where the libraries are installed.

#### Example (Windows/MinGW):

INCLUDEPATH += C:/msys64/mingw64/include
LIBS += -LC:/msys64/mingw64/lib -lgmp -lmpfr

#### Example (macOS/Homebrew):

INCLUDEPATH += /opt/homebrew/include
LIBS += -L/opt/homebrew/lib -lgmp -lmpfr

Or, sometimes libraries might be under /usr/local/include and /usr/local/lib.

#### Example (Linux):

INCLUDEPATH += /usr/include
LIBS += -L/usr/lib -lgmp -lmpfr

Note:

- Make sure the paths match the actual location of your libraries!
- You can check using `brew info gmp` and `brew info mpfr` (macOS) or `whereis libgmp.so`/`libmpfr.so` (Linux).

---

## 2. Building the Project

### Windows

1. Open a Qt Command Prompt (or MSYS2 shell) with the appropriate compiler in your PATH.
2. Run:
   qmake -r p4.pro
   mingw32-make
   mingw32-make install
   If your Qt kit is installed somewhere else, set the PATH accordingly.

### macOS

1. Open Terminal.
2. Make sure you have Homebrew installed (for GMP/MPFR):
   brew install gmp mpfr
3. Run:
   qmake -r p4.pro
   make
   make install
   If libraries are not found, ensure your INCLUDEPATH and LIBS in .pro files match /opt/homebrew/include and /opt/homebrew/lib (Apple Silicon) or /usr/local/include/lib (Intel Macs).

### Linux

1. Install Qt, GMP, MPFR via package manager (e.g., `sudo apt install qtbase5-dev libgmp-dev libmpfr-dev`)
2. Run:
   qmake -r p4.pro
   make
   sudo make install
   Make sure LIBS and INCLUDEPATH in your .pro files are correct.

---

## 3. Troubleshooting

- If you get linker/include errors, double-check that path settings in both .pro files correctly point to the include and library directories for GMP/MPFR.
- You may need to install development headers (`-dev` packages on Linux).
- On Mac, sometimes libraries are only visible after running `brew link gmp mpfr --force`.

---

## Summary

- Edit p4.pro and lyapunov.pro to match your system's GMP/MPFR locations.
- Run `qmake -r p4.pro` and then `make`/`mingw32-make` depending on platform.
- Use `make install` (or `mingw32-make install`) if needed.
- On Mac, Homebrew installs libraries to /opt/homebrew/include and /opt/homebrew/lib by default.

---

If you need help finding your libraries, check the output of:

- `brew info gmp mpfr` (macOS)
- `pkg-config --cflags --libs gmp mpfr` (Linux)
- Or the installation paths in your Qt environment (Windows)
