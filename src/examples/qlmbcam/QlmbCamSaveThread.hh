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

#ifndef QLMBCAMSAVETHREAD_HH
#define QLMBCAMSAVETHREAD_HH

#include <QThread>
#include <QString>
#include <QEvent> 
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
  
  
  class SaveDoneEvent : public QEvent
  {
  public:
    SaveDoneEvent(unsigned int actual_frame, unsigned int nFrames)
            :QEvent(Type(saveDoneE)), _actual_frame(actual_frame), _nFrames(nFrames)
          {}
    
    SaveDoneEvent()
    :QEvent(Type(saveDoneE)),_actual_frame(0),_nFrames(0) //Dummy
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
