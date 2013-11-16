dnl Autoconf stuff to check for graphics libraries is adapted from 
dnl imagemagick's configure.in

AC_DEFUN(AC_FIND_GRAPHICS_LIBS,

[

GRAPHICS_LIBS=""

#
# Check for JPEG
#
have_jpeg='no'
if test "$with_jpeg" != 'no'; then
    have_jpeg_header='no'
    AC_CHECK_HEADER(jpeglib.h,have_jpeg_header='yes',have_jpeg_header='no')
    if test "$have_jpeg_header" != 'no'; then
	have_libjpeg='no'
        AC_CHECK_LIB(jpeg,jpeg_read_header,have_libjpeg='yes',have_libjpeg='no',)
	if test "$have_libjpeg" != 'no'; then
	    GRAPHICS_LIBS="$GRAPHICS_LIBS -ljpeg"
	    AC_DEFINE(HAVE_LIBJPEG,,Define if you have JPEG library)
            have_jpeg='yes'
  	fi
    fi
    if test "$have_jpeg" != 'yes'; then
        echo "error: must have LIBJPEG support installed!"
        exit 1
    fi
fi

#
# Check for PNG
#
have_png='no'
if test "$with_png" != 'no'; then
    have_png_header='no'
    AC_CHECK_HEADER(png.h,have_png_header='yes',have_png_header='no')
    AC_CHECK_HEADER(pngconf.h,have_png_header='yes',have_png_header='no')
    if test "$have_png_header" != 'no'; then
	have_libpng='no'
        AC_CHECK_LIB(png,png_info_init,have_libpng='yes',have_libpng='no',-lm)
	if test "$have_libpng" != 'no'; then
	    GRAPHICS_LIBS="$GRAPHICS_LIBS -lpng"
	    AC_DEFINE(HAVE_LIBPNG,,Define if you have PNG library)
            have_png='yes'
  	fi
    fi
    if test "$have_png" != 'yes'; then
        echo "error: must have PNG support installed!"
        exit 1
    fi
fi

AC_SUBST(GRAPHICS_LIBS)

])

