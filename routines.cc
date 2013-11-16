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

#include <stdio.h>
#include <fstream>
#include <iostream>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// check for C++ and define libjpeg
// as external C functions or the bloody thing
// will show missing symbols while linking

#ifdef __cplusplus
extern "C" {
#endif
#include <jpeglib.h>
#include <jerror.h>
#ifdef __cplusplus
}
#endif

#ifdef __cplusplus
extern "C" {
#endif
#include <png.h>
#ifdef __cplusplus
}
#endif

#include "location.h"
#include "options.h"
#include "planet.h"
#include "util.h"
#include "conicplanet.h"

static inline int max( int a, int b ) { return a > b ? a : b; }
static inline int max( int a, int b, int c ) { return max( a, max( b, c ) ); }

static inline int min( int a, int b ) { return a < b ? a : b; }
static inline int min( int a, int b, int c ) { return min( a, min( b, c ) ); }

static inline double max( double a, double b ) { return a > b ? a : b; }
static inline double max( double a, double b, double c ) 
	{ return max( a, max( b, c ) ); }

static inline double min( double a, double b ) { return a < b ? a : b; }
static inline double min( double a, double b, double c ) 
	{ return min( a, min( b, c ) ); }

static int window_width, window_height;

double	gamma_table[101];

void 
init_display (int &w, int &h)
{
      window_width = opts.window_width;
      window_height = opts.window_height;

  // (x,y) of the globe's center
  if (opts.centx < 0 || opts.centy < 0)
    {
      opts.centx = window_width / 2;
      opts.centy = window_height / 2;
    }

  w = window_width;
  h = window_height;
}

unsigned char *crop_image (unsigned char *olddata, int old_width, int old_height, int x, int y,
	    int width, int height)
{
  int i, j;
  unsigned char *old_rgb = olddata;
  unsigned char *new_rgb = (unsigned char *) malloc (3 * width * height);

  int ipos = 0;
  for (j = 0; j < height; j++)
    for (i = 0; i < width; i++)
      {
	memcpy (new_rgb + ipos, 
		old_rgb + 3 * ((j + y) * old_width + (i + x)), 3);
	ipos += 3;
      }

  return new_rgb;
}

unsigned char *clone_scaled_image (unsigned char *olddata, int new_width, int new_height, int old_width, int old_height, int nearest_neighbor = 0)
{
  int i, j, ii, jj;
  double dx, dy;
  double t, u;
  int in_pos[4], out_pos;
  int sum;
  double weight[4];
  int ix[4], iy[4];

  double frac_h = ((double) old_height - 1) / (new_height - 1);
  double frac_w = ((double) old_width - 1) / (new_width - 1);

  unsigned char *old_rgb = olddata;
  unsigned char *new_rgb = (unsigned char *) malloc (3 * new_width * new_height);

  if (nearest_neighbor)
    {
      out_pos = 0;
      for (j = 0; j < new_height; j++)
	{
	  dy = j * frac_h;
	  iy[0] = (int) dy;
	  for (i = 0; i < new_width; i++)
	    {
	      dx = i * frac_w;
	      ix[0] = (int) dx;
	      in_pos[0] = 3 * (iy[0] * old_width + ix[0]);

	      memcpy (new_rgb + out_pos, old_rgb + in_pos[0], 3);
	      out_pos += 3;
	    }
	}
    }
  else
    {
      for (j = 0; j < new_height; j++)
	{
	  dy = ((double) j / (new_height - 1)) * (old_height - 1);
	  
	  iy[0] = iy[1] = (int) dy;
	  iy[2] = iy[0] + 1;
	  if (iy[2] == old_height) iy[2]--;
	  iy[3] = iy[2];
	  
	  u = 1 - (dy - iy[0]);
	  for (i = 0; i < new_width; i++)
	    {
	      dx = ((double) i / (new_width - 1)) * (old_width - 1);
	      
	      ix[0] = ix[2] = (int) dx;
	      ix[1] = ix[0] + 1;
	      if (ix[1] == old_width) ix[1] = 0;
	      ix[3] = ix[1];

	      t = dx - ix[0];
	      if (t > old_width/2) 
		t -= old_width;
	      else if (t < -old_width/2) 
		t += old_width;
	      
	      /*
		Weights are from Numerical Recipes, 2nd Edition
		weight[0] = (1 - t) * u;
		weight[1] = t * u;
		weight[2] = (1-t) * (1-u);
		weight[3] = t * (1-u);
	      */
	      
	      weight[1] = t * u;
	      weight[0] = u - weight[1];
	      weight[2] = 1 - t - u + weight[1];
	      weight[3] = t - weight[1];
	  
	      for (jj = 0; jj < 4; jj++)
		in_pos[jj] = 3 * (iy[jj] * old_width + ix[jj]);

	      out_pos = 3 * (j * new_width + i);
	      for (ii = 0; ii < 3; ii++)
		{
		  sum = 0;
		  for (jj = 0; jj < 4; jj++)
		    sum += (int) (weight[jj] * old_rgb[in_pos[jj] + ii]);
		  new_rgb[out_pos + ii] = (unsigned char) (sum & 0xff);
		}
	    }
	}
    }

    return new_rgb;
}

