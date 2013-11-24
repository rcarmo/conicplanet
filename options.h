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

#include <sys/time.h>

enum body_name
{
  MERCURY, VENUS, EARTH, MOON, MARS,
  JUPITER, IO, EUROPA, GANYMEDE, CALLISTO,
  SATURN, TITAN, URANUS, NEPTUNE, PLUTO,
  RANDOMBODY
};

enum optionType
{
  AZIMUTHAL, BODY, CENTER, DAYSIDE, NIGHTSIDE, EARTHSIDE, FILTER, MOONSIDE,
  FIXED, FUZZ, GRID, GRID1, GRID2, MARKERS, MERCATOR, MOLLWEIDE,
  OBSERVER, ORTHOGRAPHIC, PROJECTION, RADIUS, RANGE, RECTANGULAR, ROTATE,
  SHADE, STARFREQ, EVENING, MORNING
};

enum ContentTypes
{
  IMAGEJPEG, IMAGEPNG
};

class options 
{
 public:
  char         *dirdata;
  char         *defaultmapext;
  int          body;
  int          brightness;
  int          centx;
  int          centy;
  char         *cloud;
  int          cloudlevel;
  int          contenttype;
  char         *day;
  int          filter;
  int          fuzz;
  double       gamma;
  int          greyscale;
  int          grid1;
  int          grid2;
  int          markers;
  char         *night;
  int          projection;
  double       obslat, obslon;
  int          position;
  int          radius;
  double       range;
  double       rotate;
  double       shade;
  int          size;
  int          smoothen;
  double       starfreq;
  struct timeval time;
  int          terminator;
  int          usecloud;
  int          usenight;
  bool         printcontenttype;

  unsigned int window_width;
  unsigned int window_height;
  int          window_x;
  int          window_y;
  unsigned int temp;
  int          label_x;
  int          label_y;

  options() 
    {
      body             = 0;
      brightness       = 100;
      centx            = -1;
      centy            = -1;
      cloud            = NULL;
      cloudlevel       = 90;
      contenttype      = IMAGEJPEG;
      day              = NULL;
      filter           = 0;
      fuzz             = 6;
      gamma            = 1;
      greyscale        = 0;
      grid1            = 0;
      grid2            = 15;
      markers          = 0;
      night            = NULL;
      projection       = RECTANGULAR;
      obslat           = 0;
      obslon           = 0;
      position         = FIXED;
      radius           = 50;
      range            = 1000;
      rotate           = 0;
      shade            = 0.3;
      smoothen         = 1;
      starfreq         = 1E-3;
      terminator       = 0;
      usecloud         = 0;
      usenight         = 0;

      window_width     = 600;
      window_height    = 600;
      window_x         = 0;
      window_y         = 0;
      printcontenttype = false;

      gettimeofday (&time, NULL);
    }
  
}; 

extern options opts;

extern void process_args (int argc, char **argv);
