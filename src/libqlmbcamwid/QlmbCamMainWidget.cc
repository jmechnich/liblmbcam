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

#include <QApplication>
#include <QStatusBar>
#include <QMenuBar>

#include <string>
#include <sstream>
#include <vector>
#include <ctime>

#include "QlmbCamMainWidget.hh"

YUV411_RGBConverter QlmbCamMainWidget::YUV411Conv;
YUYV_RGBConverter   QlmbCamMainWidget::YUYVConv;
UYVY_RGBConverter   QlmbCamMainWidget::UYVYConv;
YVYU_RGBConverter   QlmbCamMainWidget::YVYUConv;
VYUY_RGBConverter   QlmbCamMainWidget::VYUYConv;
MONO_RGBConverter   QlmbCamMainWidget::MONOConv;
MONO16_RGBConverter QlmbCamMainWidget::MONO16Conv;
RGB_RGBConverter    QlmbCamMainWidget::RGBConv;
RGB16_RGBConverter  QlmbCamMainWidget::RGB16Conv;

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
QlmbCamMainWidget::QlmbCamMainWidget( LMBCam* camera, QWidget* parent)
        :QMainWindow( parent), 
         p_camera( camera), 
         p_currentFrame( -1),
         p_freeze( false), 
         p_dropFrames(true),
         p_showFPS( true),
         p_wasClosed( false),
         p_frames( 0), 
         p_start_time( clock()),
         p_data( 0)
{
  try
  {
    /*---------------------------------------------------------------------
     *  Initialize FID widget
     *---------------------------------------------------------------------*/
    p_fidWidget = new FastImageDisplayWid( this);    
    p_fidWidget->setImageSize( p_camera->width(),
                               p_camera->height());
    (void) menuBar();
    
    QMenu *file = new QMenu( "&File", this );
    file->addAction( "&Close",  this, SLOT( close()), Qt::CTRL+Qt::Key_C );
    menuBar()->addMenu( file );

    (void) statusBar();
    setCentralWidget( p_fidWidget);

    /*---------------------------------------------------------------------
     *  create control widget
     *---------------------------------------------------------------------*/
    p_controls = new QWidget( 0, Qt::WindowStaysOnTopHint);
    QHBoxLayout* controlLayout = new QHBoxLayout;
    p_modeControl =
        new QlmbCamModeControlWidget( camera, p_controls);
    controlLayout->addWidget( p_modeControl);
    connect( p_modeControl, SIGNAL( videoModeChanged( const QString&)),
             this, SLOT( changeVideoMode( const QString&)));
    connect( p_modeControl, SIGNAL( framerateChanged( double)),
             this, SLOT( changeFramerate( double)));
    connect( p_modeControl, SIGNAL( widthChanged( unsigned int)),
             this, SLOT( changeWidth( unsigned int)));
    connect( p_modeControl, SIGNAL( heightChanged( unsigned int)),
             this, SLOT( changeHeight( unsigned int)));
    
    QLabel* label = new QLabel( p_controls);
    label->setFrameStyle( QFrame::VLine | QFrame::Plain);
    controlLayout->addWidget( label);
    
    controlLayout->addWidget( new QlmbCamParamControlWidget( camera, p_controls));
    p_controls->setLayout( controlLayout);
    
    /*---------------------------------------------------------------------
     *  make menu to show controls
     *---------------------------------------------------------------------*/
    QMenu *edit = new QMenu( "&Edit", this );
    edit->addAction( "&Settings...",  p_controls, SLOT( show()), Qt::CTRL+Qt::Key_S );
    menuBar()->addMenu( edit );

    /*---------------------------------------------------------------------
     *  Start camera
     *---------------------------------------------------------------------*/
    p_camera->startCameraRunContinuous();
 }
  catch( LMBCriticalError& err)
  {
    std::cout << "Caught LMBCriticalError: "
              << err.str() << std::endl;
    exit( 1);
  }
  catch( LMBError& err)
  {
    std::cout << "Caught LMBError: "
              << err.str() << std::endl;
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
QlmbCamMainWidget::keyPressEvent( QKeyEvent* e)
{
  switch( e->key())
  {
  case Qt::Key_Q:
    close();
    break;
  case Qt::Key_0:
    p_freeze = !p_freeze;
    break;
  case Qt::Key_1:
    p_camera->setFramerate( 1.875, true);
    p_camera->restartCamera();
    p_currentFrame = -1;
    break;
  case Qt::Key_2:
    p_camera->setFramerate( 3.75, true);
    p_camera->restartCamera();
    p_currentFrame = -1;
    break;
  case Qt::Key_3:
    p_camera->setFramerate( 7.5, true);
    p_camera->restartCamera();
    p_currentFrame = -1;
    break;
  case Qt::Key_4:
    p_camera->setFramerate( 15, true);
    p_camera->restartCamera();
    p_currentFrame = -1;
    break;
  case Qt::Key_5: 
    p_camera->setFramerate( 30, true);
    p_camera->restartCamera();
    p_currentFrame = -1;
    break;
  case Qt::Key_6: 
    p_camera->setFramerate( 60, true);
    p_camera->restartCamera();
    p_currentFrame = -1;
    break;
  case Qt::Key_M:
  {
    std::cout << "AVAILABLE MODES:" << std::endl;
    std::cout << "================" << std::endl;
    std::vector<std::string> availableModes = p_camera->availableModes();
    for( std::vector<std::string>::iterator it = availableModes.begin();
         it != availableModes.end(); ++it)
    {
      std::cout << (*it) << std::endl;
    }
    std::cout << std::endl;
    break;
  }
  case Qt::Key_P:
  {
    std::cout << "AVAILABLE PARAMETERS:" << std::endl;
    std::cout << "=====================" << std::endl;
    std::vector<std::string> parameters = p_camera->availableParameters();
    for( std::vector<std::string>::iterator it
             = parameters.begin();
         it != parameters.end(); ++it)
    {
      std::cout << (*it) << " ("
                << "min: "   << p_camera->param(*it)->minValueAsInt() << " / "
                << "value: " << p_camera->param(*it)->asInt()    << " / "
                << "max: "   << p_camera->param(*it)->maxValueAsInt() << ")"
                << std::endl;
    }
    std::cout << std::endl;
    break;
  }
  case Qt::Key_I:
    std::cout << "GENERAL INFORMATION:" << std::endl;
    std::cout << "====================" << std::endl;
    std::cout << "Vendor: " << p_camera->vendor() << std::endl;
    std::cout << "Model : " << p_camera->model() << std::endl;
    std::cout << "GUID  : " << p_camera->guid() << std::endl;            
    std::cout << std::endl;
    break;
  case Qt::Key_Question:
    std::cout << "HELP:\n"
              << "=====\n"
              << "Key  Description\n\n"
              << "q    quit application\n"
              << "0    freeze picture\n"
              << "1    set framerate to 1.875 fps (except when"
              << " in Format7 mode)\n"
              << "2    set framerate to 3.75 fps  (except when"
              << " in Format7 mode)\n"
              << "3    set framerate to 7.5 fps   (except when"
              << " in Format7 mode)\n"
              << "4    set framerate to 15 fps    (except when"
              << " in Format7 mode)\n"
              << "5    set framerate to 30 fps    (except when"
              << " in Format7 mode)\n"
              << "6    set framerate to 60 fps    (except when"
              << " in Format7 mode)\n"
              << "m    print available camera modes\n"
              << "p    print available camera parameters\n"
              << "i    print general camera information\n"
              << "w    increase brightness by 10\n"
              << "s    decrease brightness by 10\n"
              << "x    switch auto mode for brightness\n"
              << "e    increase exposure by 10\n"
              << "d    decrease exposure by 10\n"
              << "c    switch auto mode for exposure\n"
              << "r    increase gain by 10\n"
              << "f    decrease gain by 10\n"
              << "v    switch auto mode for gain\n"
              << "t    increase shutter by 10\n"
              << "g    decrease shutter by 10\n"
              << "b    switch auto mode for shutter\n"
              << "z    increase white balance U by 10\n"
              << "h    decrease white balance U by 10\n"
              << "n    switch auto mode for white balance\n"
              << "u    increase white balance V by 10\n"
              << "j    decrease white balance V by 10\n\n";
    break;
  case Qt::Key_W:
    parameterIncrease( "brightness", 10);
    break;
  case Qt::Key_S:
    parameterDecrease( "brightness", 10);
    break;
  case Qt::Key_X:
    parameterSwitchAuto( "brightness_auto");
    break;
  case Qt::Key_E:
    parameterIncrease( "exposure", 10);
    break;
  case Qt::Key_D:
    parameterDecrease( "exposure", 10);
    break;
  case Qt::Key_C:
    parameterSwitchAuto( "exposure_auto");
    break;
  case Qt::Key_R:
    parameterIncrease( "gain", 10);
    break;
  case Qt::Key_F:
    parameterDecrease( "gain", 10);
    break;
  case Qt::Key_V:
    parameterSwitchAuto( "gain_auto");
    break;
  case Qt::Key_T:
    parameterIncrease( "shutter", 10);
    break;
  case Qt::Key_G:
    parameterDecrease( "shutter", 10);
    break;
  case Qt::Key_B:
    parameterSwitchAuto( "shutter_auto");
    break;
  case Qt::Key_Z:
    parameterIncrease( "white_balance_u", 1);
    break;
  case Qt::Key_H:
    parameterDecrease( "white_balance_u", 1);
    break;
  case Qt::Key_N:
    parameterSwitchAuto( "white_balance_auto");
    break;
  case Qt::Key_U:
    parameterIncrease( "white_balance_v", 1);
    break;
  case Qt::Key_J:
    parameterDecrease( "white_balance_v", 1);
    break;
  default:
    break;
  }
  
  e->accept();
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:QlmbCamMainWidget::updateImage()
 *  ==> see headerfile
 *=======================================================================*/
void
QlmbCamMainWidget::updateImage()
{
  try
  {
    if( !p_freeze)
    {
      long tmp =  nextFrame();
      if( tmp == p_currentFrame)
          return;
      
      p_currentFrame = tmp;
      
      if( p_camera->tryLockFrame( p_currentFrame))
      {
        p_data =
            p_camera->getFrame( p_currentFrame);
        
        if( p_data)
        {
          std::string colorcoding = p_camera->colorCoding();
          
          if( colorcoding == "YUV411")
          {
            p_fidWidget->setImagePerLine( p_data, YUV411Conv);
          }
          else if( colorcoding == "YUV422" ||
                   colorcoding == "UYVY")
          {
            p_fidWidget->setImagePerLine( p_data, UYVYConv);
          }
          else if( colorcoding == "VYUY")
          {
            p_fidWidget->setImagePerLine( p_data, VYUYConv);
          }
          else if( colorcoding == "YUYV")
          {
            p_fidWidget->setImagePerLine( p_data, YUYVConv);
          }
          else if( colorcoding == "YUYV")
          {
            p_fidWidget->setImagePerLine( p_data, YUYVConv);
          }
          else if( colorcoding == "YUV444")
          {
            p_fidWidget->setImagePerLine( p_data, YUV444Conv);
          }
          else if( colorcoding == "MONO")
          {
            p_fidWidget->setImagePerLine( p_data, MONOConv);
          }
          else if( colorcoding == "RGB")
          {
            p_fidWidget->setImagePerLine( p_data, RGBConv);
          }
          else if( colorcoding == "MONO16")
          {
            p_fidWidget->setImagePerLine( MONO16_RGBConverter::SRCTYPE( p_data),
                                          MONO16Conv);
          }
          else if( colorcoding == "RGB16")
          {
            p_fidWidget->setImagePerLine( RGB16_RGBConverter::SRCTYPE( p_data),
                                          RGB16Conv);
          }
          else
          {
            std::cout << "Colorcoding " << colorcoding << " not supported"
                      << std::endl;
          }
        }
        
        p_camera->unlockFrame();
      }
    }
    
    /*-------------------------------------------------------------------
     *  Display frames per second
     *-------------------------------------------------------------------*/
    if( p_showFPS)
    {
      ++p_frames;
      float elapsed_time = (float)(clock() - p_start_time) / CLOCKS_PER_SEC;
      if( elapsed_time > 1 )
      {
        std::stringstream msg;
        msg << "FPS: " << (float)p_frames / elapsed_time;
        statusBar()->showMessage( msg.str().c_str());
        p_frames = 0;
        p_start_time = clock();
      }
    }
  }
  catch( liblmbcam::LMBError& error)
  {
    std::cout << "QlmbCamMainWidget::updateImage() :" << error.str() << std::endl;
  }
}