int toHSV(double *red, double *green, double *blue)
{
	double r = *red;
	double g = *green;
	double b = *blue;

	double v = max(r, g, b);
	double x = min(r, g, b);

	if (v == x)
		return -1;

	r /= 255;
	g /= 255;
	b /= 255;

	v = max(r, g, b);
	x = min(r, g, b);

	double vmx = (v - x);
	double s = vmx / v;
	
	double r1 = (v - r) / vmx;
	double b1 = (v - b) / vmx;
	double g1 = (v - g) / vmx;

	if(r == v)
	{
		if (g == x) 
			r = 5.0 + b1;
		else 
			r = 1.0 - g1;
	}
	else if (g == v)
	{
		if (b == x) 
			r = r1 + 1.0;
		else
			r = 3.0 - b1;
	}
	else
	{
		if (r == x)
			r = 3.0 + g1;
		else
			r = 5.0 - r1;
	}

	r *= 60;
	g = s;
	b = v;
	
	*red = r;
	*green = g;
	*blue = b;
	
	return 0;
}

int toRGB(double *hue, double *sat, double *intensity)
{
	double h = *hue;
	double s = *sat;
	double v = *intensity;

	// convert from degrees to hexagon section
	double h1 = h / 60.0;

	// find the dominant colour
	int i = (int) h1;
	double f = h1 - (double) i;

	double a[7];
	a[ 1 ] = a[ 2 ] = v;
	a[ 3 ] = v * ( 1.0 - (s * f) );
	a[ 4 ] = a[ 5 ] = v * ( 1.0 - s );
	a[ 6 ] = v * ( 1.0 - ( s * (1.0 - f) ) );

	// map strengths to rgb
	i += ( i > 4 ) ? -4 : + 2;
	h = a[ i ] * 255.0;
	i += ( i > 4 ) ? -4 : + 2;
	v = a[ i ] * 255.0;
	i += ( i > 4 ) ? -4 : + 2;
	s = a[ i ] * 255.0;

	*hue = h;
	*sat = s;
	*intensity = v;

	return 0;
}

void do_greyscale(unsigned char *rgb_data, int width, int height)
{
	int	i, j, k, g;
	
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
		{
			k = ((i * width) + j) * 3;
			g = (int) (((rgb_data[k] * 11) + (rgb_data[k + 1] * 16) + (rgb_data[k + 2] * 5)) / 32);

			rgb_data[k] = g;
			rgb_data[k + 1] = g;
			rgb_data[k + 2] = g;
		}

}

