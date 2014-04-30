AM_CXXFLAGS = -Wall -g 

LIBLMBCAM_CFLAGS = -I$(top_srcdir)/src/liblmbcam
LIBQLMBCAM_CFLAGS = -I$(top_srcdir)/src/libqlmbcam
LIBLMBCAM_LIBS = -L$(top_builddir)/src/liblmbcam -llmbcam
LIBQLMBCAM_LIBS = -L$(top_builddir)/src/libqlmbcam -lqlmbcam

bin_PROGRAMS = qlmbcam

qlmbcam_CXXFLAGS = \
	$(LIBLMBCAM_CFLAGS) \
	$(LIBQLMBCAM_CFLAGS) \
	$(LIBFIDWID_CFLAGS) \
	$(LIBCMDLINE_CFLAGS) \
	@QtGui_CFLAGS@

qlmbcam_DEPENDENCIES = $(top_builddir)/src/libqlmbcam/libqlmbcam.la

MOC_GEN_SOURCES = moc_QlmbCamMainWindow.cc

noinst_HEADERS = QlmbCamMainWindow.hh \
	QlmbCamSaveThread.hh

qlmbcam_SOURCES = qlmbcam.cc \
		QlmbCamMainWindow.cc \
	QlmbCamSaveThread.cc \
		$(MOC_GEN_SOURCES)

qlmbcam_LDFLAGS = $(LIBQLMBCAM_LIBS) \
	$(LIBLMBCAM_LIBS) \
	$(LIBFIDWID_LIBS) \
	$(LIBLMBCAM_LIBS) \
	$(LIBCMDLINE_LIBS) \
	@QtGui_LIBS@

#  Rules for moc
#
$(MOC_GEN_SOURCES): moc_%.cc: %.hh
	$(MOC) $< -o $@

MOSTLYCLEANFILES = $(MOC_GEN_SOURCES)
