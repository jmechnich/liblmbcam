/**************************************************************************
**       Title: 
**    $RCSfile: QlmbCamVideoWidget.cc,v $
**   $Revision: 1.20 $$Name:  $
**       $Date: 2005/07/14 09:36:35 $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: QlmbCamVideoWidget.cc,v $
**  Revision 1.20  2005/07/14 09:36:35  tschmidt
**  - added SubSampling capabilities
**
**  Revision 1.19  2005/05/10 20:39:09  ckeller
**  added event for progress
**
**  Revision 1.18  2005/05/04 12:10:26  ckeller
**  resetting grabCurrentFrame in copyImageToBuffer
**
**  Revision 1.17  2005/05/03 14:23:42  ckeller
**  fixed mistake in "FS-mode"
**
**  Revision 1.16  2005/05/03 12:58:42  ckeller
**  Fixed problem with buffer
**
**  Revision 1.15  2005/04/27 17:48:41  mechnich
**  added functions for grabbing image series
**
**  Revision 1.14  2005/02/28 00:13:19  mechnich
**  added subdir scripts Makefile.am
**
**  Revision 1.13  2004/12/14 10:24:16  ronneber
**  - added "show Overexposure" to Context-menu
**
**  Revision 1.12  2004/10/19 05:55:38  mechnich
**  added DMA stuff without testing, will probably need future fixes; added absolute control features
**
**  Revision 1.11  2004/03/18 15:40:12  mechnich
**  *** empty log message ***
**
**  Revision 1.10  2003/10/23 14:50:33  mechnich
**   fixed error in Equalizer
**
**  Revision 1.9  2003/10/21 19:32:32  mechnich
**  improved image output
**
**  Revision 1.8  2003/10/21 17:34:26  mechnich
**  - improved handling of equalizer widget
**
**  Revision 1.7  2003/10/17 22:52:48  mechnich
**  - added geometric equalizer
**  - camera in bus widget is selected now when video widget gets focus
**  - fixed bug in update image
**
**  Revision 1.6  2003/10/08 19:30:45  mechnich
**  corrected handling of grabbed frame
**
**  Revision 1.5  2003/10/07 12:56:45  mechnich
**  replaced calls of QMessageBox::question() by QMessageBox::information()
**  for qt backward compatibility.
**
**  Revision 1.4  2003/10/07 05:21:53  mechnich
**  - completed bayer decoding integration
**  - introduced file saving function
**  - made the widget a QScrollView
**
**  Revision 1.3  2003/10/06 16:13:29  mechnich
**  corrected bayer decoding errors in updateImage()
**
**  Revision 1.2  2003/10/06 13:46:12  mechnich
**  added Bayer decoding
**
**  Revision 1.1  2003/10/05 19:27:20  mechnich
**  initial revision
**
**
**
**************************************************************************/

#include <QMessageBox>
#include <QPushButton>
#include <QHBoxLayout>
#include <QLCDNumber>
#include <QImage>
#include <QSignalMapper>
#include <QDesktopWidget>

#include <fstream>

#include "QlmbCamVideoWidget.hh"
#include "Equalizer.hh"

#include <LMBError.hh>

YUV411_RGBConverter libqlmbcam::QlmbCamVideoWidget::_YUV411Conv;
YUV422_RGBConverter libqlmbcam::QlmbCamVideoWidget::_YUV422Conv;
YUV444_RGBConverter libqlmbcam::QlmbCamVideoWidget::_YUV444Conv;
MONO_RGBConverter   libqlmbcam::QlmbCamVideoWidget::_MONOConv;
MONO_overexp_RGBConverter   libqlmbcam::QlmbCamVideoWidget::_MONORedConv;
MONO16_RGBConverter libqlmbcam::QlmbCamVideoWidget::_MONO16Conv;
RGB_RGBConverter    libqlmbcam::QlmbCamVideoWidget::_RGBConv;
RGB16_RGBConverter  libqlmbcam::QlmbCamVideoWidget::_RGB16Conv;

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
libqlmbcam::QlmbCamVideoWidget::QlmbCamVideoWidget( liblmbcam::LMBCam* camera,
                                                    QWidget* parent)
        :QScrollArea( parent),
         _camera( camera), _updateThread( this),
         _doUpdate( false), _freeze( false),
         _data( 0), _extGrabData( 0), _grabNFrames( 0), _grabCurrentFrame( 0),
         _frameSize( 0), _currentFrameIndex( -1),
         _bayerDecoding( NO_BAYER_DECODING), _pxlOrder( BAYER_PATTERN_RGGB),
         _lambda( 0), _equalizer( 0), _recv( 0), _showOverExposure( false),
         _fs(false)

