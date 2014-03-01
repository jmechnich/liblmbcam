AM_CXXFLAGS = -Wall -g

LIBLMBCAM_CFLAGS = -I$(top_srcdir)/src/liblmbcam
LIBLMBCAM_LIBS = -L$(top_builddir)/src/liblmbcam -llmbcam

bin_PROGRAMS = multiview

noinst_HEADERS = DisplayWidget.hh \
	DisplayWidget.icc

multiview_CXXFLAGS = $(LIBCMDLINE_CFLAGS) $(LIBLMBCAM_CFLAGS) $(LIBFIDWID_CFLAGS)
multiview_SOURCES = multiview.cc DisplayWidget.cc
multiview_LDFLAGS = $(LIBCMDLINE_LIBS) $(LIBFIDWID_LIBS) $(LIBLMBCAM_LIBS)
