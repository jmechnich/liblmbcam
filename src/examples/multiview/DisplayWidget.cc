/**************************************************************************
**       Title: 
**    $RCSfile: DisplayWidget.cc,v $
**   $Revision: 1.4 $$Name:  $
**       $Date: 2005/02/28 00:21:07 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: DisplayWidget.cc,v $
**  Revision 1.4  2005/02/28 00:21:07  mechnich
**  adapted to new libfidwid interface
**
**  Revision 1.3  2004/10/19 05:49:11  mechnich
**  changed LMBError interface
**
**  Revision 1.2  2004/01/27 03:59:04  mechnich
**  corrected install path
**
**  Revision 1.1  2003/10/01 13:41:43  pigorsch
**  - initial revision
**
**
**
**
**************************************************************************/

#include <qapplication.h>
#include <qcursor.h>

#include "DisplayWidget.hh"

#include <string>
#include <sstream>
#include <vector>
#include <ctime>

YUV411_RGBConverter DisplayWidget::YUV411Conv;
YUV422_RGBConverter DisplayWidget::YUV422Conv;
YUV444_RGBConverter DisplayWidget::YUV444Conv;
MONO_RGBConverter DisplayWidget::MONOConv;
MONO16_RGBConverter DisplayWidget::MONO16Conv;
RGB_RGBConverter DisplayWidget::RGBConv;
RGB16_RGBConverter DisplayWidget::RGB16Conv;

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
DisplayWidget::
DisplayWidget(LMBCam* camera, QWidget* parent)
        :QScrollView(parent, "displaywidget", Qt::WRepaintNoErase), 
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
    p_fidWidget = new FastImageDisplayWid( viewport(), "fidwid", Qt::WRepaintNoErase | Qt::WResizeNoErase);
    p_fidWidget->setImageSize( p_camera->width(),
                               p_camera->height());

//    setResizePolicy(QScrollView::AutoOne);
    
    addChild(p_fidWidget);
    
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
        else if( colorcoding == "YUV422")
        {
          p_fidWidget->setImagePerLine( p_data,
                                        YUV422Conv);
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
