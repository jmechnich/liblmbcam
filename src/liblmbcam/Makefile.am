SUBDIRS = etc scripts

AM_CPPFLAGS = -Wall -g -O2 -D_REENTRANT @libraw1394_CFLAGS@ @libraw1394_CFLAGS@  @libv4l2_CFLAGS@ @libudev_CFLAGS@ -DLIBLMBCAMDEBUG

lib_LTLIBRARIES = liblmbcam.la

liblmbcam_la_LDFLAGS = 	\
	-version-info @lt_major@:@lt_revision@:@lt_age@ \
	@libdc1394_LIBS@ @libraw1394_LIBS@ @libv4l2_LIBS@ @libudev_LIBS@ -lpthread

if HAVE_LIBDC1394
#AM_CPPFLAGS += -DHAVE_LIBDC1394
#FIRESOURCES = \
#	FireCam.cc \
#	FireCamBusRegistry.cc \
#	FireCamBus.cc \
#	FireCamParam.cc
endif

if HAVE_LIBV4L2
AM_CPPFLAGS += -DHAVE_V4L2
V4L2SOURCES = \
	V4LCamBus.cc \
	V4LCam.cc \
	V4LCamParam.cc
endif

liblmbcam_la_SOURCES =	\
	FileCam.cc \
	LMBCam.cc \
	LMBCamBusIndex.cc \
	LMBCamFileIO.cc \
	LMBErrorHandler.cc \
	$(V4L2SOURCES) \
	$(FIRESOURCES)

pkginclude_HEADERS =	AtomicCounter.hh \
			AtomicCounter.icc \
			FileCam.hh \
			FileCam.icc \
			FileCamError.hh \
			FireCam.hh \
			FireCam.icc \
			FireCamBusRegistry.hh \
			FireCamBus.hh \
			FireCamError.hh \
			FireCamParam.hh \
			FireCamParam.icc \
			LMBCam.hh \
			LMBCam.icc \
			LMBCamBus.hh \
			LMBCamBusIndex.hh \
			LMBCamError.hh \
			LMBCamFileIO.hh \
			LMBCamParam.hh \
			LMBError.hh \
			LMBErrorHandler.hh \
			MutexLocker.hh \
			V4LCam.hh \
			V4LCam.icc \
			V4LCamBus.hh \
			V4LCamError.hh \
			V4LCamParam.hh
