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

#ifndef __LOCATION_H__
#define __LOCATION_H__

#include <math.h>
#include <stdio.h>

class location 
{
 public:
  double x,  // x axis points through (0,0)
    y,       // y axis points through (90,0)
    z;       // z axis points through north pole
  double lon,
    lat,
    range;
  char *name;
  
  location (double r);
  location (double latitude, double longitude, double r, 
     char *text);
  location (double a, double b, double c);

  void setlocation (double a, double b, double c);

  double dot (location l1);
  void rotate (double a[3][3]);

  void spher_to_rect (void);
  void rect_to_spher (void);
};

#endif
