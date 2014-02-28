liblmbcam 0.1.0
---------------

Components:

liblmbcam  - C++ library mainly for controlling firewire digital cameras.
             Uses libdc1394 (http://sourceforge.net/projects/libdc1394/)

libqlmbcam - static GUI library providing widgets for liblmbcam 
             applications. Uses Trolltech's Qt (http://www.trolltech.no)

qlmbcam    - a simple GUI application for grabbing images from your 
             camera. At the moment you can only save gray images to a 
             pgm file.

lmbcam_matlab - Matlab interface to liblmbcam

Known Issues:

There seem to be problems running libdc1394 applications on smp machines 
in combination with nVidia's closed source video driver, especially on 
Linux 2.6 which leads to system lockups.
