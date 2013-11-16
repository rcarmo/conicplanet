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

#include <iostream.h>
#include <math.h>

#include "get_color.h"
#include "options.h"
#include "util.h"
#include "ProjectionAzimuthal.h"

ProjectionAzimuthal::ProjectionAzimuthal (int w, int h) 
  : Projection (w, h)  // call the Projection constructor
{
  set_background ();

  radius = sqrt (0.02 * opts.radius);

  // The rendered globe is contained in a square with sides of length
  // iside and upper left corner at (istart, jstart).
  iside = (int) (radius * height);
}

int
ProjectionAzimuthal::pixel_to_spher (int x, int y, double &lon, double &lat)
{
  double X = 2.0 * (x - opts.centx) / iside;
  double Y = -2.0 * (y - opts.centy) / iside;

  double rho = sqrt (X*X + Y*Y);
  if (rho > radius) return (0);

  double c = M_PI * rho / radius;

  if (rho == 0) 
    {
      lat = 0;
      lon = 0;
    }
  else 
    {       
      double arg = Y * sin (c) / rho;
      lat = asin (arg);
      lon = atan2 (X * sin (c), rho * cos (c));
    }

  if (opts.obslon || opts.obslat || opts.rotate) rotate_point (lon, lat);

  if (lon > M_PI) lon -= TWO_PI;
  else if (lon < -M_PI) lon += TWO_PI;

  return (1);
}

int
ProjectionAzimuthal::spher_to_pixel (double lon, double lat, int &x, int &y)
{
  if (opts.obslon || opts.obslat || opts.rotate) rotate_point (lon, lat);
  
  double c = acos (cos (lat) * cos (lon));
  double k = radius * c / (M_PI * sin (c));

  double X = k * cos (lat) * sin (lon);
  double Y = k * sin (lat);

  x = opts.centx + int (X * iside/2);
  if (x < 0 || x >= width) return (0);

  y = opts.centy - int (Y * iside/2);
  if (y < 0 || y >= height) return (0);

  return (1);
}

