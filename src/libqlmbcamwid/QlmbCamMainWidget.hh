/**************************************************************************
**       Title: 
**    $RCSfile: QlmbCamMainWidget.hh,v $
**   $Revision: 1.11 $$Name:  $
**       $Date: 2005/02/28 00:12:09 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: QlmbCamMainWidget.hh,v $
**  Revision 1.11  2005/02/28 00:12:09  mechnich
**  changed include path for libfidwid headers
**
**  Revision 1.10  2005/02/25 19:18:32  mechnich
**  don't use namespace liblmbcam
**
**  Revision 1.9  2004/02/06 13:41:04  mechnich
**  changed include paths
**
**  Revision 1.8  2004/01/27 21:51:30  mechnich
**  use PixelConverter from libfidwid
**
**  Revision 1.7  2004/01/27 03:59:04  mechnich
**  corrected install path
**
**  Revision 1.6  2003/08/05 15:35:54  mechnich
**  added lineedits for Format7 image position and changed layout
**
**  Revision 1.5  2003/05/22 16:28:28  ronneber
**  - added Menu entry for control widget
**
**  Revision 1.4  2003/05/22 08:30:30  mechnich
**  - added slots for changeing video moder and framerate
**
**  Revision 1.3  2003/05/21 14:54:49  mechnich
**  ssetting framerates allows rounding now
**
**  Revision 1.2  2003/05/21 08:59:31  mechnich
**  added includes
**
**  Revision 1.1  2003/05/12 17:19:17  mechnich
**  initial revision
**
**
**
**************************************************************************/

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
  static YUV422_RGBConverter YUV422Conv;
  static YUV444_RGBConverter YUV444Conv;
  static MONO_RGBConverter MONOConv;
  static MONO16_RGBConverter MONO16Conv;
  static RGB_RGBConverter RGBConv;
  static RGB16_RGBConverter RGB16Conv;
  
};

#include "QlmbCamMainWidget.icc"
#endif
