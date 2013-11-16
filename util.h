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

#include <time.h>

extern const double deg_to_rad;
extern const double TWO_PI;

extern double kepler (double e, double M);
extern double poly (double a1, double a2, double a3, double a4, double t);
extern double jcentury (time_t tv_sec);
extern double compute_obliquity ( double T );
extern void compute_ra_dec (double lon, double lat, double &alpha, 
			    double &delta, double eps);
extern double gmst (double T, time_t tv_sec);

extern void rotate_xyz (double matrix[3][3], double angle_x, 
			double angle_y, double angle_z);
extern void rotate_zyx (double matrix[3][3], double angle_x, 
			double angle_y, double angle_z);
