/**************************************************************************
**       Title: 
**    $RCSfile: DisplayWidget.hh,v $
**   $Revision: 1.4 $$Name:  $
**       $Date: 2005/02/28 00:18:57 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: DisplayWidget.hh,v $
**  Revision 1.4  2005/02/28 00:18:57  mechnich
**  changed include path for libfidwid headers
**
**  Revision 1.3  2004/02/06 13:41:04  mechnich
**  changed include paths
**
**  Revision 1.2  2004/01/27 03:59:04  mechnich
**  corrected install path
**
**  Revision 1.1  2003/10/01 13:40:43  pigorsch
**  - initial revision
**
**
**
**
**************************************************************************/

#ifndef DISPLAYWIDGET_HH
#define DISPLAYWIDGET_HH

#include <qscrollview.h>

#include <unistd.h>

#include <LMBCam.hh>
#include <LMBCamParam.hh>

#include <FastImageDisplayWid.hh>
#include <OpenGLFidWidget.hh>
#include <PixelConverter.hh>

using namespace liblmbcam;

class DisplayWidget: public QScrollView
{
public:
  DisplayWidget(LMBCam* camera, QWidget* parent=0);
  ~DisplayWidget();
  
  bool newFrameAvailable() const
        {
          return (p_currentFrame!=p_camera->mostRecentFrameIndex());
        };
  
  long nextFrame();
  void updateImage();
  unsigned char* currentRawImage();
  void copyRawImage(unsigned char* dest);
  
  void setFrameDrop(bool dropFrames);

  void startCamera()
        {
          p_camera->startCameraRunContinuous();
        };
  
  void stopCamera()
        {
          p_camera->stopCamera();
        };


private:
  void cleanup();
  
  LMBCam* p_camera;
  FastImageDisplayWid* p_fidWidget;
  
  long p_currentFrame;
  bool p_freeze;
  bool p_dropFrames;
  bool p_wasClosed;
  
  int p_frames;
  clock_t p_start_time;

  unsigned char* p_data;
  
  static YUV411_RGBConverter YUV411Conv;
  static YUV422_RGBConverter YUV422Conv;
  static YUV444_RGBConverter YUV444Conv;
  static MONO_RGBConverter MONOConv;
  static MONO16_RGBConverter MONO16Conv;
  static RGB_RGBConverter RGBConv;
  static RGB16_RGBConverter RGB16Conv;
  
};

#include "DisplayWidget.icc"

#endif
