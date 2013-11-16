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

#include <fstream>
#include <iostream>
#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "cgi.h"
#include "auxfiles.h"
#include "checkfuncs.h"
#include "cloud.h"
#include "options.h"
#include "planet.h"
#include "Projection.h"
#include "ProjectionRectangular.h"
#include "ProjectionOrthographic.h"
#include "ProjectionAzimuthal.h"
#include "ProjectionMollweide.h"
#include "ProjectionMercator.h"
#include "sunpos.h"
#include "moonpos.h"
#include "util.h"
#include "routines.h"

int texture_width=1024, texture_height=1024;

unsigned char *elevation = NULL;
ProjectionRectangular *image_rect;

int 
main (int argc, char **argv)
{ 
  double alpha, delta;    // Solar right ascension & declination
  double sunlon, sunlat, sundist; 
  int res;
  char *current_body;
  
  if ((res = cgi_init()) != CGIERR_NONE)
  {
  	printf("error initializing cgi!\n");
  	return 0;
  }	

  if (opts.contenttype == IMAGEJPEG)
	 printf("Content-type: image/jpeg\n\n");
  else if (opts.contenttype == IMAGEPNG)
	 printf("Content-type: image/png\n\n");
  else
	 printf("Content-type: image/jpeg\n\n");
  
  // Suggested by Francesco Usseglio Gaudi for date label
  setlocale (LC_ALL, "");
  
  process_args (argc, argv);
  
  switch (opts.body)
  {
  	case MERCURY	:	current_body = "mercury";
  				break;
  	case VENUS	:	current_body = "venus";
  				break;
  	case EARTH	:	current_body = "earth";
  				break;
  	case MOON	:	current_body = "moon";
  				break;
  	case MARS	:	current_body = "mars";
  				break;
  	case JUPITER	:	current_body = "jupiter";
  				break;
  	case IO		:	current_body = "io";
  				break;
  	case EUROPA	:	current_body = "europa";
  				break;
  	case GANYMEDE	:	current_body = "ganymede";
  				break;
  	case CALLISTO	:	current_body = "callisto";
  				break;
  	case SATURN	:	current_body = "saturn";
  				break;
  	case TITAN	:	current_body = "titan";
  				break;
  	case URANUS	:	current_body = "uranus";
  				break;
  	case NEPTUNE	:	current_body = "neptune";
  				break;
  	case PLUTO	:	current_body = "pluto";
  				break;
  	default		:	current_body = "earth";
  }

  double T = jcentury (opts.time.tv_sec);
  double eps = compute_obliquity (T);

  sunpos (sunlon, sunlat, sundist, T);

  if (opts.body == EARTH)     // Earth
    {
      compute_ra_dec (sunlon, sunlat, alpha, delta, eps);
      //  set sunlon and sunlat to subsolar longitude/latitude
      sunlon = TWO_PI * (alpha - gmst (T, opts.time.tv_sec)) / 24;
      sunlat = delta;
    }
  else 
    {
      planet selected (T);
      if (opts.body == MOON)
	{
	  selected.helio.lon = sunlon + M_PI;
	  selected.helio.lat = sunlat;
	  selected.helio.dist = sundist;
	}
      else
	selected.calc_helio ();

      opts.obslon *= -flipped;  

      coordinates subsolar = selected.calc_coordinates (selected.helio, eps);

      if (opts.position == EARTHSIDE)
	{
	  if (opts.body == MOON)
	    moonpos (selected.geo.lon, selected.geo.lat, selected.geo.dist, T);
	  else
	    selected.calc_geo (sunlon, sundist);

	  coordinates subearth = selected.calc_coordinates (selected.geo, eps);
	  opts.rotate += subearth.dist;       // position angle of north pole 
	  opts.obslon = subearth.lon;   
	  opts.obslon += subearth.alpha;      // correction for light_time
	  opts.obslat = subearth.lat;

	  subsolar.lon += subearth.alpha;     // correction for light_time
	}

      sunlon = subsolar.lon;
      sunlat = subsolar.lat;
    }

  switch (opts.position)
    {
    case DAYSIDE:
      opts.obslon = sunlon;
      opts.obslat = sunlat;
      break;
    case NIGHTSIDE:
      opts.obslon = sunlon + M_PI;
      opts.obslat = -sunlat;
      break;
    case MOONSIDE:
      double moonlon, moonlat, moondist;
      moonpos (moonlon, moonlat, moondist, T);
      compute_ra_dec (moonlon, moonlat, alpha, delta, eps);
      opts.obslon = TWO_PI * (alpha - gmst(T, opts.time.tv_sec)) / 24;
      opts.obslat = delta;
      break;
    default:
      break;
    }

  if (opts.terminator != 0)
    {
      double H0 = tan (sunlat) * tan (opts.obslat);

      if (H0 > 1)                      // Polar day, return midnight
	{
	  opts.obslon = sunlon + M_PI;
	  opts.rotate += M_PI_2;
	}
      else if (H0 < -1)                // Polar night, return noon
	{
	  opts.obslon = sunlon;
	  opts.rotate += M_PI_2;
	}
      else
	{
	  // halflength goes from 0 (polar night) to M_PI (polar day)
	  double halflength_of_day = M_PI - acos (H0);

	  double dH0 = (tan (sunlat) 
			* (1 + tan (opts.obslat) * tan (opts.obslat)));

	  // Make the terminator vertical (if opts.rotate == 0)
	  // This is off by a couple of degrees near solstice around 
	  // 45 degrees
	  double delrot = atan (dH0);

	  // I thought that this would be the right expression 
	  // (derivative of halflength) but it's worse than using dH0
	  //	  delrot = atan (dH0 / sqrt (1 - H0 * H0));

	  if (opts.terminator == MORNING)
	    {
	      opts.obslon = sunlon - halflength_of_day;
	      opts.rotate -= delrot;
	    }
	  else
	    {
	      opts.obslon = sunlon + halflength_of_day;
	      opts.rotate += delrot;
	    }
	}
    }

  while (opts.obslon < -M_PI) opts.obslon += TWO_PI;
  while (opts.obslon > M_PI) opts.obslon -= TWO_PI;

  int image_width, image_height;
  int window_width, window_height;
  unsigned char *day = NULL;
  unsigned char *night = NULL;

  opts.window_width *= opts.smoothen;
  opts.window_height *= opts.smoothen;
  opts.centx *= opts.smoothen;
  opts.centy *= opts.smoothen;

  init_display (window_width, window_height);

   opts.day = new char [300];
   strcpy (opts.day, dirdata);
   strcat (opts.day, current_body);
   if ((opts.window_width > 800) || (opts.window_height > 800) || (opts.radius > 75))
   	strcat(opts.day, "-highres");
   
   strcat (opts.day, defaultmapext);

   opts.night = new char [300];
   strcpy (opts.night, dirdata);
   strcat (opts.night, current_body);
   strcat (opts.night, "-night");
   strcat (opts.night, defaultmapext);

   opts.cloud = new char [300];
   strcpy (opts.cloud, dirdata);
   strcat (opts.cloud, current_body);
   strcat (opts.cloud, "-cloud.png");
  
  if (opts.projection == RECTANGULAR)
    {
      image_width = window_width;
      image_height = window_height;
    } 
  else // set dimensions to image file's dimensions
    {
      image_width = -1;
      image_height = -1;
    }

  read_image_file (opts.day, image_width, image_height, day);
  
  // the following is esp for the cloud handling code below
  if (opts.usenight != 0)
  	opts.shade = 0.3;

  if ((opts.usenight != 0) && (strncasecmp(current_body, "earth", 5) == 0))
    read_image_file (opts.night, image_width, image_height, night);
  else
    {
      night = (unsigned char *) malloc(3 * image_width * image_height);
      memcpy (night, day, 3 * image_width * image_height); 
      for (int i = 0; i < 3 * image_width * image_height; i++) 
	night[i] = (unsigned char) (night [i] * opts.shade);
    }

  if ((opts.usecloud != 0) && (strncasecmp(current_body, "earth", 5) == 0))
    {
      unsigned char *cloud = NULL;
      read_ssec_image (opts.cloud, image_width, image_height, cloud);

      // overlay the cloud image onto the day & night maps
      for (int j = 0; j < 3 * image_width * image_height; j += 3) 
	{
	
	  if ((cloud[j] >= opts.cloudlevel) && (cloud[j + 1] >= opts.cloudlevel) &&
	  	(cloud[j + 2] >= opts.cloudlevel))
	  	for (int i = j; i < (j + 3); i++)
		  {
		  	double opacity = double (cloud[i] / 255.);
			day[i] = (unsigned char) (opacity * cloud[i] 
					    + (1-opacity) * (day[i]));
			night[i] = (unsigned char) (opacity * opts.shade * cloud[i] 
					      + (1-opacity) * (night[i]));
		  }
	}
       free(cloud);
    }

  /*
    This object will hold the day/night image used to create the other 
    projections.
  */
  image_rect = new ProjectionRectangular (image_width, image_height);
  image_rect->create_projection (sunlon, sunlat, day, night);

  free(day);
  free(night);

  Projection *image = NULL;

  switch (opts.projection)
    {
    case AZIMUTHAL:
      image = new ProjectionAzimuthal (window_width, window_height);
      break;
    case MERCATOR:
      image = new ProjectionMercator (window_width, window_height);
      break;
    case MOLLWEIDE:
      image = new ProjectionMollweide (window_width, window_height);
      break;
    case ORTHOGRAPHIC:
      image = new ProjectionOrthographic (window_width, window_height);
      break;
    case RECTANGULAR:
      image = image_rect;
      break;
    default:
      break;
    }
  if (opts.projection != RECTANGULAR)
     image->create_projection ();
  
  render (image, sunlon, sunlat);
  
  cgi_quit();
  return 0;
}

