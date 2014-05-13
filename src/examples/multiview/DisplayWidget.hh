// This file is part of liblmbcam.
//
// liblmbcam is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// liblmbcam is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with liblmbcam.  If not, see <http://www.gnu.org/licenses/>.

#ifndef DISPLAYWIDGET_HH
#define DISPLAYWIDGET_HH

#include <OpenGLFidWidget.hh>
#include <FastImageDisplayWid.hh>
#include <PixelConverter.hh>

#include <QScrollArea>

#include <LMBCam.hh>
#include <LMBCamParam.hh>

#include <unistd.h>

using namespace liblmbcam;

class DisplayWidget: public QScrollArea
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
  static UYVY_RGBConverter   UYVYConv;
  static YUYV_RGBConverter   YUYVConv;
  static VYUY_RGBConverter   VYUYConv;
  static YVYU_RGBConverter   YVYUConv;
  static YUV444_RGBConverter YUV444Conv;
  static MONO_RGBConverter   MONOConv;
  static MONO16_RGBConverter MONO16Conv;
  static RGB_RGBConverter    RGBConv;
  static RGB16_RGBConverter  RGB16Conv;
  
};

#include "DisplayWidget.icc"

#endif
