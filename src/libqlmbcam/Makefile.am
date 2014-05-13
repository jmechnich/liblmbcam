SUBDIRS = scripts

AM_CXXFLAGS = -Wall -g -O4 @QtGui_CFLAGS@

libqlmbcamincludedir = $(includedir)/libqlmbcam

AM_CPPFLAGS =-I$(top_srcdir)/src/liblmbcam $(LIBFIDWID_CFLAGS)

MOC_GEN_SOURCES = moc_QlmbCamBusWidget.cc \
		moc_QlmbCamVideoWidget.cc \
		moc_QlmbCamWidget.cc

lib_LTLIBRARIES = libqlmbcam.la

libqlmbcam_la_LDFLAGS = \
	-version-info @lt_major@:@lt_revision@:@lt_age@ \
	@QtGui_LIBS@

libqlmbcam_la_SOURCES = \
	Converters.cc \
	QlmbCamBusWidget.cc \
	QlmbCamUpdateThread.cc \
	QlmbCamVideoWidget.cc \
	QlmbCamWidget.cc \
	$(MOC_GEN_SOURCES)

libqlmbcaminclude_HEADERS = \
	Converters.hh \
	Equalizer.hh \
	QlmbCamBusWidget.hh \
	QlmbCamUpdateThread.hh \
	QlmbCamVideoWidget.hh \
	QlmbCamWidget.hh \
	MONO_overexp_RGBConverter.hh

#  Rules for moc
#
$(MOC_GEN_SOURCES): moc_%.cc: %.hh
	$(MOC) $< -o $@

MOSTLYCLEANFILES = $(MOC_GEN_SOURCES)
