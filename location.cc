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

#include "location.h"
#include "util.h"

location::location (double r) 
{
  lon = 0.;
  lat = 0.;
  range = r;
}

location::location (double a, double b, double c) 
{
  x = a;
  y = b;
  z = c;
}
  
location::location (double latitude, double longitude, double r, 
		    char *text)
{
  lat = latitude;
  if (longitude > M_PI) longitude -= TWO_PI;
  else if (longitude < -M_PI) longitude += TWO_PI;
  lon = longitude;
  range = r;

  if (text != NULL) 
    name = strdup (text);
  else 
    name = NULL;
}

void
location::setlocation (double a, double b, double c)
{
  x = a; 
  y = b;
  z = c;
}

double
location::dot (location l1)
{
  return (x * l1.x + y * l1.y + z * l1.z);
}

void 
location::rotate (double a[3][3]) 
{
  double newx = a[0][0] * x + a[0][1] * y + a[0][2] * z;
  double newy = a[1][0] * x + a[1][1] * y + a[1][2] * z;
  double newz = a[2][0] * x + a[2][1] * y + a[2][2] * z;
      
  x = newx;
  y = newy;
  z = newz;
}

void 
location::spher_to_rect (void) 
{
  x = range * cos (lon) * cos (lat);
  y = range * sin (lon) * cos (lat);
  z = range * sin (lat);
}
  
void 
location::rect_to_spher (void) 
{
  range = sqrt (x*x + y*y + z*z);
  lat = M_PI_2 - acos (z/range);
  lon = atan2 (y,x);
}
