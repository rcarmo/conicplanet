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
#include <stdlib.h>
#include <math.h>

#include "ProjectionRectangular.h"
#include "location.h"
#include "options.h"
#include "util.h"

ProjectionRectangular::ProjectionRectangular (int w, int h) : Projection (w, h)
{
  int i;
  del_lon = TWO_PI/width;
  del_lat = M_PI/height;

  lon_array = new double [width];
  for (i = 0; i < width; i++) 
    lon_array[i] = (i + 0.5) * del_lon - M_PI;

  lat_array = new double [height];
  for (i = 0; i < height; i++)
    lat_array[i] = M_PI_2 - (i + 0.5) * del_lat;
}

double 
ProjectionRectangular::get_dellon ()
{
  return (del_lon);
}

double 
ProjectionRectangular::get_dellat ()
{
  return (del_lat);
}

/*
  Given lon, lat, returns x, y such that:
  lon_array[x-1] < lon < lon_array[x]
  lat_array[y-1] > lat > lat_array[y]

  for -M_PI   < x < M_PI
      -M_PI_2 < y < M_PI_2
*/
int
ProjectionRectangular::spher_to_pixel (double lon, double lat, int &x, int &y)
{
  x = int ((lon + M_PI)/del_lon + 0.5);

  if (x >= width)     
    x -= width;
  else if (x < 0)
    x += width;

  y = int ((M_PI_2 - lat)/del_lat + 0.5);
  if (y >= height) y = height - 1;

  return (1);
}

int
ProjectionRectangular::pixel_to_spher (int x, int y, double &lon, double &lat)
{
  lon = lon_array[x];
  lat = lat_array[y];
  return (1);
}


double
ProjectionRectangular::x_to_longitude (int x)
{
  return (lon_array[x]);
}

double
ProjectionRectangular::y_to_latitude (int y)
{
  return (lat_array[y]);
}

void
ProjectionRectangular::create_projection ()
{
  cout << "ProjectionRectangular::create_projection() called\n";
  exit (EXIT_FAILURE);
}

void
ProjectionRectangular::create_projection (double sunlon, double sunlat, 
					  unsigned char *day, 
					  unsigned char *night)
{
  int i, j, ii, jj;

  if (opts.fuzz == 0) 
    {
      // number of rows at top and bottom that are in polar day/night
      int ipolar = abs (int (sunlat/del_lat));

      if (sunlat < 0) // North pole is dark
	memcpy (day, night, 3 * ipolar * width);
      else            // South pole is dark
	memcpy (day + 3 * ((height - ipolar) * width),
		night + 3 * ((height - ipolar) * width), 
		3 * ipolar * width);

      // subsolar longitude pixel column - this is where it's noon
      int inoon = int (width/2 * (sunlon / M_PI - 1)); 
      while (inoon < 0) inoon += width;
      while (inoon >= width) inoon -= width;

      for (i = ipolar; i < height - ipolar; i++) 
	{ 
	  double length_of_day, H0;

	  /* compute length of daylight as a fraction of the day at 
	     the current latitude.  Based on Chapter 42 of Astronomical 
	     Formulae for Calculators by Meeus. */

	  H0 = tan (lat_array[i]) * tan (sunlat);
	  if (H0 > 1) length_of_day = 1;
	  else if (H0 < -1) length_of_day = 0;
	  else length_of_day = 1 - (acos (H0) / M_PI);

	  // ilight = number of pixels from noon to the terminator
	  int ilight = int (width/2 * length_of_day + 0.5);  

	  /* idark = number of pixels that are in darkness at the current 
	     latitude */
	  int idark = width - 2 * ilight;

	  // start at the evening terminator
	  int start_row = i * width;
	  int ipos = inoon + ilight;

	  for (j = 0; j < idark; j++) 
	    {
	      if (ipos >= width) ipos -= width;
	      memcpy (day + 3 * (start_row + ipos), 
		      night + 3 * (start_row + ipos), 3);
	      ipos++;
	    }
	} 
    }
  else  // opts.fuzz > 0
    {
      char *temp = new char [3 * width * height];
      memcpy (temp, day, 3 * width * height);

      location sunloc (sunlat, sunlon, 1, NULL);
      sunloc.spher_to_rect ();

      double border = sin (opts.fuzz * deg_to_rad);

      // break the image up into a 100x100 grid
      int istep = width/100;
      int jstep = height/100;
      if (istep == 0) istep = 1; 
      if (jstep == 0) jstep = 1;
      for (i = 0; i < width; i += istep)
	{
	  int istep2 = istep;
	  if (i + istep > width) istep2 = width - i;
	  for (j = 0; j < height; j += jstep)
	    {
	      location point (lat_array[j], lon_array[i], 1, NULL);
	      point.spher_to_rect ();
	      double x = point.dot (sunloc);
	      if (x < -2*border)  // NIGHT
		{
		  for (jj = 0; jj < jstep; jj++) 
		    {
		      if ((j + jj) >= height) break;
		      memcpy (temp + 3 * ((j+jj) * width + i),
			      night + 3 * ((j+jj) * width + i), 
			      3 * istep2);
		    }
		}
	      else if (x < 2*border ) // TWILIGHT
		{
		  for (jj = 0; jj < jstep; jj++)
		    {
		      int ilat = j + jj;
		      if (ilat >= height) break;
		      for (ii = 0; ii < istep; ii++)
			{
			  int ilon = i + ii;
			  if (ilon >= width) ilon -= width;
			  location tmp (lat_array[ilat], lon_array[ilon], 
					1, NULL);
			  tmp.spher_to_rect ();
			  double dayweight = ((border + tmp.dot (sunloc))
					      / (2 * border));
			  int ipos = 3 * (ilat * width + ilon);
			  if (dayweight < 0)
			    memcpy (temp + ipos, night + ipos, 3);
			  else if (dayweight < 1) 
			    {
			      dayweight = 0.5 * (1 - cos (dayweight * M_PI));
			      for (int k = 0; k < 3; k++)
				{
				  temp[ipos + k] = 
				    ((unsigned char) 
				     (dayweight * day[ipos + k] 
				      + ((1 - dayweight) * night[ipos + k])));
				}
			    }
			} // for ( ii = ... ) block
		    }     // for ( jj = ... ) block
		}         // end TWILIGHT block
	    }             // for ( j = ... ) block
	}                 // for ( i = ... ) block
      memcpy (day, temp, 3 * width * height);
      delete [] temp;
    }                   // end (opts.fuzz > 0) block

    memcpy (rgb_data, day, (3 * width * height));

  if (opts.projection == RECTANGULAR && opts.grid1) add_grid ();
}
