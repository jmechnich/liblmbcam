MEX = mex
MEXFLAGS = -g $(LIBFIDWID_CFLAGS) @libdc1394_CFLAGS@ @libraw1394_CFLAGS@ @qt3_CFLAGS@
SUFFIXES = .c .mexglx
CXXFLAGS = -Wall -g $(LIBFIDWID_CFLAGS) @libdc1394_CFLAGS@ @libraw1394_CFLAGS@ @qt3_CFLAGS@
AM_CPPFLAGS = \
	-I$(top_srcdir)/src/liblmbcam \
	-I$(top_srcdir)/src/libqlmbcamwid

LDLIBS = $(top_builddir)/src/libqlmbcamwid/libqlmbcamwid.a \
	$(top_builddir)/src/liblmbcam/.libs/liblmbcam.a \
	$(LIBFIDWID_LIBS) @libdc1394_LIBS@ @libraw1394_LIBS@ @qt3_LIBS@


MATLABSOURCES = \
      FireCamBus_cameraByGUID.m			\
      FireCamBus_cameraByIndex.m		\
      FireCamBus_delete.m			\
      FireCamBus_nCameras.m			\
      FireCamBus_new.m				\
      FireCamBus_rescan.m			\
      LMBCam_availableModes.m			\
      LMBCam_availableParameters.m		\
      LMBCam_framerate.m			\
      LMBCam_getFrame.m				\
      LMBCam_mode.m				\
      LMBCam_mostRecentFrameIndex.m		\
      LMBCam_paramSetValue.m			\
      LMBCam_printActualParameters.m		\
      LMBCam_reset.m				\
      LMBCam_setFramerate.m			\
      LMBCam_setMode.m				\
      LMBCam_setRingBufferSize.m		\
      LMBCam_showLiveWindow.m			\
      LMBCam_startCameraRunContinuous.m		\
      LMBCam_startCameraRunNFrames.m		\
      LMBCam_stopCamera.m			\
      LMBCam_waitForFrameFinished.m


matlabdir = $(libdir)/lmbcam/matlab
matlab_DATA = lmbcam_matlab.mexglx 		\
      test_qt.mexglx                            \
      $(MATLABSOURCES)

CPPHEADERS = InputArgs.hh \
	FireCamBusML.hh \
	LMBCamML.hh \
	OutputArgs.hh

CPPSOURCES = InputArgs.cc \
	lmbcam_matlab.cc \
	test_qt.cc

EXTRA_DIST = $(MATLABSOURCES) $(CPPHEADERS) $(CPPSOURCES)

all: lmbcam_matlab.mexglx test_qt.mexglx

lmbcam_matlab.mexglx: lmbcam_matlab.cc InputArgs.o FireCamBusML.hh InputArgs.hh LMBCamML.hh OutputArgs.hh
	$(MEX) $(MEXFLAGS) $(INCLUDES) $(LDFLAGS) lmbcam_matlab.cc InputArgs.o $(LDLIBS)

InputArgs.o: InputArgs.cc
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c InputArgs.cc

test_qt.mexglx: test_qt.cc
	$(MEX) $(MEXFLAGS) $(INCLUDES) $(LDFLAGS) test_qt.cc $(LDLIBS)


CLEANFILES = lmbcam_matlab.mexglx InputArgs.o test_qt.mexglx
