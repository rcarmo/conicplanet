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
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgi.h"
#include "checkfuncs.h"
#include "options.h"
#include "util.h"
#include "conicplanet.h"

options opts;

void 
process_args (int argc, char **argv)
{
	const char	*temp;

	temp = cgi_getentrystr("contenttype");

	if (temp == NULL) temp = "";	
	
	if (strncasecmp(temp, "jpeg", 4) == 0)
		opts.contenttype = IMAGEJPEG;
	else if (strncasecmp(temp, "png", 3) == 0)
		opts.contenttype = IMAGEPNG;
	else
		opts.contenttype = IMAGEJPEG;

	temp = cgi_getentrystr("projection");

	if (temp == NULL) temp = "";	
	
	if (strncasecmp(temp, "azimuthal", 9) == 0)
		opts.projection = AZIMUTHAL;
	else if (strncasecmp(temp, "mercator", 8) == 0)
		opts.projection = MERCATOR;
	else if (strncasecmp(temp, "mollweide", 9) == 0)
		opts.projection = MOLLWEIDE;
	else if (strncasecmp(temp, "orthographic", 12) == 0)
		opts.projection = ORTHOGRAPHIC;
	else if (strncasecmp(temp, "rectangular", 11) == 0)
		opts.projection = RECTANGULAR;
	else
		opts.projection = RECTANGULAR;

	if (cgi_getentryint("filter") == 1)
	{
		opts.filter = 1;
	  	if (opts.projection == RECTANGULAR)
	  		opts.projection = ORTHOGRAPHIC;
	}
	
	opts.smoothen = cgi_getentryint("smoothen");
	
	if (opts.smoothen < 1)
		opts.smoothen = 1;

	if (opts.smoothen > 4)
		opts.smoothen = 4;

	temp = cgi_getentrystr("body");
	
	if (temp == NULL) temp = "";
	
	if (strncasecmp(temp, "earth", 5) == 0)
		opts.body = EARTH;
	else if (strncasecmp(temp, "mercury", 7) == 0)
		opts.body = MERCURY;
	else if (strncasecmp(temp, "venus", 5) == 0)
		opts.body = VENUS;
	else if (strncasecmp(temp, "moon", 4) == 0)
		opts.body = MOON;
	else if (strncasecmp(temp, "mars", 4) == 0)
		opts.body = MARS;
	else if (strncasecmp(temp, "jupiter", 7) == 0)
		opts.body = JUPITER;
	else if (strncasecmp(temp, "io", 2) == 0)
		opts.body = IO;
	else if (strncasecmp(temp, "europa", 6) == 0)
		opts.body = EUROPA;
	else if (strncasecmp(temp, "ganymede", 8) == 0)
		opts.body = GANYMEDE;
	else if (strncasecmp(temp, "callisto", 8) == 0)
		opts.body = CALLISTO;
	else if (strncasecmp(temp, "saturn", 6) == 0)
		opts.body = SATURN;
	else if (strncasecmp(temp, "titan", 5) == 0)
		opts.body = TITAN;
	else if (strncasecmp(temp, "uranus", 6) == 0)
		opts.body = URANUS;
	else if (strncasecmp(temp, "neptune", 7) == 0)
		opts.body = NEPTUNE;
	else if (strncasecmp(temp, "pluto", 5) == 0)
		opts.body = PLUTO;
	else
		opts.body = EARTH;

	opts.centx = cgi_getentryint("centerx");
	opts.centy = cgi_getentryint("centery");
	
	if (opts.centx == 0)
		opts.centx = -1;
	if (opts.centy == 0)
		opts.centy = -1;
	
	if (((opts.centx != -1) || (opts.centy != -1)) && (opts.projection == RECTANGULAR))
		opts.projection = ORTHOGRAPHIC;
	
	if (cgi_getentryint("greyscale") == 1)
		opts.greyscale = 1;

	if (cgi_getentryint("clouds") == 1)
		opts.usecloud = 1;

	opts.cloudlevel = cgi_getentryint("cloudlevel");
	
	if (opts.cloudlevel == 0)
		opts.cloudlevel = 90;

	if (cgi_getentryint("night") == 1)
		opts.usenight = 1;


	temp = cgi_getentrystr("position");
	
	if (temp == NULL) temp = "";
	
	if (strncasecmp(temp, "fixed", 5) == 0)
		opts.position = FIXED;
	else if (strncasecmp(temp, "dayside", 7) == 0)
		opts.position = DAYSIDE;
	else if (strncasecmp(temp, "nightside", 9) == 0)
		opts.position = NIGHTSIDE;
	else if (strncasecmp(temp, "earthside", 9) == 0)
		opts.position = EARTHSIDE;
	else if (strncasecmp(temp, "moonside", 8) == 0)
		opts.position = MOONSIDE;
	else
		opts.position = FIXED;

/*
	case RANDOM:
          opts.obslon = random() % 360;
          opts.obslon *= deg_to_rad;
	  // Weight random latitudes towards the equator
	  opts.obslat = (random() % 2000)/1000.0 - 1;
	  opts.obslat = asin (opts.obslat);
          opts.rotate = random() % 360;
          opts.rotate *= deg_to_rad;
	  if (opts.projection == RECTANGULAR) opts.projection = ORTHOGRAPHIC;
*/	  
	
	if ((opts.position != FIXED) && (opts.projection == RECTANGULAR))
		opts.projection = ORTHOGRAPHIC;

	if (cgi_getentryint("fuzz") > 0)
		opts.fuzz = cgi_getentryint("fuzz");
	
	opts.window_width = cgi_getentryint("width");
	opts.window_height = cgi_getentryint("height");
	if ((opts.window_width < 16) || (opts.window_width > 4800))
		opts.window_width = 600;
	if ((opts.window_height < 16) || (opts.window_height > 4800))
		opts.window_height = 600;
	
	if (cgi_getentryint("grid") == 1)
		opts.grid1 = 6;
	
	if ((cgi_getentryint("grid1") > 0) && (cgi_getentryint("grid1") < 90))
		opts.grid1 = cgi_getentryint("grid1");
	
	if (cgi_getentryint("grid2") > 0)
		opts.grid2 = cgi_getentryint("grid2");

	if (cgi_getentryint("markers") == 1)
		opts.markers = 1;

	opts.obslon = (float) cgi_getentrydouble("longitude");
	opts.obslat = (float) cgi_getentrydouble("latitude");

	  if (opts.obslon < 0 || opts.obslon > 360) {
	    opts.obslon = fmod (opts.obslon, 360.);
	    if (opts.obslon < 0) opts.obslon += 360;
	  }
	  
	  if (opts.obslat < -90 || opts.obslat > 90) {
	    while(opts.obslat < -90) opts.obslat =  -180 - opts.obslat;
	    while(opts.obslat > 90) opts.obslat = 180 - opts.obslat;
	  }

	  if (((opts.obslon != 0) || (opts.obslat != 0)) && (opts.projection == RECTANGULAR))
	  	 opts.projection = ORTHOGRAPHIC;

	  opts.obslon *= deg_to_rad;
	  opts.obslat *= deg_to_rad;

	if (cgi_getentryint("radius") > 0)
		opts.radius = cgi_getentryint("radius");
	
	if ((opts.radius != 50) && (opts.projection == RECTANGULAR))
		opts.projection = ORTHOGRAPHIC;

	if (cgi_getentrydouble("range") > 0)
	{
		opts.range = cgi_getentrydouble("range");
		opts.projection = ORTHOGRAPHIC;
	}

	opts.rotate = (float) cgi_getentrydouble("rotate");
	  if (opts.rotate < 0 || opts.rotate > 360) 
	    {
	      opts.rotate = fmod (opts.rotate, 360.);
	      if (opts.rotate < 0) opts.rotate += 360;
	    }
	  opts.rotate *= deg_to_rad;

	  if ((opts.rotate != 0) && (opts.projection == RECTANGULAR))
	  	 opts.projection = ORTHOGRAPHIC;


	if (cgi_getentrydouble("shade") > 0)
	{
	  opts.shade = cgi_getentrydouble("shade");

	  if (opts.shade < 0 || opts.shade > 100) 
	    {
	      if (opts.shade < 0) opts.shade = 0;
	      else if (opts.shade > 100) opts.shade = 100;
	    }
	  opts.shade = opts.shade/100.;
	}

	if (cgi_getentrydouble("starfreq") > 0)
	{
	  opts.starfreq = cgi_getentrydouble("starfreq");

	  if (opts.starfreq < 0)
	  	opts.starfreq = 0;
	  else if (opts.starfreq > 1)
	  	opts.starfreq = 1;

	  if ((opts.starfreq != 1E-3) && (opts.projection == RECTANGULAR))
	  	 opts.projection = ORTHOGRAPHIC;

	}

	if (cgi_getentryint("brightness") > 0)
	{
	  opts.brightness = cgi_getentryint("brightness");

	  if (opts.brightness <= 0)
	  	opts.brightness = 1;
	  else if (opts.brightness > 200)
	  	opts.brightness = 200;
	}

	if (cgi_getentrydouble("gamma") > 0)
	{
	  opts.gamma = cgi_getentrydouble("gamma");

	  if (opts.gamma <= 0)
	  	opts.gamma = 0.1;
	  else if (opts.gamma > 5)
	  	opts.gamma = 5;
	}
	
	temp = cgi_getentrystr("terminator");

	if (temp == NULL) temp = "";	
	
	if (strncasecmp(temp, "evening", 7) == 0)
		opts.terminator = EVENING;
	else if (strncasecmp(temp, "morning", 7) == 0)
		opts.terminator = MORNING;
	else
		opts.terminator = 0;
}

