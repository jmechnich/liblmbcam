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
YUYV_RGBConverter   libqlmbcam::QlmbCamVideoWidget::_YUYVConv;
UYVY_RGBConverter   libqlmbcam::QlmbCamVideoWidget::_UYVYConv;
YVYU_RGBConverter   libqlmbcam::QlmbCamVideoWidget::_YVYUConv;
VYUY_RGBConverter   libqlmbcam::QlmbCamVideoWidget::_VYUYConv;
YUV444_RGBConverter libqlmbcam::QlmbCamVideoWidget::_YUV444Conv;
MONO_RGBConverter   libqlmbcam::QlmbCamVideoWidget::_MONOConv;
MONO_overexp_RGBConverter   libqlmbcam::QlmbCamVideoWidget::_MONORedConv;
MONO16_RGBConverter libqlmbcam::QlmbCamVideoWidget::_MONO16Conv;
RGB_RGBConverter    libqlmbcam::QlmbCamVideoWidget::_RGBConv;
BGR_RGBConverter    libqlmbcam::QlmbCamVideoWidget::_BGRConv;
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
         _currentFrame(0), _width(0), _height(0),
         _data( 0), _extGrabData( 0), _grabNFrames( 0), _grabCurrentFrame( 0),
         _frameSize( 0), _currentFrameIndex( -1), _bytesUsed(0),
         _bayerDecoding( NO_BAYER_DECODING), _pxlOrder( BAYER_PATTERN_RGGB),
         _lambda( 0), _equalizer( 0), _recv( 0), _showOverExposure( false),
         _fs(false)

{
  setAttribute( Qt::WA_QuitOnClose, false);
  setAttribute( Qt::WA_DeleteOnClose, true);
  
  _fidWid = new FastImageDisplayWid( this);
  _fidWid->setImageSize( _camera->width(), _camera->height());
  _frameSize = (unsigned int) ( _camera->width() *
                                _camera->height()*
                                _camera->bytePerPixel());
  _data = (unsigned char*) malloc( _frameSize);
  _bytesUsed = _frameSize;
  
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
  int width  = _camera->width();
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
  else if( colorcoding == "YUV422" ||
           colorcoding == "UYVY")
  {
    for( int y=0; y < height; ++y)
    {
      _UYVYConv.lineToRgba8( _data, image.scanLine(y), width);
    }
  }
  else if( colorcoding == "VYUY")
  {
    for( int y=0; y < height; ++y)
    {
      _VYUYConv.lineToRgba8( _data, image.scanLine(y), width);
    }
  }
  else if( colorcoding == "YUYV")
  {
    for( int y=0; y < height; ++y)
    {
      _YUYVConv.lineToRgba8( _data, image.scanLine(y), width);
    }
  }
  else if( colorcoding == "YVYU")
  {
    for( int y=0; y < height; ++y)
    {
      _YVYUConv.lineToRgba8( _data, image.scanLine(y), width);
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
  // if( _fidWid->sizeHint().width() < 1025)
  //     resize( maximumWidth(), height());
  // if( _fidWid->sizeHint().height() < 769)
  //     resize( width(), maximumHeight());
  resize( maximumWidth(), maximumHeight());
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
      
      _width  = _camera->width();
      _height = _camera->height();
      _currentFrame = _camera->getFrame( _currentFrameIndex);
      _bytesUsed    = _camera->bytesUsed(_currentFrameIndex);
      
      if(      colorcoding == "YUV411")  setImage( _YUV411Conv);
      else if( colorcoding == "YUV422" ||
               colorcoding == "UYVY")    setImage( _UYVYConv);
      else if( colorcoding == "YUYV"  )  setImage( _YUYVConv);
      else if( colorcoding == "VYUY")    setImage( _VYUYConv);
      else if( colorcoding == "YVYU")    setImage( _YVYUConv);
      else if( colorcoding == "YUV444")  setImage( _YUV444Conv);
      else if( colorcoding == "YUV420")
      {
        unsigned char* buffer = new unsigned char[_width*_height*3];
        ConvertYUV420( _currentFrame, buffer, _width, _height);
        _fidWid->setImagePerLine( buffer, _RGBConv);
        delete[] buffer;
        copyImageToBuffer( _currentFrame);
      }
      else if( colorcoding == "YVU420")
      {
        unsigned char* buffer = new unsigned char[_width*_height*3];
        ConvertYVU420( _currentFrame, buffer, _width, _height);
        _fidWid->setImagePerLine( buffer, _RGBConv);
        delete[] buffer;
        copyImageToBuffer( _currentFrame);
      }
      else if( colorcoding == "JPEG")
      {
        unsigned char* buffer = new unsigned char[_width*_height*3];
        ConvertJPEG( _currentFrame, _bytesUsed, buffer);
        _fidWid->setImagePerLine( buffer, _RGBConv);
        delete[] buffer;
        //copyImageToBuffer( _currentFrame);
      }
      else if( colorcoding == "MONO")
      {
        switch( _bayerDecoding)
        {
        case(BAYER_DECODING_NEAREST):
          setBayerImage(BayerNearestNeighbor);
          break;
        case(BAYER_DECODING_EDGE_SENSE):
          setBayerImage(BayerEdgeSense);
          break;
        case(BAYER_DECODING_DOWNSAMPLE):
          setBayerImage(BayerDownsample);
          break;
        default:
          if( _lambda != 0)
          {
            EqualizeImage( _currentFrame, _data, _width, _height, _lambda, 1);
            if( _showOverExposure)
                _fidWid->setImagePerLine( _data, _MONORedConv);
            else
                _fidWid->setImagePerLine( _data, _MONOConv);
          }
          else
          {
            if( _showOverExposure) setImage( _MONORedConv);
            else                   setImage( _MONOConv);
          }
          break;
        }
      }
      else if( colorcoding == "RGB") setImage( _RGBConv);
      else if( colorcoding == "BGR") setImage( _BGRConv);
      else if( colorcoding == "MONO16")
      {
        if( _lambda != 0)
        {
          EqualizeImage( _currentFrame, _data, _width, _height, _lambda, 2);
          _fidWid->setImagePerLine( MONO16_RGBConverter::SRCTYPE( _currentFrame),
                                    _MONO16Conv);
        }
        else
        {
          _fidWid->setImagePerLine( MONO16_RGBConverter::SRCTYPE( _currentFrame),
                                    _MONO16Conv);
          copyImageToBuffer( _currentFrame);
        }
        
      }
      else if( colorcoding == "RGB16")
      {
        _fidWid->setImagePerLine( RGB16_RGBConverter::SRCTYPE( _currentFrame),
                                  _RGB16Conv);
        copyImageToBuffer( _currentFrame);
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
    //int y_bordersize = frameGeometry().height() - height();
    _wsize=size();
    _wpos=pos();
    resize(qApp->desktop()->size());
    move(QPoint(0,0));
  }
  else 
  { 
    resize(_wsize);
    move(_wpos);
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
  
  if( _data && _frameSize)
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
