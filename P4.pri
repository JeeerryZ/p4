#  This file is part of P4
# 
#  Copyright (C) 1996-2017  J.C. Artés, P. De Maesschalck, F. Dumortier,
#                           C. Herssens, J. Llibre, O. Saleta, J. Torregrosa
# 
#  P4 is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Lesser General Public License as published
#  by the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
# 
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Lesser General Public License for more details.
# 
#  You should have received a copy of the GNU Lesser General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.

PROJECT_ROOT=$$PWD
BUILD_DIR=$$PROJECT_ROOT/build

# ---- GMP/MPFR library detection ----------------------------------------
# Sets GMP_LIBS, GMP_MPFR_LIBS, GMP_INCLUDEPATH for use in sub-projects.
# On Windows, also probes MSYS2/MinGW install locations automatically.

unix:!macx {
    # Linux: libraries are in standard search paths via package manager
    GMP_LIBS        = -lgmp
    GMP_MPFR_LIBS   = -lgmp -lmpfr
    GMP_INCLUDEPATH =
}

macx {
    # Probe Homebrew, preferring the prefix matching Qt's own target
    # architecture first — a Rosetta (x86_64) build must not pick up the
    # native arm64 Homebrew at /opt/homebrew (or vice versa), or the linker
    # fails with "found architecture 'arm64', required architecture
    # 'x86_64'" (or the reverse).
    contains(QT_ARCH, x86_64) {
        HB_PREFIX_1 = /usr/local
        HB_PREFIX_2 = /opt/homebrew
    } else {
        HB_PREFIX_1 = /opt/homebrew
        HB_PREFIX_2 = /usr/local
    }

    exists($$HB_PREFIX_1/include/gmp.h) {
        GMP_PREFIX = $$HB_PREFIX_1
    } else:exists($$HB_PREFIX_2/include/gmp.h) {
        GMP_PREFIX = $$HB_PREFIX_2
    } else {
        GMP_PREFIX = /usr
    }
    GMP_LIBS        = -L$$GMP_PREFIX/lib -lgmp
    GMP_MPFR_LIBS   = -L$$GMP_PREFIX/lib -lgmp -lmpfr
    GMP_INCLUDEPATH = $$GMP_PREFIX/include

    exists($$HB_PREFIX_1/opt/qt/lib) {
        QMAKE_LFLAGS   += -L$$HB_PREFIX_1/opt/qt/lib
        QMAKE_CXXFLAGS += -I$$HB_PREFIX_1/opt/qt/include
    } else:exists($$HB_PREFIX_2/opt/qt/lib) {
        QMAKE_LFLAGS   += -L$$HB_PREFIX_2/opt/qt/lib
        QMAKE_CXXFLAGS += -I$$HB_PREFIX_2/opt/qt/include
    }
}

win32 {
    # GMP_PREFIX may already be set via a qmake command-line variable
    # (e.g. `qmake GMP_PREFIX=...`) — CI passes this explicitly because
    # GitHub Actions' MSYS2 install path is dynamic (under a per-run temp
    # directory), so none of the hardcoded local-dev paths below would match.
    isEmpty(GMP_PREFIX) {
        # Probe common MSYS2 and Qt-bundled MinGW locations
        exists(C:/msys64/mingw64/include/gmp.h) {
            GMP_PREFIX = C:/msys64/mingw64
        } else:exists(D:/msys64/mingw64/include/gmp.h) {
            GMP_PREFIX = D:/msys64/mingw64
        } else:exists(C:/Qt/Tools/mingw64/include/gmp.h) {
            GMP_PREFIX = C:/Qt/Tools/mingw64
        } else:exists(D:/Qt/Tools/mingw64/include/gmp.h) {
            GMP_PREFIX = D:/Qt/Tools/mingw64
        } else:exists(E:/Qt/Tools/mingw64/include/gmp.h) {
            GMP_PREFIX = E:/Qt/Tools/mingw64
        } else {
            warning("GMP not found. Install via MSYS2: pacman -S mingw-w64-x86_64-gmp mingw-w64-x86_64-mpfr")
            GMP_PREFIX =
        }
    }
    !isEmpty(GMP_PREFIX) {
        GMP_LIBS        = -L$$GMP_PREFIX/lib -lgmp
        GMP_MPFR_LIBS   = -L$$GMP_PREFIX/lib -lgmp -lmpfr
        GMP_INCLUDEPATH = $$GMP_PREFIX/include
    }
}
# -------------------------------------------------------------------------
