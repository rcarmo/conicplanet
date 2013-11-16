/******************************************************************************
    Conic Planet - CGI program to render an image of a planet
    Copyright (C) 2000 Mukund Iyer <muks@crosswinds.net>
    
    Most portions of this code were developed by and are
    Copyright (C) 2000 Hari Nair <hari@alumni.caltech.edu>
    for XPlanet 0.72

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
******************************************************************************/

/* Based on Chapter 18 of Astronomical Formulae for Calculators by Meeus */

#include <math.h>

#include "util.h"

void 
sunpos (double &sunlon, double &sunlat, double &sundist, double T)
{
  double L = (poly (2.7969668E2, 3.600076892E4, 3.025E-4, 0, T) 
	      * deg_to_rad);
  double M = (poly (3.5847583E2, 3.599904975E4, -1.5E-4, -3.3E-6, T) 
	      * deg_to_rad);
  double ecc = poly (1.675104E-2, -4.18E-5, -1.26E-7, 0, T);
  double eccanom = kepler (ecc, M);
  double nu = 2 * atan (sqrt ((1 + ecc) / (1 - ecc)) * tan (eccanom / 2));
  double theta = L + nu - M;

  sunlon = theta;
  sunlat = 0;
  sundist = 1.0000002 * (1 - ecc * cos (eccanom));
}