void do_gammacorrect(unsigned char *rgb_data, int width, int height, double gamma)
{
	int	i, j, k;
	double	r, g, b;

	gamma = 1 / gamma;
	
	for (i = 0; i <= 100; i++)
		gamma_table[i] = pow((double) i / 100.0, gamma);
	
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
		{
			k = ((i * width) + j) * 3;
			
			r = (double) rgb_data[k];
			g = (double) rgb_data[k + 1];
			b = (double) rgb_data[k + 2];
			
			if (toHSV(&r, &g, &b) == -1)
				continue;
			
			b = gamma_table[(int) (b * 100)];
			
			toRGB(&r, &g, &b);
			
			rgb_data[k] = (int) r;
			rgb_data[k + 1] = (int) g;
			rgb_data[k + 2] = (int) b;
		}


}

void do_brighten(unsigned char *rgb_data, int width, int height, int brightness)
{
	int	i, j, k;
	double	delta, r, g, b;
	
	delta = brightness / 100.0;
	
	for (i = 0; i < height; i++)
		for (j = 0; j < width; j++)
		{
			k = ((i * width) + j) * 3;
			
			r = rgb_data[k];
			g = rgb_data[k + 1];
			b = rgb_data[k + 2];
			
			if (toHSV(&r, &g, &b) == -1)
				continue;
			
			b *= delta;
			
			if (b > 1)
				b = 1;
			else if (b < 0)
				b = 0;
			
			toRGB(&r, &g, &b);

			rgb_data[k] = (int) r;
			rgb_data[k + 1] = (int) g;
			rgb_data[k + 2] = (int) b;
		}

}
      
unsigned char *read_png(char *filename, int &width, int &height)
{
  png_structp png_ptr;
  png_infop info_ptr;
  png_bytepp row_pointers;
  unsigned char *ptr = NULL, *rgb_data = NULL;
  png_uint_32 w, h;
  int bit_depth, color_type, interlace_type;
  int i;
  FILE *infile;
  
  if ((infile = fopen(filename, "rb")) == NULL)
  {
    std::cout << "Unable to open " << filename << "\n";
  	exit(1);
  }

  png_ptr = png_create_read_struct (PNG_LIBPNG_VER_STRING, 
				    (png_voidp) NULL, 
				    (png_error_ptr) NULL, 
				    (png_error_ptr) NULL);
  if (!png_ptr) 
    {
      fclose (infile);
      return (0);
    }
  
  info_ptr = png_create_info_struct (png_ptr);
  if (!info_ptr)
    {
      png_destroy_read_struct (&png_ptr, (png_infopp) NULL, 
			       (png_infopp) NULL);
      fclose (infile);
      return (0);
    }
  
  if (setjmp (png_ptr->jmpbuf))
    {
      png_destroy_read_struct (&png_ptr, &info_ptr, (png_infopp) NULL);
      fclose (infile);
      return (0);
    }
  
  png_init_io (png_ptr, infile);
  png_read_info (png_ptr, info_ptr);

  png_get_IHDR (png_ptr, info_ptr, &w, &h, &bit_depth, &color_type,
		&interlace_type, (int *) NULL, (int *) NULL);

  width = (int) w;
  height = (int) h;

  /* Change a paletted/grayscale image to RGB */
  if (color_type == PNG_COLOR_TYPE_PALETTE && bit_depth <= 8) 
    png_set_expand (png_ptr);

  /* Change a grayscale image to RGB */
  if (color_type == PNG_COLOR_TYPE_GRAY 
      || color_type == PNG_COLOR_TYPE_GRAY_ALPHA)
    png_set_gray_to_rgb (png_ptr);

  /* Don't need the alpha channel */
  if (color_type & PNG_COLOR_MASK_ALPHA) png_set_strip_alpha (png_ptr);
 
  /* If the PNG file has 16 bits per channel, strip them down to 8 */
  if (bit_depth == 16) png_set_strip_16 (png_ptr);

  /* use 1 byte per pixel */
  png_set_packing (png_ptr);

  row_pointers = (png_bytep *) malloc (height * sizeof (png_bytep));

  for (i = 0; i < height; i++)
    row_pointers[i] = (unsigned char *) malloc (3 * width * sizeof (png_byte));

  png_read_image (png_ptr, row_pointers);
  
  rgb_data = (unsigned char *) malloc (3 * width * height);
  ptr = rgb_data;

  for (i = 0; i < height; i++)
    {
      memcpy (ptr, row_pointers[i], 3 * width);
      ptr += 3 * width;
    }

  png_destroy_read_struct (&png_ptr, &info_ptr, (png_infopp) NULL);

  for (i = 0; i < height; i++)
  	free (row_pointers[i]);
  free (row_pointers);

  fclose (infile);
  return rgb_data;
}

