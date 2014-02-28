/**************************************************************************
**       Title: 
**    $RCSfile: QlmbCamSaveThread.hh,v $
**   $Revision: 1.4 $$Name:  $
**       $Date: 2005/05/23 07:41:30 $
**   Copyright: GPL $Author: ckeller $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: QlmbCamSaveThread.hh,v $
**  Revision 1.4  2005/05/23 07:41:30  ckeller
**  Added progress bars
**
**  Revision 1.3  2005/05/10 20:57:30  ckeller
**  Added progress bar
**
**  Revision 1.2  2005/05/04 12:09:05  ckeller
**  Now using thread to save images
**
**  Revision 1.1  2005/05/03 18:53:36  ckeller
**  initial version
**
**  Revision 1.1  2001/04/27 13:44:19  ckeller
**  initial revision
**
**
**************************************************************************/

#ifndef QLMBCAMSAVETHREAD_HH
#define QLMBCAMSAVETHREAD_HH

#include <qthread.h>
#include <qstring.h>
#include <qevent.h> 
#include <tiffio.h>


namespace libqlmbcam
{

  /*-------------------------------------------------------------------------
   *  Enum for SaveDoneEvent 
   *-------------------------------------------------------------------------*/
  enum saveStat
  {
      saveDoneE = 666666
  };
  
  
  class SaveDoneEvent : public QCustomEvent
  {
  public:
    SaveDoneEvent(unsigned int actual_frame, unsigned int nFrames)
            :QCustomEvent(saveDoneE), _actual_frame(actual_frame), _nFrames(nFrames)
          {}
    
    SaveDoneEvent( void )
            :QCustomEvent(saveDoneE),_actual_frame(0),_nFrames(0) //Dummy
          {}
    
    
    bool saveDone(void)
          {
            bool done = ( actual_frame() == nFrames() );
            return done;
          }
    
    unsigned int nFrames(void)
          {
            return _nFrames;
          }
    unsigned int actual_frame(void)
          {
            return _actual_frame;
          }
    
  private:
    
    unsigned int _actual_frame;
    unsigned int _nFrames;
  };
  
  
  class QlmbCamSaveThread : public QThread
  {
  
  public:
    QlmbCamSaveThread(unsigned char* ngrab_buffer,  QString filename,
                      unsigned int grab_frameSize, 
                      unsigned int grab_numberFrames, unsigned int width,
                      unsigned int height, std::string colorCoding,
                      QObject* recv);

  protected:
    
    void run();

  private:

    void saveAsPGM(void);
    void saveAsTIFF(void);
    
    unsigned char* _ngrab_buffer;
    QString _filename;
    unsigned int _grab_frameSize;
    unsigned int _grab_numberFrames;
    unsigned int _width;
    unsigned int _height;
    std::string _colorCoding;
    QObject* _recv;

  };
}


#endif //QLMBCAMSAVETHREAD_HH
