AC_INIT([liblmbcam],[0.1.0])
AC_CONFIG_SRCDIR([src/liblmbcam/LMBCam.hh])
AM_INIT_AUTOMAKE

AC_CONFIG_MACRO_DIR([m4])

AC_ARG_WITH([qt],
	    AC_HELP_STRING([--with-qt],
	                   [build Qt-based gui programs (default=yes)]),
            [], [with_qt=yes])

AC_ARG_ENABLE([examples],
	      AC_HELP_STRING([--enable-examples],
			     [compile examples (default=yes)]),
	      [], [enable_examples=yes])

AC_ARG_ENABLE([matlab],
	      AC_HELP_STRING([--enable-matlab],
			     [compile matlab interface (default=no)]),
	      [], [enable_matlab=no])

AM_CONDITIONAL([BUILD_QTLIBS], [test "$with_qt" = yes])
AM_CONDITIONAL([BUILD_EXAMPLES], [test "$enable_examples" = yes])
AM_CONDITIONAL([BUILD_LMBCAM_MATLAB], [test "$enable_matlab" = yes])

AC_PROG_CC
AC_PROG_CXX
AC_LANG(C++)
AC_PROG_LIBTOOL
AM_PROG_LIBTOOL
AC_PROG_INSTALL

PKG_PROG_PKG_CONFIG([0.17])

if test $with_qt = "yes"; then
        PKG_CHECK_MODULES([QtGui], [QtGui >= 4.8.4])
        AC_CHECK_PROG(MOC, [moc], [moc])
else
	enable_examples=no
fi

if test $enable_matlab = "yes"; then
	AC_CHECK_HEADER( mex.h, [], AC_MSG_ERROR( matlab headers are missing))
fi

AC_CHECK_HEADER([jpeglib.h], [], AC_MSG_ERROR( libjpeg headers are missing))
AC_CHECK_LIB([jpeg],[jpeg_start_decompress], [], AC_MSG_ERROR( libjpeg is missing))

PKG_CHECK_MODULES([libv4l2], [libv4l2 >= 1.0])
PKG_CHECK_MODULES([libudev], [libudev >= 200])

# Check for libraw1394
PKG_CHECK_MODULES([libraw1394], [libraw1394])

# Check for libdc1394
PKG_CHECK_MODULES([libdc1394], [libdc1394], [], [
    PKG_CHECK_MODULES([libdc1394], [libdc1394-2], [], [AC_CHECK_LIBDC])])

AM_CONDITIONAL([HAVE_LIBDC1394], [test x"$libdc1394_LIBS" != x])
AM_CONDITIONAL([HAVE_LIBV4L2], [test x"$libv4l2_LIBS" != x])

# Check for libtif
AC_CHECK_HEADER( tiffio.h, [], AC_MSG_ERROR( libtiff headers are missing))
AC_CHECK_LIB( tiff, TIFFOpen, [], AC_MSG_ERROR( libtiff is missing))

# set the libtool shared library version numbers
lt_major=1
lt_revision=0
lt_age=0

AC_SUBST(lt_major)
AC_SUBST(lt_revision)
AC_SUBST(lt_age)

AC_CHECK_PROG(LIBCMDLINE_BIN,libcmdline-config,libcmdline-config)
if test -z "$LIBCMDLINE_BIN" ; then
   	  AC_MSG_ERROR(You need libcmdline-config. Perhaps check PATH)
fi	

AC_CHECK_PROG(LIBFIDWID_BIN,libfidwid-config,libfidwid-config)
if test -z "$LIBFIDWID_BIN" ; then
   	  AC_MSG_ERROR(You need libfidwid-config. Perhaps check PATH)
fi	

LIBCMDLINE_CFLAGS=`libcmdline-config --cflags`
LIBCMDLINE_LIBS=`libcmdline-config --libs`
LIBFIDWID_CFLAGS=`libfidwid-config --cflags`
LIBFIDWID_LIBS=`libfidwid-config --libs`

AC_SUBST(LIBCMDLINE_CFLAGS)
AC_SUBST(LIBCMDLINE_LIBS)
AC_SUBST(LIBFIDWID_CFLAGS)
AC_SUBST(LIBFIDWID_LIBS)

AC_SUBST(HAVE_LIBDC1394)

AC_CONFIG_FILES([Makefile])
AC_CONFIG_FILES([src/Makefile])
AC_CONFIG_FILES([src/liblmbcam/Makefile])
AC_CONFIG_FILES([src/liblmbcam/etc/Makefile])
AC_CONFIG_FILES([src/liblmbcam/scripts/Makefile])

AC_CONFIG_FILES([src/libqlmbcamwid/Makefile])
AC_CONFIG_FILES([src/libqlmbcam/Makefile])
AC_CONFIG_FILES([src/libqlmbcam/scripts/Makefile])
AC_CONFIG_FILES([src/lmbcam_matlab/Makefile])
AC_CONFIG_FILES([src/examples/Makefile])
AC_CONFIG_FILES([src/examples/multiview/Makefile])
AC_CONFIG_FILES([src/examples/qlmbcam/Makefile])

AC_OUTPUT

AC_MSG_RESULT([
-------------------------------------------------
Summary of liblmbcam build options:

build Qt dependent libraries          $with_qt
build examples                        $enable_examples
build matlab interface                $enable_matlab
-------------------------------------------------])
