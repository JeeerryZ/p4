/*  This file is part of P4
 *
 *  Copyright (C) 1996-2017  J.C. Artés, P. De Maesschalck, F. Dumortier
 *                           C. Herssens, J. Llibre, O. Saleta, J. Torregrosa
 *
 *  P4 is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published
 *  by the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "p4settings.h"

#include "file_paths.h"
#include "file_vf.h"
#include "main.h"

#include <QApplication>
#include <QDir>
#include <QFile>
#include <QSettings>
#include <QStandardPaths>

static QString s_Settings_mathmanipulator;
static QString s_Settings_sumtablepath;
static QString s_Settings_temppath;
static QString s_Settings_mapleexe;
static bool s_Settings_changed;

// -----------------------------------------------------------------------
//                      EXTRACTMAPLESCRIPTS
// -----------------------------------------------------------------------
//
// Copies the embedded Maple .m files (from the Qt resource ::/maple/) to
// AppLocalDataLocation/maple/ so that the cmaple process can read them
// from real filesystem paths.  Re-extracts whenever the app version changes.

void extractMapleScripts()
{
    const QString targetDir =
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
        + "/maple";

    QDir().mkpath(targetDir);

    // Use a hidden stamp file to skip extraction when already up to date.
    const QString stampPath = targetDir + "/.p4version";
    {
        QFile stamp(stampPath);
        if (stamp.open(QIODevice::ReadOnly)) {
            if (QString::fromLatin1(stamp.readAll()).trimmed() == g_p4Version)
                return;
        }
    }

    // Extract every file listed under the :/maple/ resource prefix.
    const QDir resourceDir(":/maple");
    for (const QString &name : resourceDir.entryList(QDir::Files)) {
        const QString dest = targetDir + "/" + name;
        QFile::remove(dest);
        QFile::copy(":/maple/" + name, dest);
        // Qt copies resources as read-only; make them writable.
        QFile::setPermissions(
            dest,
            QFile::ReadOwner | QFile::WriteOwner |
            QFile::ReadGroup  | QFile::ReadOther);
    }

    // Write new stamp.
    QFile stamp(stampPath);
    if (stamp.open(QIODevice::WriteOnly))
        stamp.write(g_p4Version.toLatin1());
}

// -----------------------------------------------------------------------
//                      PATH ACCESSORS
// -----------------------------------------------------------------------

QString getP4MaplePath(void)
{
    return QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation)
           + "/maple";
}

// The sub-executables (lyapunov, separatrice, lyapunov_mpf) must sit next
// to the p4 executable.  On macOS this is inside the .app bundle's MacOS/
// directory; on Windows it's the same folder as p4.exe.
QString getP4BinPath(void)
{
    return QApplication::applicationDirPath();
}

// -----------------------------------------------------------------------
//                      SETTINGS GETTERS / SETTERS
// -----------------------------------------------------------------------

QString getP4TempPath(void)     { return s_Settings_temppath; }
QString getP4SumTablePath(void) { return s_Settings_sumtablepath; }
QString getMapleExe(void)       { return s_Settings_mapleexe; }

void setP4TempPath(QString s)
{
    if (s_Settings_temppath != s) {
        s_Settings_temppath = s;
        s_Settings_changed = true;
    }
}

void setP4SumTablePath(QString s)
{
    if (s_Settings_sumtablepath != s) {
        s_Settings_sumtablepath = s;
        s_Settings_changed = true;
    }
}

void setMapleExe(QString s)
{
    if (s_Settings_mapleexe != s) {
        s_Settings_mapleexe = s;
        s_Settings_changed = true;
    }
}

QString getMathManipulator(void) { return s_Settings_mathmanipulator; }

int getMathPackage(void)
{
    return PACKAGE_MAPLE;
}

// -----------------------------------------------------------------------
//                      READ / SAVE SETTINGS
// -----------------------------------------------------------------------

bool readP4Settings(void)
{
    QSettings *p4settings = new QSettings();
    bool _ok = false;

    s_Settings_changed = false;

    if (p4settings->value("/Version").toString() == g_p4Version) {
        _ok = true;
        s_Settings_sumtablepath = p4settings->value("/SumtablePath").toString();
        s_Settings_temppath     = p4settings->value("/TempPath").toString();
        s_Settings_mapleexe     = p4settings->value("/MapleExe").toString();
        s_Settings_mathmanipulator = "Maple";

        if (s_Settings_mapleexe.isEmpty())
            _ok = false;
    }

    delete p4settings;

    if (!_ok) {
        s_Settings_sumtablepath    = getDefaultP4SumTablePath();
        s_Settings_temppath        = getDefaultP4TempPath();
        s_Settings_mapleexe        = getDefaultMapleInstallation();
        s_Settings_mathmanipulator = getDefaultMathManipulator();
        s_Settings_changed         = true;
        return false;
    }

    return true;
}

void saveP4Settings(void)
{
    if (!s_Settings_changed)
        return;

    QSettings *p4settings = new QSettings();
    p4settings->setValue("/Version",      g_p4Version);
    p4settings->setValue("/BuildDate",    g_p4VersionDate);
    p4settings->setValue("/SumtablePath", getP4SumTablePath());
    p4settings->setValue("/TempPath",     getP4TempPath());
    p4settings->setValue("/MapleExe",     getMapleExe());
#ifndef Q_OS_WIN
    p4settings->setValue("/Math", getMathManipulator());
#endif
    delete p4settings;
}
