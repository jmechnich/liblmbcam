AC_DEFUN([AC_CHECK_LIBDC],[
	AC_MSG_CHECKING(for libdc1394)
 	AC_TRY_LINK([
	#include <libdc1394/dc1394_control.h>
	#include <libraw1394/raw1394.h>
 	],[
        raw1394handle_t handle;
        dc1394_camerainfo info;
	dc1394_cameracapture capture;
 	],[
 	libdc1394=yes
 	],[
 	libdc1394=no
 	])
 	AC_MSG_RESULT($libdc1394)
 	if test x$libdc1394 = xno; then
          AC_ERROR(**      libdc1394 is not installed.
**      Source tarballs and CVS available on the SourceForge website:
**      http://www.sf.net/projects/libdc1394)
 	fi
])
