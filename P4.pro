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

include(P4.pri)

TEMPLATE = subdirs
SUBDIRS = src-gui src-mpl

# ---------------------------------------------------------------------------
# Unix/macOS install target  →  dist/
#
# Usage:  make install
#
# Maple scripts and help files are embedded in p4 via QRC — no separate
# files needed.  On macOS, run macdeployqt on dist/p4 afterwards to bundle
# Qt frameworks for distribution.
#
# Windows: use install_win.ps1 instead (handles windeployqt automatically).
# ---------------------------------------------------------------------------

INSTALL_DIR = $$PROJECT_ROOT/dist

unix {
    bins.path     = $$INSTALL_DIR
    bins.commands = \
        mkdir -p $$INSTALL_DIR && \
        cp $$BUILD_DIR/p4/p4 \
           $$BUILD_DIR/lyapunov/lyapunov \
           $$BUILD_DIR/lyapunov_mpf/lyapunov_mpf \
           $$BUILD_DIR/separatrice/separatrice \
           $$INSTALL_DIR/

    INSTALLS += bins
}
