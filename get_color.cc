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

#include <math.h>
#include <string.h>

#include "options.h"
#include "util.h"
#include "conicplanet.h"

void
get_color (const double lon, const double lat, unsigned char pixel[3]) 
{
  if (opts.filter == 0)
    {
      int ilon, ilat;
      image_rect->spher_to_pixel (lon, lat, ilon, ilat);
      memcpy (pixel, image_rect->rgb_data + 
	      3 * (ilat*image_rect->get_width() + ilon), 3);
      return;
    }

  /*
    The point (lon, lat) falls into one of these four pixels.  The 
    color returned will be an area weighted average of these four.
    
     --- ---
    | 0 | 1 |
     --- ---
    | 2 | 3 |
     --- ---
  */
  
  int ii, jj;
  int ix[4], iy[4];

  image_rect->spher_to_pixel (lon, lat, ix[1], iy[2]);

  if (ix[1] == 0)
    {
      ix[0] = image_rect->get_width() - 1;
      ix[1] = 0;
    }
  else
    {
      ix[0] = ix[1] - 1;
    }

  iy[0] = iy[2] - 1;
  if (iy[0] == -1) iy[0] = 0;

  ix[2] = ix[0];
  ix[3] = ix[1];
  iy[1] = iy[0];
  iy[3] = iy[2];
  
  double weight[4];
  double t = lon - image_rect->x_to_longitude (ix[2]);
  if (t > M_PI) t -= TWO_PI;
  if (t < -M_PI) t += TWO_PI;
  t /= image_rect->get_dellon ();

  double u = ((lat - image_rect->y_to_latitude (iy[2]))
	      / image_rect->get_dellat ());

  // Weights are from Numerical Recipes, 2nd Edition
  //	      weight[0] = (1 - t) * u;
  //	      weight[2] = (1-t) * (1-u);
  //	      weight[3] = t * (1-u);
  weight[1] = t * u;
  weight[0] = u - weight[1];
  weight[2] = 1 - t - u + weight[1];
  weight[3] = t - weight[1];

  memset (pixel, 0, 3);
  for (ii = 0; ii < 4; ii++)
    {
      int ipos = 3*(iy[ii]*image_rect->get_width() + ix[ii]);
      for (jj = 0; jj < 3; jj++) 
	{
	  pixel[jj] += (unsigned char) 
	    (weight[ii] * image_rect->rgb_data[ipos+jj]);
	}
    }
}

