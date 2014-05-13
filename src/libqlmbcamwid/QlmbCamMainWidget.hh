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

#ifndef QLMBCAMMAINWIDGET_HH
#define QLMBCAMMAINWIDGET_HH

#include <QMainWindow>
#include <QHBoxLayout>
#include <QLabel>
#include <QlmbCamParamControl.hh>
#include <QlmbCamModeControl.hh>
#include <QCloseEvent>

#include <LMBCam.hh>
#include <LMBCamParam.hh>

#include <FastImageDisplayWid.hh>
#include <PixelConverter.hh>

/*======================================================================*/
/*!
 *  \class (ClassName) (includeFile)
 *  \brief The (ClassName) class (is/provides/specifies/contains)
 *
 *  (description)
 */
/*======================================================================*/
class QlmbCamMainWidget : public QMainWindow
{
  Q_OBJECT

public:
  QlmbCamMainWidget( LMBCam* camera, QWidget* parent = 0);

  long nextFrame();
  void updateImage();
  unsigned char* currentRawImage();
  void copyRawImage( unsigned char* dest);
  
  void setFrameDrop( bool dropFrames);
  void showFPS( bool show);
  bool wasClosed() const;

public slots:
  void changeVideoMode( const QString& mode);
  void changeFramerate( double fps);
  void changeWidth( unsigned int width);
  void changeHeight( unsigned int height);

protected:
  void keyPressEvent( QKeyEvent* e);
  void closeEvent( QCloseEvent* e);

private:
  void cleanup();
  void parameterIncrease( std::string name, int amount);
  void parameterDecrease( std::string name, int amount);
  void parameterSwitchAuto( std::string name);
  
  liblmbcam::LMBCam* p_camera;
  FastImageDisplayWid* p_fidWidget;
  QWidget* p_controls;
  QlmbCamModeControlWidget* p_modeControl;
  

  long p_currentFrame;
  bool p_freeze;
  bool p_dropFrames;
  bool p_showFPS;
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

#include "QlmbCamMainWidget.icc"
#endif
