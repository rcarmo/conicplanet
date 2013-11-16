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
#include <math.h>
#include <stdlib.h>
#include <string.h>

#include "routines.h"
#include "options.h"
#include "util.h"

static void
moll_spher_to_pixel (int width, int height, double lon, double lat, 
		     int &x, int &y)
{
  double theta = lat;
  double del_theta = 1;
  while (fabs (del_theta) > 1e-5)
    {
      del_theta = -(theta + sin(theta) - M_PI * sin(lat)) / (1 + cos(theta));
      theta += del_theta;
    }
  theta /= 2;

  double X = lon / M_PI * cos (theta);
  double Y = sin (theta);

  x = (int) ((X + 1) * width/2);
  y = (int) (height/2 * (1 - Y));
}

static int
equalize_histogram (unsigned char *&rgb, int width, int height)
{
  int i;
  int *hist = (int *) malloc(256 * sizeof(int));
  memset (hist, 0, 256 * sizeof (int));
  for (i = 0; i < 3 * width * height; i += 3) hist[(int) rgb[i]]++;

  int *ihist = (int *) malloc(256 * sizeof(int));
  ihist[0] = hist[0];
  for (i = 1; i < 256; i++) ihist[i] = ihist[i-1] + hist[i];

  // replace the histogram by an intensity map
  double denom = (double) (ihist[255] - ihist[0]);
  if (denom < 1) return (0);

  for (i = 0; i < 256; i++)
    hist[i] = (int) (255 * (ihist[i] - ihist[0])/denom);

  for (i = 0; i < 3 * width * height; i++) 
    rgb[i] = (unsigned char) hist[(int) rgb[i]];

  free(hist);
  free(ihist);

  return (1);
}

static void
rect_pixel_to_spher (int width, int height, int x, int y, double &lon, 
		     double &lat)
{
  lon = (x + 0.5) * TWO_PI / width - M_PI;
  lat = M_PI_2 - (y + 0.5) * M_PI / height;
}

/*
  This routine reads in a global cloud image downloaded from
  http://www.ssec.wisc.edu/data/comp/latest_moll.gif 
  and reprojects and resizes the image, gets rid of the ugly pink
  coastlines, and stretches the contrast.
*/
void
read_ssec_image (char *filename, int &width, int &height, 
		 unsigned char *&rgb_data)
{
  int image_width, image_height;
  unsigned char *temp = NULL, *temp2;
  int i, j, ii, jj, ipos;
  double lon, lat;
  int avg;
  int npoints;
  int avgwhole = 0;
  int npointswhole = 0;

  temp = read_png (filename, image_width, image_height);

  // This array will hold the final cloud image
  rgb_data = (unsigned char *) malloc (3 * image_width * image_height);
  memset (rgb_data, 0, 3 * image_width * image_height);

  // There's a 20 pixel border on the left & right and a 10 pixel border
  // on the top and bottom
  temp2 = temp;
  temp = crop_image (temp2, image_width, image_height, 10, 20, image_width - 20,
	      image_height - 40);
  free(temp2);
  
  image_height -= 40;
  image_width -= 20;

  // This converts the mollweide projection to rectangular
  ipos = 0;
  for (j = 0; j < image_height; j++)
    for (i = 0; i < image_width; i++)
      {
	rect_pixel_to_spher (image_width, image_height, i, j, lon, lat);
	moll_spher_to_pixel (image_width, image_height, lon, lat, ii, jj);
	memcpy (rgb_data + ipos, temp + 3 * (jj * image_width + ii), 3);
	ipos += 3;
      }

  // Replace any pink outlines by the average value in a 10x10 pixel square.
  for (j = 0; j < 31; j++)
    for (i = 0; i < 62; i++)
      {
	avg = 0;
	npoints = 0;
	for (jj = 0; jj < 10; jj++)
	  for (ii = 0; ii < 10; ii++)
	    {
	      ipos = 3*((10 * j + jj) * 620 + 10 * i + ii);
	      if (!(rgb_data[ipos] == 0xff && rgb_data[ipos+1] == 0
		    && rgb_data[ipos+2] == 0xff))
		{
		  npoints++;
		  avg += (int) rgb_data[ipos];
		  npointswhole++;
		  avgwhole += (int) rgb_data[ipos];
		}
	    }
	if (npoints != 0) avg = (int) (avg / (double) npoints);

	for (jj = 0; jj < 10; jj++)
	  for (ii = 0; ii < 10; ii++)
	    {
	      ipos = 3*((10 * j + jj) * 620 + 10 * i + ii);
	      if (rgb_data[ipos] == 0xff && rgb_data[ipos+1] == 0
		  && rgb_data[ipos+2] == 0xff) 
		memset (rgb_data + ipos, avg, 3);
	    }
      }

  // Fill in the poles
  if (npointswhole != 0) avgwhole = (int) (avgwhole / (double) npointswhole);
  for (i = 0; i < image_width * image_height; i++)
    {
      ipos = 3 * i;
      if (rgb_data[ipos] < 0x03) memset (rgb_data + ipos, avgwhole, 3);
    }

  // Smooth out the seam at 180 degrees Longitude
  double eastVal, westVal;
  int eastIndex, westIndex;
  for (i = 0; i < image_height - 1; i++)
    {
      eastIndex = 3 * (i * image_width + 1);
      westIndex = 3 * ((i + 1) * image_width - 2);
      eastVal = (double) rgb_data[eastIndex];
      westVal = (double) rgb_data[westIndex];
      memset (rgb_data + eastIndex - 3, 
	      (int) (eastVal + (westVal - eastVal)/3), 3);
      memset (rgb_data + westIndex + 3, 
	      (int) (westVal + (eastVal - westVal)/3), 3);
    }

  // Equalize histogram
  equalize_histogram (rgb_data, image_width, image_height);

  free(temp);

  if (width < 0 || height < 0)
    {
      width = image_width;
      height = image_height;
    }
  else
  {
    temp = rgb_data;
    rgb_data = clone_scaled_image (temp, width, height, image_width, image_height, 0);
    free(temp);
  }
}

