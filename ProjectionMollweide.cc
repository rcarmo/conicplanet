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
#include "ProjectionMollweide.h"

ProjectionMollweide::ProjectionMollweide (int w, int h) 
  : Projection (w, h) // call the Projection constructor
{
  set_background ();

  radius = 0.01 * opts.radius;  // M_SQRT2 * R from Snyder (1987), p 251
}

int
ProjectionMollweide::pixel_to_spher (int x, int y, double &lon, double &lat)
{
  double X = 2.0 * (x + width/2 - opts.centx) / width - 1;
  double Y = 1 - 2.0 * (y + height/2 - opts.centy) / height;

  double arg = Y / radius;
  if (fabs (arg) > 1) return (0);
  double theta = asin (arg);

  arg = (2 * theta + sin (2*theta)) / M_PI;
  if (fabs (arg) > 1) return (0);
  lat = asin (arg);

  if (fabs (theta) == M_PI)
    lon = 0;
  else 
    {
      lon = M_PI * X / (2 * radius * cos (theta));
      if (fabs (lon) > M_PI) return (0);
    }
 
  if (opts.obslon || opts.obslat || opts.rotate) rotate_point (lon, lat);

  if (lon > M_PI) lon -= TWO_PI;
  else if (lon < -M_PI) lon += TWO_PI;

  return (1);
}

int
ProjectionMollweide::spher_to_pixel (double lon, double lat, int &x, int &y)
{
  if (opts.obslon || opts.obslat || opts.rotate) rotate_point (lon, lat);

  double theta = lat;
  double del_theta = 1;
  while (fabs (del_theta) > 1e-5)
    {
      del_theta = -(theta + sin(theta) - M_PI * sin(lat)) / (1 + cos(theta));
      theta += del_theta;
    }
  theta /= 2;

  double X = (2 * radius / M_PI) * lon * cos (theta);
  double Y = radius * sin (theta);

  x = (int) ((X + 1) * width/2) + opts.centx - width/2;
  if (x < 0 || x >= width) return (0);

  y = (int) (height/2 * (1 - Y)) + opts.centy - height/2;
  if (y < 0 || y >= height) return (0);

  return (1);
}