{
  _fidWid = new FastImageDisplayWid( viewport());
  _fidWid->setImageSize( _camera->width(), _camera->height());
  _frameSize = (unsigned int) ( _camera->width() *
                                _camera->height()*
                                _camera->bytePerPixel());
  _data = (unsigned char*) malloc( _frameSize);
  
  setWidget( _fidWid);
  setFocusPolicy(Qt::StrongFocus);

  fitSize();
  _wsize=size();
  _wpos=pos();
  
  QSignalMapper* bayerMapper = new QSignalMapper(this);
  QAction* action;
  _bayerPopup = new QMenu( this);
  action = new QAction( "No Bayer", _bayerPopup);
  bayerMapper->setMapping( action, 0);
  _bayerPopup->addAction( action);
  connect( action, SIGNAL( triggered()), bayerMapper, SLOT( map()));
  action = new QAction( "Nearest", _bayerPopup);
  bayerMapper->setMapping( action, 1);
  _bayerPopup->addAction( action);
  connect( action, SIGNAL( triggered()), bayerMapper, SLOT( map()));
  action = new QAction( "Edge Sense", _bayerPopup);
  bayerMapper->setMapping( action, 2);
  _bayerPopup->addAction( action);
  connect( action, SIGNAL( triggered()), bayerMapper, SLOT( map()));
  action = new QAction( "Downsample", _bayerPopup);
  bayerMapper->setMapping( action, 3);
  _bayerPopup->addAction( action);
  connect( action, SIGNAL( triggered()), bayerMapper, SLOT( map()));
  _bayerPopup->addSeparator();
  action = new QAction( "BGGR", _bayerPopup);
  bayerMapper->setMapping( action, 5);
  _bayerPopup->addAction( action);
  connect( action, SIGNAL( triggered()), bayerMapper, SLOT( map()));
  action = new QAction( "GRBG", _bayerPopup);
  bayerMapper->setMapping( action, 6);
  _bayerPopup->addAction( action);
  connect( action, SIGNAL( triggered()), bayerMapper, SLOT( map()));
  action = new QAction( "RGGB", _bayerPopup);
  bayerMapper->setMapping( action, 7);
  _bayerPopup->addAction( action);
  connect( action, SIGNAL( triggered()), bayerMapper, SLOT( map()));
  action = new QAction( "GBRG", _bayerPopup);
  bayerMapper->setMapping( action, 8);
  _bayerPopup->addAction( action);
  connect( action, SIGNAL( triggered()), bayerMapper, SLOT( map()));
  if( _camera->colorCoding() == "MONO")
  {
    _bayerPopup->addSeparator();
    action = new QAction( "Equalizer", _bayerPopup);
    bayerMapper->setMapping( action, 10);
    _bayerPopup->addAction( action);
    connect( action, SIGNAL( triggered()), bayerMapper, SLOT( map()));
    _bayerPopup->addSeparator();
    action = new QAction( "Show Overexposure", _bayerPopup);
    bayerMapper->setMapping( action, 12);
    _bayerPopup->addAction( action);
    connect( action, SIGNAL( triggered()), bayerMapper, SLOT( map()));
    action = new QAction( "Hide Overexposure", _bayerPopup);
    bayerMapper->setMapping( action, 13);
    _bayerPopup->addAction( action);
    connect( action, SIGNAL( triggered()), bayerMapper, SLOT( map()));
  }
    
  connect( bayerMapper, SIGNAL( mapped( int)),
           this, SLOT( setBayerMode( int)));
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
libqlmbcam::QlmbCamVideoWidget::~QlmbCamVideoWidget()
{
  stopUpdate();
  if( _equalizer != 0)
  {
    _equalizer->close();
  }
  free( _data);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamVideoWidget::startUpdate()
{
  if( !_updateThread.isRunning())
  {
    _doUpdate = true;
    _updateThread.start();
  }
}


/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamVideoWidget::stopUpdate()
{
  if( _updateThread.isRunning())
  {
    _doUpdate = false;
    _updateThread.wait();
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamVideoWidget::freezeImage( bool freeze)
{
  _freeze = freeze;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamVideoWidget::saveCurrentImage( const QString& filename)
{
  std::string colorcoding = _camera->colorCoding();
  int width = _camera->width();
  int height = _camera->height();

/*-------------------------------------------------------------------------
 *  TEMPORARY CODE BEGIN
 *-------------------------------------------------------------------------*/
  QString filenameCopy( filename);
  if( colorcoding == "MONO")
  {
    if( !filename.endsWith( ".pgm"))
    {
      int dot = filename.lastIndexOf( ".");
      int slash = filename.lastIndexOf( "/");
      if( slash < dot)
      {
        filenameCopy = filenameCopy.left( dot);
      }
      filenameCopy.append( ".pgm");
      int answer = QMessageBox::information( this, "Rename file ...",
                                             "Filename does not end with "
                                             "'.pgm'.\n"
                                             "Change filename to " +
                                             filenameCopy + " ?",
                                             QMessageBox::Yes,
                                             QMessageBox::No);
      switch(answer)
      {
      case(QMessageBox::Yes):
        break;
      case(QMessageBox::No):
        filenameCopy = filename;
        break;
      default:
        break;
      }
    }
        
    std::ofstream file( filenameCopy.toLatin1().data());
    file << "P5" << std::endl
         << width << " " << height << " 255" << std::endl;
    
    for( unsigned int pos=0; pos < _frameSize; ++pos)
    {
      file << _data[pos];
    }
  }
  else
  {
    QMessageBox::information( this, "Save Current Image ...",
                              "Fileformat not supported (yet)!",
                              QMessageBox::Ok);
  }
  
  return; // return until real image output is written
/*-------------------------------------------------------------------------
 *  TEMPORARY CODE END
 *-------------------------------------------------------------------------*/
  
  QImage image( width, height, QImage::Format_ARGB32);
  QString format = filename.right( 3).toUpper();
   
  if( colorcoding == "YUV411")
  {
    for( int y=0; y < height; ++y)
    {
      _YUV411Conv.lineToRgba8( _data, image.scanLine(y), width);
    }
  }
  else if( colorcoding == "YUV422")
  {
    for( int y=0; y < height; ++y)
    {
      _YUV422Conv.lineToRgba8( _data, image.scanLine(y), width);
    }
  }
  else if( colorcoding == "YUV444")
  {
    for( int y=0; y < height; ++y)
    {
      _YUV444Conv.lineToRgba8( _data, image.scanLine(y), width);
    }
  }
  else if( colorcoding == "MONO")
  {
    for( int y=0; y < height; ++y)
    {
      _MONOConv.lineToRgba8( _data, image.scanLine(y), width);
    }
  }
  else if( colorcoding == "RGB")
  {
    for( int y=0; y < height; ++y)
    {
      _RGBConv.lineToRgba8( _data, image.scanLine(y), width);
    }
  }
  else if( colorcoding == "MONO16")
  {
    for( int y=0; y < height; ++y)
    {
      _MONO16Conv.lineToRgba8( MONO16_RGBConverter::SRCTYPE( _data),
                               image.scanLine(y), width);
    }
  }
  else if( colorcoding == "RGB16")
  {
    for( int y=0; y < height; ++y)
    {
      _RGB16Conv.lineToRgba8( MONO16_RGBConverter::SRCTYPE( _data),
                              image.scanLine(y), width);
    }
  }
  else
  {
    std::cout << "Colorcoding " << colorcoding << " not supported"
              << std::endl;
    return;
  }

  image.save( filename, format.toLatin1());
}


/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamVideoWidget::setBayerMode( int id)
{
  stopUpdate();

  switch( id)
  {
  case(0):
    _bayerDecoding = NO_BAYER_DECODING;
    _fidWid->setImageSize( _camera->width(), _camera->height());
    break;
  case(1):
    _bayerDecoding = BAYER_DECODING_NEAREST;
    _fidWid->setImageSize( _camera->width(), _camera->height());
    break;
  case(2):
    _bayerDecoding = BAYER_DECODING_EDGE_SENSE;
    _fidWid->setImageSize( _camera->width(), _camera->height());
    break;
  case(3):
    _bayerDecoding = BAYER_DECODING_DOWNSAMPLE;
   _fidWid->setImageSize( _camera->width()/2, _camera->height()/2);
    break;
  case(5):
    _pxlOrder = BAYER_PATTERN_BGGR;
    break;
  case(6):
    _pxlOrder = BAYER_PATTERN_GRBG;
    break;
  case(7):
    _pxlOrder = BAYER_PATTERN_RGGB;
    break;
  case(8):
    _pxlOrder = BAYER_PATTERN_GBRG;
    break;
  case(10):
  {
    if( _equalizer == 0)
    {
      _equalizer = new QWidget;
      _equalizer->setAttribute(Qt::WA_DeleteOnClose) ;
      _equalizer->setWindowTitle( "Equalizer");
      QHBoxLayout* eqLayout = new QHBoxLayout( _equalizer);
      QPushButton* lambdaReset = new QPushButton( "Reset", _equalizer);
      eqLayout->addWidget( lambdaReset);
      _lambdaSlider = new QSlider( Qt::Horizontal, _equalizer);
      _lambdaSlider->setMinimum( -1000);
      _lambdaSlider->setMaximum( 1000);
      _lambdaSlider->setPageStep( 10);
      _lambdaSlider->setValue( 0);
      eqLayout->addWidget( _lambdaSlider);
      QLCDNumber* lambdaDisplay = new QLCDNumber( 5, _equalizer);
      eqLayout->addWidget( lambdaDisplay);
      connect( _lambdaSlider, SIGNAL( valueChanged( int)),
               lambdaDisplay, SLOT( display( int)));
      connect( _lambdaSlider, SIGNAL( valueChanged( int)),
               this, SLOT( setLambda( int)));
      _lambdaSlider->setValue( _lambda);
      connect( lambdaReset, SIGNAL( clicked()),
               this, SLOT( resetLambda()));
      connect( _equalizer, SIGNAL( destroyed()),
               this, SLOT( resetEqualizerWidget()));
      _equalizer->setLayout( eqLayout);
      _equalizer->show();
      break;
    }
    else
    {
      _equalizer->activateWindow();
      _equalizer->setFocus();
    }
  }
  case(12):
    _showOverExposure = true;
    break;
  case(13):
    _showOverExposure = false;
    break;
  default:
    break;
  }

  fitSize();
  
  startUpdate();
}
    
/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamVideoWidget::setLambda( int lambda)
{
  _lambda = lambda;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamVideoWidget::resetLambda()
{
  _lambdaSlider->setValue( 0);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamVideoWidget::resetEqualizerWidget()
{
  _equalizer = 0;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamVideoWidget::closeEvent( QCloseEvent* e)
{
  if( _equalizer != 0)
  {
    delete _equalizer;
    _equalizer = 0;
  }
  stopUpdate();
  emit( aboutToClose());
  
  QScrollArea::closeEvent( e);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamVideoWidget::fitSize()
{
  setMaximumSize( _fidWid->sizeHint().width()+5,
                  _fidWid->sizeHint().height()+5);
   if( _fidWid->sizeHint().width() < 1025)
       resize( maximumWidth(), height());
   if( _fidWid->sizeHint().height() < 769)
       resize( width(), maximumHeight());
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamVideoWidget::contextMenuEvent( QContextMenuEvent* e)
{
  e->accept();
  _bayerPopup->popup( e->globalPos());
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamVideoWidget::focusInEvent( QFocusEvent* e)
{
  emit( focused( _camera->guid().c_str()));
  QScrollArea::focusInEvent( e);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:QlmbCamVideoWidget::updateImage()
b *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamVideoWidget::updateImage()
{
  try
  {
    long tmpIndex = _camera->mostRecentFrameIndex();
    if( tmpIndex == _currentFrameIndex)
        tmpIndex = _camera->waitForFrameFinished( _currentFrameIndex+1);
    
    _currentFrameIndex = tmpIndex;
    
    if( _freeze)
    {
      return;
    }
    
    if( _camera->tryLockFrame( _currentFrameIndex))
    {
      std::string colorcoding = _camera->colorCoding();
        
      int width = _camera->width();
      int height = _camera->height();
      unsigned char* currentFrame = _camera->getFrame( _currentFrameIndex);
      
      if( colorcoding == "YUV411")
      {
        _fidWid->setImagePerLine( currentFrame, _YUV411Conv);
        copyImageToBuffer( currentFrame);
      }
      else if( colorcoding == "YUV422")
      {
        _fidWid->setImagePerLine( currentFrame, _YUV422Conv);
        copyImageToBuffer( currentFrame);
      }
      else if( colorcoding == "YUV444")
      {
        _fidWid->setImagePerLine( currentFrame, _YUV444Conv);
        copyImageToBuffer( currentFrame);
      }
      else if( colorcoding == "MONO")
      {
        switch( _bayerDecoding)
        {
        case(BAYER_DECODING_NEAREST):
        {
          unsigned char* buffer = (unsigned char*) malloc( width * height * 3);
          BayerNearestNeighbor( currentFrame, buffer,
                                width, height, _pxlOrder);
          _fidWid->setImagePerLine( buffer, _RGBConv);
          free( buffer);
          copyImageToBuffer( currentFrame);
          break;
        }
        case(BAYER_DECODING_EDGE_SENSE):
        {
          unsigned char* buffer = (unsigned char*) malloc( width * height * 3);
          BayerEdgeSense( currentFrame, buffer,
                          width, height, _pxlOrder);
          _fidWid->setImagePerLine( buffer, _RGBConv);
          free( buffer);
          copyImageToBuffer( currentFrame);
          break;
        }
        case(BAYER_DECODING_DOWNSAMPLE):
        {
          unsigned char* buffer = (unsigned char*) malloc(
              width/2 * height/2 * 3);
          BayerDownsample( currentFrame, buffer,
                           width, height, _pxlOrder);
          _fidWid->setImagePerLine( buffer, _RGBConv);
          free( buffer);
          copyImageToBuffer( currentFrame);
          break;
        }
        default:
          if( _lambda != 0)
          {
            EqualizeImage( currentFrame, _data, width, height, _lambda, 1);
            if( _showOverExposure)
            {
              _fidWid->setImagePerLine( _data, _MONORedConv);
            }
            else
            {
              _fidWid->setImagePerLine( _data, _MONOConv);
            }
            
          }
          else
          {
            if( _showOverExposure)
            {
              _fidWid->setImagePerLine( currentFrame, _MONORedConv);
            }
            else
            {
              _fidWid->setImagePerLine( currentFrame, _MONOConv);
            }
            
            copyImageToBuffer( currentFrame);
          }
          break;
        }
      }
      else if( colorcoding == "RGB")
      {
        _fidWid->setImagePerLine( currentFrame, _RGBConv);
        copyImageToBuffer( currentFrame);
      }
      else if( colorcoding == "MONO16")
      {
        if( _lambda != 0)
        {
          EqualizeImage( currentFrame, _data, width, height, _lambda, 2);
          _fidWid->setImagePerLine( MONO16_RGBConverter::SRCTYPE( currentFrame),
                                    _MONO16Conv);
        }
        else
        {
          _fidWid->setImagePerLine( MONO16_RGBConverter::SRCTYPE( currentFrame),
                                    _MONO16Conv);
          copyImageToBuffer( currentFrame);
        }
        
      }
      else if( colorcoding == "RGB16")
      {
        _fidWid->setImagePerLine( RGB16_RGBConverter::SRCTYPE( currentFrame),
                                  _RGB16Conv);
        copyImageToBuffer( currentFrame);
      }
      else
      {
        std::cout << "Colorcoding " << colorcoding << " not supported"
                  << std::endl;
      }
      
      _camera->unlockFrame();
    }
  }
  catch( liblmbcam::LMBError& error)
  {
    QMessageBox::information( this, "Update Image ... ",
                              error.str().c_str(), QMessageBox::Ok);
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamVideoWidget::toggleFS(void)
{
  _fs=!_fs;
  
  if (_fs) 
  { 
    int y_bordersize = frameGeometry().height() - height();
    _wsize=size();
    _wpos=pos();
    resize(qApp->desktop()->size());
    move(QPoint(0,0-y_bordersize) );
  }
  else 
  { 
    move(_wpos);
    resize(_wsize);
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void 
libqlmbcam::QlmbCamVideoWidget::mouseDoubleClickEvent(QMouseEvent* e)
{
  toggleFS();
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamVideoWidget::copyImageToBuffer( unsigned char* src)
{
  if( _extGrabData!= 0 && _grabCurrentFrame < _grabNFrames)
  {
    unsigned char* data = _extGrabData + _grabCurrentFrame * _frameSize;
    memcpy( data, src, _frameSize);
 
    ++_grabCurrentFrame;
   QApplication::postEvent(_recv,
                            new SavetoMemEvent( _grabCurrentFrame, _grabNFrames));
    if( _grabCurrentFrame == _grabNFrames && _recv != 0)
    {
      QApplication::postEvent( _recv,
                               new BufferFullEvent(
                                   _extGrabData,
                                   _frameSize,
                                   _grabNFrames,
                                   _camera->width(),
                                   _camera->height(),
                                   _camera->colorCoding()
                                   ));

      _extGrabData = 0; // recv has to delete the frames
      _grabCurrentFrame = 0;
    }
    
  }
  else
  {
    memcpy( _data, src, _frameSize);
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamVideoWidget::setBuffer( unsigned int nFrames, QObject* recv)
{
  if( _extGrabData == 0)
  {
    _extGrabData = new unsigned char[nFrames*_frameSize];
    _grabNFrames = nFrames;
    _recv = recv;
  }
  else
  {
    std::cerr << __PRETTY_FUNCTION__ << ": _extGrabData != 0" << std::endl;
  }
}

void
libqlmbcam::QlmbCamVideoWidget::setSubSampling(const unsigned int subSampling) 
{
  _fidWid->setSubSampling(subSampling);
  fitSize();
}
