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

#ifndef LMBCAM_HH
#define LMBCAM_HH

#include "LMBCamError.hh"
#include "LMBErrorHandler.hh"

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <limits.h>

namespace liblmbcam
{
  
  class LMBCamParam;
  
/*======================================================================*/
/*!
 *  \class LMBCam LMBCam.hh
 *  \brief The LMBCam class specifies the interface for all camera classes
 *
 *  (description)
 */
/*======================================================================*/
class LMBCam
{
public:
  /*-----------------------------------------------------------------------
   *  Pure virtual functions to be implemented in children classes
   *-----------------------------------------------------------------------*/
  virtual std::string vendor() const=0;
  virtual std::string model() const=0;
  virtual std::string guid() const=0;
  
  virtual void reset()=0;
  virtual LMBCamParam* param( const std::string& paramStr)=0;

  virtual bool setMode( const std::string& mode)=0;
  virtual std::string mode() const=0;
  virtual std::vector<std::string> availableModes() const=0;
  virtual bool setFramerate( double framerate, bool allowRounding=false)=0;
  virtual double framerate() const=0;
  virtual std::vector<double> availableFramerates() const=0;
  virtual std::vector<double> availableFrameratesInMode(
      const std::string& mode) const=0;

  virtual bool setColorCoding( const std::string& colorCoding)=0;
  virtual std::string colorCoding() const=0;
  
  virtual unsigned int left() const=0;
  virtual bool setLeft( unsigned int left)=0;
  virtual unsigned int top() const=0;
  virtual bool setTop( unsigned int top)=0;
  virtual unsigned int width() const=0;
  virtual bool setWidth( unsigned int width)=0;
  virtual unsigned int maxWidth() const=0;
  virtual unsigned int height() const=0;
  virtual bool setHeight( unsigned int height)=0;
  virtual unsigned int maxHeight() const=0;

  virtual void setRingBufferSize( unsigned int nFrames)=0;
  virtual void stopCamera()=0;
  virtual long mostRecentFrameIndex()=0;
  virtual int mostRecentRingBufferIndex() = 0;

  virtual long waitForFrameStarted( long frameIndex)=0;
  virtual long waitForFrameFinished( long frameIndex)=0;
  virtual bool tryLockFrame( long frameIndex)=0;
  virtual size_t bytesUsed( long frameIndex) const;
  
  virtual unsigned char* getFrame( long frameIndex)=0;
  virtual void unlockFrame()=0;

  virtual bool isCompressed() const=0;
  virtual unsigned int nComponents() const=0;
  virtual double bytePerPixel() const=0;

  virtual void setExternalRingBuffer(std::vector<unsigned char*>) = 0;
  virtual std::vector<timeval> measureRelativeFrameStarttimes(int no) = 0;

  virtual void restartCamera();
  virtual void frameReceiveStarted( long frameIndex);
  virtual void frameReceiveFinished( long frameIndex);
  virtual void ringBufferCritical();
  virtual void ringBufferOverflow();

  virtual bool initSequenceFeature(char count = 16, bool autorewind = false)
  {
    return false;
  };


  virtual bool applyParametersToSequence(char position = -1)
  {
    return false;
  };

  virtual bool stopSequenceFeature()
  {
    return false;
  };

  virtual struct timeval getFrameFilltime (long number)
  {
    return timeval();
  };

  virtual struct timeval getFrameStarttime (long number)
  {
    return timeval();
  };

  virtual void setupCapture(){};
  
  void getFrameStartMutex( pthread_mutex_t** m, pthread_cond_t** c);
  void getFrameFinishMutex( pthread_mutex_t** m, pthread_cond_t** c);
  
  /*-----------------------------------------------------------------------
   *  Fixed functions
   *-----------------------------------------------------------------------*/
  void readState( std::istream& is);
  void writeState( std::ostream& os);
  void loadParametersFromMap(
      const std::map<std::string,std::string>& paramMap,
      const std::string& prefix=std::string(), bool clearExtended=true);
  void saveParametersToMap(
      std::map<std::string,std::string>& paramMap,
      const std::string& prefix=std::string());
  
  std::vector<std::string> availableParameters() const;  
  std::map<std::string,LMBCamParam*> parameterMap() const;
  std::vector<std::string> extendedParameters() const;  
  std::map<std::string,std::string> extendedParameterMap() const;

  void startCameraRunContinuous();
  void startCameraRunNFrames( long nFrames);
  long stopAtFrameIndex() const;
  bool isRunning() const;

protected:
  LMBCam()
          :p_cameraIsRunning( false), p_maxReadFrames( LONG_MAX)
        {
          pthread_mutex_init( &p_devMutex,         NULL);
          pthread_cond_init(  &p_frameStartCond,   NULL);
          pthread_mutex_init( &p_frameStartMutex,  NULL);
          pthread_cond_init(  &p_frameFinishCond,  NULL);
          pthread_mutex_init( &p_frameFinishMutex, NULL);
        }
  virtual ~LMBCam()
        {
          pthread_mutex_destroy( &p_frameFinishMutex);
          pthread_cond_destroy(  &p_frameFinishCond);
          pthread_mutex_destroy( &p_frameStartMutex);
          pthread_cond_destroy(  &p_frameStartCond);
          pthread_mutex_destroy( &p_devMutex);
        }

  virtual void startCamera()=0;

  bool p_cameraIsRunning;
  long p_maxReadFrames;

  std::map<std::string,LMBCamParam*> p_availableParameters;
  std::map<std::string,std::string>  p_extendedParameters;

  pthread_mutex_t p_devMutex;
  pthread_cond_t  p_frameStartCond;
  pthread_mutex_t p_frameStartMutex;
  pthread_cond_t  p_frameFinishCond;
  pthread_mutex_t p_frameFinishMutex;
};

#include "LMBCam.icc"

}

#endif