void write_png(FILE *outfile, int width, int height, unsigned char *rgb_data)
{
  png_structp png_ptr;
  png_infop info_ptr;
  png_bytep row_ptr;

  int i;

  png_ptr = png_create_write_struct (PNG_LIBPNG_VER_STRING, 
				     (png_voidp) NULL, 
				     (png_error_ptr) NULL, 
				     (png_error_ptr) NULL);

  if (!png_ptr)
  	return;
  
  info_ptr = png_create_info_struct (png_ptr);
  if (!info_ptr)
  {
      png_destroy_write_struct (&png_ptr, (png_infopp) NULL);
      return;
  }
  
  png_init_io (png_ptr, outfile);

  png_set_IHDR (png_ptr, info_ptr, width, height, 8, PNG_COLOR_TYPE_RGB,
		PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT,
		PNG_FILTER_TYPE_DEFAULT);

  png_write_info (png_ptr, info_ptr);

  for (i = 0; i < height; i++) 
    {
      row_ptr = rgb_data + 3 * i * width;
      png_write_rows (png_ptr, &row_ptr, 1);
    }

  png_write_end (png_ptr, info_ptr);
  png_destroy_write_struct (&png_ptr, &info_ptr);
}

void write_jpeg(FILE *outfile, int width, int height, unsigned char *rgb_data)
{
      struct jpeg_compress_struct cinfo;
      struct jpeg_error_mgr jerr;
      JSAMPROW            row_pointer[1];
      int                 row_stride;
      FILE		  *f;

          f = outfile;
	  cinfo.err = jpeg_std_error(&jerr);
	  jpeg_create_compress(&cinfo);
	  jpeg_stdio_dest(&cinfo, f);
	  cinfo.image_width = width;
	  cinfo.image_height = height;
	  cinfo.input_components = 3;
	  cinfo.in_color_space = JCS_RGB;
	  jpeg_set_defaults(&cinfo);
	  jpeg_set_quality(&cinfo, (100 * 99) >> 8, TRUE);
	  jpeg_start_compress(&cinfo, TRUE);
	  row_stride = cinfo.image_width * 3;
	  while (cinfo.next_scanline < cinfo.image_height)
	    {
	      row_pointer[0] = rgb_data + (cinfo.next_scanline * row_stride);
	      jpeg_write_scanlines(&cinfo, row_pointer, 1);
	    }
	  jpeg_finish_compress(&cinfo);
}

unsigned char *read_jpeg(char *filename, int &width, int &height)
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;
  unsigned char      *data, *line[16], *ptr;
  int                 x, y, i;
  FILE               *f;
  
  if ((f = fopen(filename, "rb")) == NULL)
  {
    std::cout << "Unable to open " << filename << "\n";
  	exit(1);
  }

  cinfo.err = jpeg_std_error(&jerr);

  jpeg_create_decompress(&cinfo);
  jpeg_stdio_src(&cinfo, f);
  jpeg_read_header(&cinfo, TRUE);
  cinfo.do_fancy_upsampling = FALSE;
  cinfo.do_block_smoothing = FALSE;
  jpeg_start_decompress(&cinfo);
  width = cinfo.output_width;
  height = cinfo.output_height;
  data = (unsigned char *) malloc(width * height * 3);
  if (!data)
  {
      jpeg_destroy_decompress(&cinfo);
      exit(1);
  }
  ptr = data;

  if (cinfo.rec_outbuf_height > 16)
  	exit(1);


  if (cinfo.output_components == 3)
    {
      for (y = 0; y < height; y += cinfo.rec_outbuf_height)
	{
	  for (i = 0; i < cinfo.rec_outbuf_height; i++)
	    {
	      line[i] = ptr;
	      ptr += width * 3;
	    }
	  jpeg_read_scanlines(&cinfo, line, cinfo.rec_outbuf_height);
	}
    }
  else if (cinfo.output_components == 1)
    {
      for (i = 0; i < cinfo.rec_outbuf_height; i++)
	{
	  if ((line[i] = (unsigned char *) malloc(width)) == NULL)
	    {
	      int                 t = 0;

	      for (t = 0; t < i; t++)
		free(line[t]);
	      jpeg_destroy_decompress(&cinfo);
	      	exit(1);
	    }
	}
      for (y = 0; y < height; y += cinfo.rec_outbuf_height)
	{
	  jpeg_read_scanlines(&cinfo, line, cinfo.rec_outbuf_height);
	  for (i = 0; i < cinfo.rec_outbuf_height; i++)
	    {
	      for (x = 0; x < width; x++)
		{
		  *ptr++ = line[i][x];
		  *ptr++ = line[i][x];
		  *ptr++ = line[i][x];
		}
	    }
	}
      for (i = 0; i < cinfo.rec_outbuf_height; i++)
	free(line[i]);
    }
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
  
  fclose(f);

  return data;
}

