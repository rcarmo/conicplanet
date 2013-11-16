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
#include <time.h>
#include <sys/time.h>

extern const double deg_to_rad = M_PI/180.;
extern const double TWO_PI = 2 * M_PI;

double 
poly (double a1, double a2, double a3, double a4, double t)
{
  return(a1 + t*(a2 + t*(a3 + t*a4)));
}

double 
julian (int year, int month, int day, int hour, int min, int sec)
{
  int a, b, c, d;
  double e, jd;

  if(month < 3) 
    {
      year -= 1;
      month += 12;
    }      
  
  // assume it's the Gregorian calendar (after 1582 Oct 15)

  a = year/100;
  b = 2 - a + a/4;
  c = int (365.25 * year);
  d = int (30.6001 * (month + 1));
  e = day + hour/24. + min/1400. + sec/86400.;

  jd = b + c + d + e + 1720994.5;

  return jd;
}

double 
jcentury (time_t tv_sec)
{
  double jd = julian (gmtime (&tv_sec)->tm_year + 1900,
		      gmtime (&tv_sec)->tm_mon + 1,
		      gmtime (&tv_sec)->tm_mday,
		      gmtime (&tv_sec)->tm_hour,
		      gmtime (&tv_sec)->tm_min,
		      gmtime (&tv_sec)->tm_sec);
  
  return ((jd - 2415020)/36525);
}

double 
kepler (double e, double M)
{
  double E = M;
  double delta = 1;

  while ( fabs (delta) > 1E-10 )
    {
      delta = (M + e * sin (E) - E)/(1 - e * cos (E));
      E += delta;
    }
  return (E);
}

double
gmst (double T, time_t tv_sec)
{      
  // Sidereal time at Greenwich at 0 UT
  double g = poly (6.6460656, 2400.051262, 0.00002581, 0, T);

  // Now find current sidereal time at Greenwich
  double currgmt = (gmtime (&tv_sec)->tm_hour 
		    + gmtime (&tv_sec)->tm_min/60.
		    + gmtime (&tv_sec)->tm_sec/3600.);
  currgmt *= 1.002737908;
  g += currgmt;
  g = fmod (g, 24.0);
  if (g < 0) g += 24;
  return (g);
}

double
compute_obliquity ( double T )
{
  return (poly (23.452294, -1.30125E-2, -1.64E-6, 5.03E-7, T)
	  * deg_to_rad);
}

void 
compute_ra_dec (double lon, double lat, double &alpha, 
		double &delta, double eps)
{
  delta = asin (sin (eps) * sin (lon) * cos (lat) + sin (lat) * cos (eps));

  alpha = (atan2 (cos (eps) * sin (lon) - tan (lat) * sin (eps), cos (lon)) 
	   / deg_to_rad);
  alpha /= 15;
  alpha = fmod (alpha, 24.0);
  if (alpha < 0) alpha += 24;
}

void rotate_xyz (double matrix[3][3], double angle_x, 
		 double angle_y, double angle_z)
{
  /*
    matrix to first rotate reference frame angle_x radians through 
    x axis, then angle_y radians through y axis, and lastly 
    angle_z radians through z axis.  Positive rotations are counter-
    clockwise looking down the axis.
  */

  matrix[0][0] = cos(angle_y) * cos(angle_z);
  matrix[0][1] = (sin(angle_x) * sin(angle_y) * cos(angle_z) 
		  + cos(angle_x) * sin(angle_z));
  matrix[0][2] = (-cos(angle_x) * sin(angle_y) * cos(angle_z) +
		  sin(angle_x) * sin(angle_z));
  matrix[1][0] = -cos(angle_y) * sin(angle_z);
  matrix[1][1] = (-sin(angle_x) * sin(angle_y) * sin(angle_z)
		  + cos(angle_x) * cos(angle_z));
  matrix[1][2] = (cos(angle_x) * sin(angle_y) * sin(angle_z)
		  + sin(angle_x) * cos(angle_z));
  matrix[2][0] = sin(angle_y);
  matrix[2][1] = -sin(angle_x) * cos(angle_y);
  matrix[2][2] = cos(angle_x) * cos(angle_y);
}

void rotate_zyx (double matrix[3][3], double angle_x, 
		 double angle_y, double angle_z)
{
  /*
    matrix to first rotate reference frame angle_z radians through 
    z axis, then angle_y radians through y axis, and lastly 
    angle_x radians through x axis.  Positive rotations are counter-
    clockwise looking down the axis.
  */

  matrix[0][0] = cos(angle_y) * cos(angle_z);
  matrix[0][1] = cos(angle_y) * sin(angle_z);
  matrix[0][2] = -sin(angle_y);
  matrix[1][0] = (-cos(angle_x) * sin(angle_z)
		  + sin(angle_x) * sin(angle_y) * cos(angle_z));
  matrix[1][1] = (sin(angle_x) * sin(angle_y) * sin(angle_z)
		  + cos(angle_x) * cos(angle_z));
  matrix[1][2] = sin(angle_x) * cos(angle_y);
  matrix[2][0] = (cos(angle_x) * sin(angle_y) * cos(angle_z)
		  + sin(angle_x) * sin(angle_z));
  matrix[2][1] = (-sin(angle_x) * cos(angle_z)
		  + cos(angle_x) * sin(angle_y) * sin(angle_z));
  matrix[2][2] = cos(angle_x) * cos(angle_y);
}
