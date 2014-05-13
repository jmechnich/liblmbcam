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

#ifndef QLMBCAMVIDEOWIDGET_HH
#define QLMBCAMVIDEOWIDGET_HH

#include <QScrollArea>
#include <QMenu>
#include <QHBoxLayout>
#include <QSlider>
#include <QEvent>

#include <FastImageDisplayWid_Templ.hh>
#include <X11FidWidget.hh>
#include <OpenGLFidWidget.hh>
#include <PixelConverter.hh>
#include "MONO_overexp_RGBConverter.hh"

#include <LMBCam.hh>

#include "QlmbCamUpdateThread.hh"
#include "Converters.hh"

typedef FastImageDisplayWid_Templ<X11FidWidget> FastImageDisplayWid;
//typedef FastImageDisplayWid_Templ<OpenGLTexWidget> FastImageDisplayWid;

namespace libqlmbcam
{
  
  /*-------------------------------------------------------------------------
   *  Enum for SavetoMemEvent + BufferFullEvent 
   *-------------------------------------------------------------------------*/
  enum videoSaveStat
  {
      buffer_full = QEvent::User + 1,
      save_to_mem = QEvent::User + 2
  };

/*======================================================================*/
/*!
 *  \class SavetoMemEvent
 *  \brief The SavetoMemEvent class is a QCustomEvent
 *
 *  to send progress informations about the copying to memory 
 */
/*======================================================================*/
  class SavetoMemEvent : public QEvent
  {

  public:
    SavetoMemEvent(unsigned int actual_frame, unsigned int nFrames)
            : QEvent(Type(save_to_mem)),
              _actual_frame(actual_frame), _nFrames(nFrames)
          {}

    unsigned int actual_frame(void)
          {
            return _actual_frame;
          }
    unsigned int nFrames(void)
          {
            return _nFrames;
          }
    
  private:
    unsigned int _actual_frame;
    unsigned int _nFrames;
  };
  
/*======================================================================*/
/*!
 *  \class BufferFullEvent
 *  \brief The BufferFullEvent class is a QCustomEvent
 *
 *  to pass the buffer (containing the images) onward
 */
/*======================================================================*/
  class BufferFullEvent : public QEvent
  {
  public:
    BufferFullEvent( unsigned char* buffer,
                     unsigned int frameSize,
                     unsigned int nFrames,
                     unsigned int width,
                     unsigned int height,
                     const std::string& colorCoding)
            :QEvent( Type(buffer_full) ), _buffer( buffer),
             _frameSize( frameSize), _nFrames( nFrames),
             _width( width), _height( height), _colorCoding( colorCoding)
          {}
    
    unsigned char* buffer()
          {
            return _buffer;
          }
    
    unsigned int frameSize() const
          {
            return _frameSize;
          }
    
    unsigned int nFrames() const
          {
            return _nFrames;
          }
    
    unsigned int width() const
          {
            return _width;
          }
    
    unsigned int height() const
          {
            return _height;
          }

    std::string colorCoding() const
          {
            return _colorCoding;
          }
    
  private:
    unsigned char* _buffer;
    unsigned int _frameSize;
    unsigned int _nFrames;
    unsigned int _width;
    unsigned int _height;
    std::string _colorCoding;
    
  };
  
  class QlmbCamVideoWidget : public QScrollArea
  {
    Q_OBJECT

  public:
    QlmbCamVideoWidget( liblmbcam::LMBCam* camera, QWidget* parent=0);
    ~QlmbCamVideoWidget();
    
    void updateImage();
    void startUpdate();
    void stopUpdate();
    void freezeImage( bool freeze);
    void saveCurrentImage( const QString& filename);
    void toggleFS(void);
    void setBuffer( unsigned int nFrames, QObject* recv);
    void setSubSampling(const unsigned int subSampling);


  signals:
    void aboutToClose();
    void focused( const QString& cameraGUID);
    
  protected slots:
    void setBayerMode( int id);
    void setLambda( int lambda);
    void resetLambda();
    void resetEqualizerWidget();
    
  protected:
    void closeEvent( QCloseEvent* e);
    void contextMenuEvent( QContextMenuEvent* e);
    void focusInEvent( QFocusEvent* e);
    void fitSize();
    void mouseDoubleClickEvent(QMouseEvent *e);
    void copyImageToBuffer( unsigned char* src);
    
    template<typename CONV>
    void setImage( CONV& c)
          {
            _fidWid->setImagePerLine( _currentFrame, c);
            copyImageToBuffer( _currentFrame);
          }
    template<typename T>
    void setBayerImage( T BayerFunc)
          {
            unsigned char* buffer =
                (unsigned char*) malloc( _width * _height * 3);
            BayerFunc( _currentFrame, buffer, _width, _height, _pxlOrder);
            _fidWid->setImagePerLine( buffer, _RGBConv);
            free( buffer);
            copyImageToBuffer( _currentFrame);
          }
    
    FastImageDisplayWid* _fidWid;
    liblmbcam::LMBCam*   _camera;
    QlmbCamUpdateThread  _updateThread;
    bool _doUpdate;
    bool _freeze;
    
    unsigned char* _currentFrame;
    unsigned int   _width;
    unsigned int   _height;
    
    unsigned char* _data;
    unsigned char* _extGrabData;
    unsigned int   _grabNFrames;
    unsigned int   _grabCurrentFrame;
    unsigned int   _frameSize;
    long           _currentFrameIndex;
    size_t         _bytesUsed;
    
    bayer_decoding_t _bayerDecoding;
    bayer_pattern_t  _pxlOrder;
    int _lambda;
    

    QWidget* _equalizer;
    QSlider* _lambdaSlider;
    QObject* _recv;
    bool _showOverExposure;
        
    QMenu* _bayerPopup;
    
    static YUV411_RGBConverter _YUV411Conv;
    static UYVY_RGBConverter   _UYVYConv;
    static YUYV_RGBConverter   _YUYVConv;
    static VYUY_RGBConverter   _VYUYConv;
    static YVYU_RGBConverter   _YVYUConv;
    static YUV444_RGBConverter _YUV444Conv;
    static MONO_RGBConverter   _MONOConv;
    static MONO_overexp_RGBConverter   _MONORedConv;
    static MONO16_RGBConverter _MONO16Conv;
    static RGB_RGBConverter    _RGBConv;
    static BGR_RGBConverter    _BGRConv;
    static RGB16_RGBConverter  _RGB16Conv;

    friend class QlmbCamUpdateThread;
    
    bool _fs;
    QSize _wsize;
    QPoint _wpos;
  };
}


#endif
  
    
