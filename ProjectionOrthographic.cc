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

#include <stdlib.h>
#include <math.h>
#include <string.h>

#include <iostream>

#include "get_color.h"
#include "location.h"
#include "options.h"
#include "planet.h"
#include "ProjectionOrthographic.h"
#include "util.h"
#include "conicplanet.h"
#include "routines.h"

static double dist_per_pixel;
static double rad_per_pixel;

static double cosalph, cosbeta, cosgamm; // direction cosines of ray

ProjectionOrthographic::ProjectionOrthographic (int w, int h) 
  : Projection (w, h) // call the Projection constructor
{
  set_background ();

  if ((opts.position == DAYSIDE) || (opts.position == NIGHTSIDE))
    opts.fuzz = 0;

  // convert opts.radius from percent to pixels
  opts.radius = int (0.01 * opts.radius * height);
  radius = 1;
}

void
ProjectionOrthographic::create_projection ()
{
  int i, j;

  dist_per_pixel = 1. / opts.radius;
  dist_per_pixel /= cos (1/opts.range);
  rad_per_pixel = dist_per_pixel / opts.range;

  // The rendered globe is contained in a square with sides of length
  // iside and upper left corner at (istart, jstart).
  int iside = 2 * opts.radius;

  // (istart, jstart) is the upper left corner of the square 
  // containing the rendered globe.
  int istart = opts.centx - iside/2;
  int jstart = opts.centy - iside/2;

  double range_squared = opts.range * opts.range;

  double range_dpp_squared = range_squared / (dist_per_pixel * dist_per_pixel);
 
  double a = 1.0;
  double c = range_squared - 1;

  location intersection (opts.range);
  location observer (opts.range);
  observer.spher_to_rect();

  for (j = jstart; j < jstart + iside; j++) 
    {
      if (j < 0 || j >= height) continue;
      int jdist = opts.centy - j;         // decreases downward
      cosgamm = jdist / sqrt (jdist * jdist + range_dpp_squared);
      for (i = istart; i < istart + iside; i++) 
	{
	  if (i < 0 || i >= width) continue;
	  int idist = i - opts.centx;         // increases to the right
 	  double dist = sqrt ( (double) (idist * idist + jdist * jdist) );
	  cosalph = -cos (dist * rad_per_pixel);
	  dist = sqrt (idist * idist + range_dpp_squared);
	  cosbeta = idist / dist;
	  double b = 2 * (cosalph * observer.x + cosbeta * observer.y 
			  + cosgamm * observer.z);
	  
	  int ilon, ilat;

	      /* If the ray doesn't intersect the globe go to the 
		 next pixel */
	      if (!calc_intersect (observer, intersection, a, b, c, 
				   ilon, ilat)) continue;
	      
	  unsigned char pixel[3] = {0,0,0};
	  
	  if (opts.filter == 0)
	    memcpy (pixel, image_rect->rgb_data + 
		    3 * (ilat*image_rect->get_width() + ilon), 3);
	  else
	    get_color (intersection.lon, intersection.lat, pixel);

	  memcpy (rgb_data + 3 * (j * width + i), pixel, 3);
	  
	}         // for (i = istart; i < istart + 2*opts.radius; i++) 
    }             // for (j = jstart; j < jstart + 2*opts.radius; j++) 

  // Rotation matrix for grid and markers
  rotate_zyx (rot, -opts.rotate, -opts.obslat, opts.obslon);
  if (opts.grid1) add_grid ();
}

int
ProjectionOrthographic::spher_to_pixel (double lon, double lat, int &x, int &y)
{
  location l (lat, lon, radius, NULL);
  l.spher_to_rect ();
  l.rotate (rot);
  l.rect_to_spher ();

  if (l.x < 0) return (0);
  x = opts.centx + int (l.y * opts.radius);
  if (x < 0 || x >= width) return (0);

  y = opts.centy - int (l.z * opts.radius);
  if (y < 0 || y >= height) return (0);

  return (1);
}

void
ProjectionOrthographic::set_radius (double r)
{
  radius = r;
}

/* 
   Returns the indices of the lon and lat of the intersection between the 
   ray and the globe.
*/
int 
ProjectionOrthographic::calc_intersect (const location observer, 
					location &intersection, 
					const double a, const double b, 
					const double c, int &ilon, int &ilat)
{
  double det = b*b - 4*a*c;
  if (det < 0) return (0);

  double sqrtdet = sqrt (det);
  double t1 = -b + sqrtdet;
  double t2 = -b - sqrtdet;
  double t = (t1 < t2 ? t1 : t2);
  t /= (2*a);
	      
  intersection.setlocation (observer.x + cosalph*t,
			    observer.y + cosbeta*t,
			    observer.z + cosgamm*t);
	      
  if (opts.obslon || opts.obslat || opts.rotate) 
    intersection.rotate (rot);
  
  intersection.rect_to_spher ();

  image_rect->spher_to_pixel (intersection.lon, intersection.lat, ilon, ilat);

  return (1);
}
