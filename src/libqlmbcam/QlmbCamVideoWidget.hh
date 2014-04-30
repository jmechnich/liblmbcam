/**************************************************************************
**       Title: 
**    $RCSfile: QlmbCamVideoWidget.hh,v $
**   $Revision: 1.16 $$Name:  $
**       $Date: 2005/07/14 09:36:35 $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: QlmbCamVideoWidget.hh,v $
**  Revision 1.16  2005/07/14 09:36:35  tschmidt
**  - added SubSampling capabilities
**
**  Revision 1.15  2005/05/10 20:39:09  ckeller
**  added event for progress
**
**  Revision 1.14  2005/05/03 12:58:42  ckeller
**  Fixed problem with buffer
**
**  Revision 1.13  2005/04/27 17:48:41  mechnich
**  added functions for grabbing image series
**
**  Revision 1.12  2005/02/28 00:12:39  mechnich
**  changed include path for libfidwid headers
**
**  Revision 1.11  2004/12/14 10:24:26  ronneber
**  - added "show Overexposure" to Context-menu
**
**  Revision 1.10  2004/03/18 15:40:12  mechnich
**  *** empty log message ***
**
**  Revision 1.9  2004/02/06 13:41:04  mechnich
**  changed include paths
**
**  Revision 1.8  2004/01/27 03:59:04  mechnich
**  corrected install path
**
**  Revision 1.7  2003/10/21 19:32:32  mechnich
**  improved image output
**
**  Revision 1.6  2003/10/21 17:34:26  mechnich
**  - improved handling of equalizer widget
**
**  Revision 1.5  2003/10/17 22:52:48  mechnich
**  - added geometric equalizer
**  - camera in bus widget is selected now when video widget gets focus
**  - fixed bug in update image
**
**  Revision 1.4  2003/10/08 19:30:51  mechnich
**  corrected handling of grabbed frame
**
**  Revision 1.3  2003/10/07 05:21:58  mechnich
**  - completed bayer decoding integration
**  - introduced file saving function
**  - made the widget a QScrollView
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
#include "Bayer.hh"

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

    
    FastImageDisplayWid* _fidWid;
    liblmbcam::LMBCam* _camera;
    QlmbCamUpdateThread _updateThread;
    bool _doUpdate;
    bool _freeze;
    
    unsigned char* _data;
    unsigned char* _extGrabData;
    unsigned int _grabNFrames;
    unsigned int _grabCurrentFrame;
    unsigned int _frameSize;
    long _currentFrameIndex;

    bayer_decoding_t _bayerDecoding;
    bayer_pattern_t _pxlOrder;
    int _lambda;
    

    QWidget* _equalizer;
    QSlider* _lambdaSlider;
    QObject* _recv;
    bool _showOverExposure;
        
    QMenu* _bayerPopup;
    
    static YUV411_RGBConverter _YUV411Conv;
    static YUV422_RGBConverter _YUV422Conv;
    static YUV444_RGBConverter _YUV444Conv;
    static MONO_RGBConverter   _MONOConv;
    static MONO_overexp_RGBConverter   _MONORedConv;
    static MONO16_RGBConverter _MONO16Conv;
    static RGB_RGBConverter    _RGBConv;
    static RGB16_RGBConverter  _RGB16Conv;

    friend class QlmbCamUpdateThread;
    
    bool _fs;
    QSize _wsize;
    QPoint _wpos;
  };
}


#endif
  
    
