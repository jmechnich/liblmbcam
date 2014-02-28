SUBDIRS = scripts

AM_CXXFLAGS = -Wall -g -O4 @qt3_CFLAGS@

libqlmbcamincludedir = $(includedir)/libqlmbcam

INCLUDES =-I$(top_srcdir)/src/liblmbcam $(LIBFIDWID_CFLAGS)
MOC      = @qt3_MOC@

MOC_GEN_SOURCES = moc_QlmbCamBusWidget.cc \
		moc_QlmbCamVideoWidget.cc \
		moc_QlmbCamWidget.cc

lib_LTLIBRARIES = libqlmbcam.la

libqlmbcam_la_LDFLAGS = \
	-version-info @lt_major@:@lt_revision@:@lt_age@ \
	@qt3_LIBS@

libqlmbcam_la_SOURCES = \
	Bayer.cc \
	QlmbCamBusWidget.cc \
	QlmbCamUpdateThread.cc \
	QlmbCamVideoWidget.cc \
	QlmbCamWidget.cc \
	$(MOC_GEN_SOURCES)

libqlmbcaminclude_HEADERS = \
	Bayer.hh \
	Equalizer.hh \
	QlmbCamBusWidget.hh \
	QlmbCamUpdateThread.hh \
	QlmbCamVideoWidget.hh \
	QlmbCamWidget.hh \
	MONO_overexp_RGBConverter.hh

#  Rules for moc
#
moc_%.cc: %.hh
	$(MOC) $< -o $@

MOSTLYCLEANFILES = $(MOC_GEN_SOURCES)
