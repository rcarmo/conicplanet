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

#ifndef __PROJECTION_H__
#define __PROJECTION_H__

class Projection
{
 public:
  unsigned char *rgb_data;

  Projection (int w, int h);
  virtual void create_projection ();
  virtual int pixel_to_spher (int x, int y, double &lon, double &lat);
  virtual int spher_to_pixel (double lon, double lat, int &x, int &y);
  int get_width();
  int get_height();
  virtual void set_radius (double r);

 protected:
  int width, height;
  double radius;
  double rot[3][3];
  void add_grid ();
  void rotate_point (double &lon, double &lat);
  void set_background();
};
#endif
