SUBDIRS = etc scripts

AM_CXXFLAGS = -Wall -g -O2 -D_REENTRANT -DLIBLMBCAMDEBUG @libraw1394_CFLAGS@ @libraw1394_CFLAGS@ 

lib_LTLIBRARIES = liblmbcam.la

liblmbcam_la_LDFLAGS = 	\
	-version-info @lt_major@:@lt_revision@:@lt_age@ \
	@libdc1394_LIBS@ @libraw1394_LIBS@ -lpthread

liblmbcam_la_SOURCES =	FileCam.cc \
			FireCam.cc \
			FireCamBusRegistry.cc \
			FireCamBus.cc \
			FireCamParam.cc \
			LMBCam.cc \
			LMBCamBusIndex.cc \
			LMBCamFileIO.cc \
			LMBErrorHandler.cc
#			V4LCam.cc \
#			V4LCamBus.cc \
#			V4LCamParam.cc

pkginclude_HEADERS =	AtomicCounter.hh \
			AtomicCounter.icc \
			FileCam.hh \
			FileCam.icc \
			FileCamError.hh \
			FireCam.hh \
			FireCam.icc \
			FireCamBusRegistry.hh \
			FireCamBus.hh \
			FireCamBus.icc \
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
			MutexLocker.hh
#			V4LCam.hh \
#			V4LCam.icc \
#			V4LCamBus.hh \
#			V4LCamBus.icc \
#			V4LCamError.hh \
#			V4LCamParam.hh \
#			V4LCamParam.icc
