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
#include "ProjectionMercator.h"

ProjectionMercator::ProjectionMercator (int w, int h) 
  : Projection (w, h)  // call the Projection constructor
{
}

int
ProjectionMercator::pixel_to_spher (int x, int y, double &lon, double &lat)
{
  double X = ((double) (x - width/2)) * TWO_PI / width;
  double Y = ((double) (height/2 - y)) * 1.5 * M_PI / height;

  lat = atan (sinh (Y));
  lon = X;
  
  if (opts.obslon || opts.obslat || opts.rotate) rotate_point (lon, lat);
  
  if (lon > M_PI) lon -= TWO_PI;
  else if (lon < -M_PI) lon += TWO_PI;

  return (1);
}

int
ProjectionMercator::spher_to_pixel (double lon, double lat, int &x, int &y)
{
  double X, Y;

  if (opts.obslon || opts.obslat || opts.rotate) rotate_point (lon, lat);
  
  X = lon;
  Y = log (tan (M_PI/4 + lat/2));

  if (X >= M_PI) X -= TWO_PI;
  else if (X < -M_PI) X += TWO_PI;

  x = (int) ((width * X / TWO_PI) + width / 2);
  y = (int) (height / 2 - (height * Y / (1.5 * M_PI)));
  
  if (y < 0 || y >= height) return (0);
  return (1);
}