/*
  Save the RGB pixel array of the image file into rgb_data.  If width or 
  height are < 0, width and height are set to image file's dimensions.  
  Otherwise rgb_data is scaled to supplied width & height.
*/
void
read_image_file (char *filename, int &width, int &height, 
		 unsigned char *&rgb_data)
{
  int w, h;
  unsigned char *data, *olddata;
  

  data = read_jpeg(filename, w, h);
  
  if ((width < 0) || (height < 0))
    {
      width = w;
      height = h;
    }
  else
    {
      olddata = data;
      data = clone_scaled_image (olddata, width, height, w, h);
      free(olddata);
    }

  rgb_data = (unsigned char *) malloc(3 * width * height);
  memcpy (rgb_data, data, 3 * width * height); 
  free(data);

}

void 
render (Projection *image, const double sunlon, const double sunlat)
{
  unsigned char *olddata;

  int image_width = image->get_width();
  int image_height = image->get_height();

  if (opts.obslon && opts.projection == RECTANGULAR)
    {
      unsigned char *temp = (unsigned char *) (3 * image_width * image_height);
      memcpy (temp, image->rgb_data, 3 * image_width * image_height);
      int istart = (int) (image_width * (1 - opts.obslon / (2*M_PI)));
      if (istart >= image_width) istart -= image_width;
      for (int i = 0; i < image_height; i++)
	{
	  memcpy (image->rgb_data + 3 * i * image_width, 
		  temp + 3 * ((i+1) * image_width - istart), 
		  3*istart);
	  memcpy (image->rgb_data + 3 * (istart + i * image_width), 
		  temp + 3 * (i * image_width), 3*(image_width - istart));
	}
      free(temp);
    }
    
      // now smoothen the image for the "smoothen" option
      
      olddata = image->rgb_data;
      image->rgb_data = clone_scaled_image (olddata, (int) (image_width / opts.smoothen), (int) (image_height / opts.smoothen), image_width, image_height);
      free(olddata);

      image_width = image_width / opts.smoothen;
      image_height = image_height / opts.smoothen;
      
      // now convert the image to greyscale and do brighness and
      // gamma correction if necessary
      
      if (opts.greyscale == 1)
      	do_greyscale(image->rgb_data, image_width, image_height);
      
      if (opts.gamma != 1)
      	do_gammacorrect(image->rgb_data, image_width, image_height, opts.gamma);
      
      if (opts.brightness != 100)
        do_brighten(image->rgb_data, image_width, image_height, opts.brightness);

      // now, write the image info out to stdout as a
      // content type

      if (opts.contenttype == IMAGEJPEG)
      	write_jpeg(stdout, image_width, image_height, image->rgb_data);
      else if (opts.contenttype == IMAGEPNG)
        write_png(stdout, image_width, image_height, image->rgb_data);
      else         // default
      	write_jpeg(stdout, image_width, image_height, image->rgb_data);

  free(image->rgb_data);
  delete image;

} 
