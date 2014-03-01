LIBLMBCAM_CFLAGS = -I$(top_srcdir)/src/liblmbcam
LIBLMBCAM_LIBS = -L$(top_builddir)/src/liblmbcam -llmbcam

bin_PROGRAMS = FilltimeBugCheck

AM_CXXFLAGS = -Wall  -g -O0 \
	$(LIBCMDLINE_CFLAGS) \
	$(LIBFIDWID_CFLAGS)  \
	$(LIBLMBCAM_CFLAGS)

AM_LDFLAGS = $(LIBCMDLINE_LIBS) \
	$(LIBFIDWID_LIBS) \
	$(LIBLMBCAM_LIBS)

FilltimeBugCheck_SOURCES = FilltimeBugCheck.cc
FilltimeBugCheck_INCLUDES = -I$(POLDEST)/ext/include/liblmbcam \
	 -I$(POLDEST)/ext/include


#test_dma_SOURCES = test_dma.cc 
#test_dma_CXXFLAGS = -I$(POLDEST)/ext/include/liblmbcam \
#	 -I$(POLDEST)/ext/include

#test_dma_LDADD = -lpolscan


#testsuite_camera_SOURCES = testsuite_camera.cc

SUBDIRS = multiview qlmbcam
