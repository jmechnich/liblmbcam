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
#include <QCursor>

#include "DisplayWidget.hh"

#include <string>
#include <sstream>
#include <vector>
#include <ctime>

YUV411_RGBConverter DisplayWidget::YUV411Conv;
YUYV_RGBConverter   DisplayWidget::YUYVConv;
UYVY_RGBConverter   DisplayWidget::UYVYConv;
YVYU_RGBConverter   DisplayWidget::YVYUConv;
VYUY_RGBConverter   DisplayWidget::VYUYConv;
YUV444_RGBConverter DisplayWidget::YUV444Conv;
MONO_RGBConverter   DisplayWidget::MONOConv;
MONO16_RGBConverter DisplayWidget::MONO16Conv;
RGB_RGBConverter    DisplayWidget::RGBConv;
RGB16_RGBConverter  DisplayWidget::RGB16Conv;

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
DisplayWidget::
DisplayWidget(LMBCam* camera, QWidget* parent)
        :QScrollArea(parent), 
         p_camera( camera), 
         p_currentFrame( -1),
         p_freeze( false), 
         p_dropFrames(true),
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
    p_fidWidget = new FastImageDisplayWid( viewport());
    p_fidWidget->setImageSize( p_camera->width(),
                               p_camera->height());

//    setResizePolicy(QScrollView::AutoOne);
    
    setWidget(p_fidWidget);
    
    /*---------------------------------------------------------------------
     *  Start camera
     *---------------------------------------------------------------------*/
    startCamera();
    
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

DisplayWidget::
~DisplayWidget()
{
  stopCamera();
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
DisplayWidget::updateImage()
{
  if( !p_freeze)
  {
    p_currentFrame = nextFrame();
    if( p_camera->tryLockFrame( p_currentFrame))
    {
      p_data =
          p_camera->getFrame( p_currentFrame);
          
      if( p_data)
      {
        std::string colorcoding = p_camera->colorCoding();
        
        if( colorcoding == "YUV411")
        {
          p_fidWidget->setImagePerLine( p_data,
                                        YUV411Conv);
        }
        else if( colorcoding == "YUV422" |
                 colorcoding == "UYVY")
        {
          p_fidWidget->setImagePerLine( p_data,
                                        UYVYConv);
        }
        else if( colorcoding == "VYUY")
        {
          p_fidWidget->setImagePerLine( p_data,
                                        VYUYConv);
        }
        else if( colorcoding == "YUYV")
        {
          p_fidWidget->setImagePerLine( p_data,
                                        YUYVConv);
        }
        else if( colorcoding == "YVYU")
        {
          p_fidWidget->setImagePerLine( p_data,
                                        YVYUConv);
        }
        else if( colorcoding == "YUV444")
        {
          p_fidWidget->setImagePerLine( p_data,
                                        YUV444Conv);
        }
        else if( colorcoding == "MONO")
        {
          p_fidWidget->setImagePerLine( p_data,
                                        MONOConv);
        }
        else if( colorcoding == "RGB")
        {
          p_fidWidget->setImagePerLine( p_data,
                                        RGBConv);
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

}
