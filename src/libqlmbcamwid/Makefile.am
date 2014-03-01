AM_CXXFLAGS = -Wall -g -O4 @QtGui_CFLAGS@

libqlmbcamwidincludedir = $(includedir)/libqlmbcamwid

AM_CPPFLAGS = -I$(top_srcdir)/src/liblmbcam $(LIBFIDWID_CFLAGS)

MOC_GEN_SOURCES = moc_QlmbCamMainWidget.cc \
		moc_QlmbCamParamControl.cc \
		moc_QlmbCamModeControl.cc

lib_LIBRARIES = libqlmbcamwid.a

libqlmbcamwid_a_SOURCES = QlmbCamMainWidget.cc \
			QlmbCamModeControl.cc \
			QlmbCamParamControl.cc \
			$(MOC_GEN_SOURCES)

libqlmbcamwidinclude_HEADERS = QlmbCamMainWidget.hh \
       QlmbCamMainWidget.icc \
       QlmbCamModeControl.hh \
       QlmbCamModeControl.icc \
       QlmbCamParamControl.hh \
       QlmbCamParamControl.icc

#  Rules for moc
#
moc_%.cc: %.hh
	$(MOC) $< -o $@

MOSTLYCLEANFILES = $(MOC_GEN_SOURCES)
