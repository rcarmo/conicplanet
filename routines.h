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

#include "Projection.h"

extern void init_display (int &win_width, int &win_height);
extern void render (Projection *image, const double sunlon, 
		    const double sunlat);

extern unsigned char *crop_image (unsigned char *olddata, int old_width, int old_height, int x, int y,
	    int width, int height);

extern void read_image_file (char *filename, int &width, int &height, 
			     unsigned char *&rgb_data);
extern unsigned char *read_png (char *filename, int &width, int &height);
extern unsigned char *clone_scaled_image (unsigned char *olddata, int new_width, int new_height, int old_width, int old_height, int nearest_neighbor = 0);
