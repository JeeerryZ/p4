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

#ifndef P4SETTINGS_H
#define P4SETTINGS_H

#include <QString>

// Maple scripts are extracted from embedded resources to AppLocalDataLocation
// on first launch (or whenever the app version changes). Maple needs real
// filesystem paths — it cannot read from Qt resources directly.
void extractMapleScripts();

void setP4TempPath(QString s);
QString getP4TempPath(void);

void setP4SumTablePath(QString s);
QString getP4SumTablePath(void);

void setMapleExe(QString s);
QString getMapleExe(void);

// Returns the directory containing the extracted .m scripts.
QString getP4MaplePath(void);

// Returns the directory containing the p4 executable (and lyapunov/separatrice).
QString getP4BinPath(void);

QString getMathManipulator(void);
int getMathPackage(void);

bool readP4Settings(void);
void saveP4Settings(void);

#endif // P4SETTINGS_H
