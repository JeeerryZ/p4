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

#pragma once

#include "file_tab.h"

double eval_term1(P4POLYNOM1 p, double value);
double eval_term2(P4POLYNOM2 f, double *value);
double eval_term3(P4POLYNOM3 F, double *value);

void delete_term1(P4POLYNOM1 p);
void delete_term2(P4POLYNOM2 p);
void delete_term3(P4POLYNOM3 p);

char *printterm2(char *buf, struct term2 *f, bool isfirst, const char *x,
                 const char *y);
char *printterm3(char *buf, struct term3 *f, bool isfirst, const char *r,
                 const char *Co, const char *Si);
