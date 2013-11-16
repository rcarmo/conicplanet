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

#include <iostream>
#include <stdlib.h>
#include <string.h>

#include "get_color.h"
#include "location.h"
#include "options.h"
#include "planet.h"
#include "util.h"
#include "routines.h"

Projection::Projection (int w, int h)
{
  width = w; 
  height = h;

  rgb_data = new unsigned char[3 * width * height];
  memset (rgb_data, 0, 3 * width * height);

  rotate_xyz (rot, opts.rotate, opts.obslat, -opts.obslon);
}

void
Projection::create_projection ()
{
  double lon, lat;
  for (int j = 0; j < height ; j++)
    {
      for (int i = 0; i < width; i++)
	{
	  if (pixel_to_spher (i, j, lon, lat))
	    get_color (lon, lat, rgb_data + 3 * (j * width + i));
	}
    }

  // set rotation matrix for grid and markers
  rotate_zyx (rot, -opts.rotate, -opts.obslat, opts.obslon);
  if (opts.grid1) add_grid ();
}

int 
Projection::pixel_to_spher (int x, int y, double &lon, double &lat)
{
  std::cout << "Projection::pixel_to_spher called.\n";
  exit (EXIT_FAILURE);
  return (0);    // keep some compilers happy
}

int 
Projection::spher_to_pixel (double lon, double lat, int &x, int &y)
{
  std::cout << "Projection::spher_to_pixel called.\n";
  exit (EXIT_FAILURE);
  return (0);    // keep some compilers happy
}

void
Projection::set_radius (double r)
{
  std::cout << "Projection::set_radius called.\n";
  exit (EXIT_FAILURE);
}

int 
Projection::get_width ()
{
  return (width);
}

int
Projection::get_height ()
{
  return (height);
}

void
Projection::add_grid ()
{
  double lon, lat;
  int ilon=0, ilat=0;

  // Grid points can be as close as 1/mag_factor degrees 
  int mag_factor = 10;
  int lon0 = -180, lon1 = 180, lat0 = -90, lat1 = 90;
  lon0 *= mag_factor;
  lon1 *= mag_factor;
  lat0 *= mag_factor;
  lat1 *= mag_factor;

  int grid_step = mag_factor * 90 / (opts.grid1 * opts.grid2);
  if (grid_step < 1) grid_step = 1;

  for (int i = lon0; i < lon1; i += (mag_factor * 90)/opts.grid1)
    {
      lon = ((double) i)/mag_factor * deg_to_rad;
      for (int j = lat0; j < lat1; j += grid_step)
	{
	  lat = ((double) j) / mag_factor * deg_to_rad;
	  if (spher_to_pixel (lon, lat, ilon, ilat))
	    memset (rgb_data + 3 * (ilat * width + ilon), 255, 3);
	}
    }

  for (int i = lon0; i < lon1; i += grid_step)
    {
      lon = ((double) i)/mag_factor * deg_to_rad;
      for (int j = lat0; j < lat1; j += (mag_factor * 90)/opts.grid1)
	{
	  lat = ((double) j) / mag_factor * deg_to_rad;
	  if (spher_to_pixel (lon, lat, ilon, ilat))
	    memset (rgb_data + 3 * (ilat * width + ilon), 255, 3);
	}
    }
}

void
Projection::rotate_point (double &lon, double &lat)
{
  location l (lat, lon, 1, NULL);
  l.spher_to_rect ();
  l.rotate (rot);
  l.rect_to_spher ();
  lon = l.lon;
  lat = l.lat;
}

void 
Projection::set_background ()
{
      // add random stars
      int num_stars = int (width * height * opts.starfreq);
      srandom ((unsigned int) opts.time.tv_sec);
      for (int i = 0; i < num_stars; i++)
	{
	  int j = random() % width;
	  int k = random() % height;
	  int brightness = random() % 256;
	  memset (rgb_data + 3 * (k * width + j), brightness, 3);
	}
}

