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

#ifndef V4LCAM_HH
#define V4LCAM_HH

/*-------------------------------------------------------------------------
 *  Own includes
 *-------------------------------------------------------------------------*/
#include "LMBCam.hh"
#include "LMBCamParam.hh"
#include "LMBErrorHandler.hh"
#include "V4LCamError.hh"
#include "AtomicCounter.hh"

/*-------------------------------------------------------------------------
 *  STL includes
 *-------------------------------------------------------------------------*/
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <sstream>
#include <algorithm>

#include <pthread.h>
#include <netinet/in.h>

namespace liblmbcam
{
  class V4LCamParam;

  struct GetKey
  {
    template<typename T>
    typename T::first_type operator()(T keyValuePair) const
          {
            return keyValuePair.first;
          }
  };
  
/*======================================================================*/
/*!
 *  \class V4LCam V4LCam.hh
 *  \brief The V4LCam class implements an ieee1394 camera object
 *
 *  (description)
 */
/*======================================================================*/
class V4LCam : public LMBCam
{
public:
  std::string vendor() const;
  std::string model() const;
  std::string guid() const;

  int handle() const;
  
  void reset();
  
  LMBCamParam* param( const std::string& paramStr);

  bool setMode( const std::string& mode);
  std::string mode() const;
  std::vector<std::string> availableModes() const;
    
  bool setFramerate( double framerate, bool allowRounding=false);
  double framerate() const;
  std::vector<double> availableFramerates() const;
  std::vector<double> availableFrameratesInMode( const std::string& mode) const;

  bool setColorCoding( const std::string& colorCoding);
  std::string colorCoding() const;
  
  unsigned int left() const;
  bool setLeft( unsigned int left);
  unsigned int top() const;
  bool setTop( unsigned int top);
  unsigned int width() const;
  bool setWidth( unsigned int width);
  unsigned int maxWidth() const;
  unsigned int height() const;
  bool setHeight( unsigned int height);
  unsigned int maxHeight() const;

  void setRingBufferSize( unsigned int nFrames);

  void stopCamera();

  long mostRecentFrameIndex();
  int mostRecentRingBufferIndex();

  long waitForFrameStarted( long frameIndex);
  long waitForFrameFinished( long frameIndex);
  
  bool tryLockFrame( long frameIndex);
  unsigned char* getFrame( long frameIndex);
  void unlockFrame();
  size_t bytesUsed( long frameIndex) const;

  bool isCompressed() const;
  unsigned int nComponents() const;
  double bytePerPixel() const;

  void setExternalRingBuffer(std::vector<unsigned char*> vRingBuffer);
  std::vector<timeval> measureRelativeFrameStarttimes(int no);
  
protected:
  V4LCam( const std::string& device, const std::string& vendor,
          const std::string& model, const std::string& guid);
  ~V4LCam();

  int xioctl(int request, void *arg);

  void deviceOpen();
  void deviceClose();
  void deviceQueryCaps();
  
  void startCamera();
  void allocateBuffer();
  void freeBuffer();
  void freeBufferMmap();
  void freeRingBuffer();
  void cleanup();

  void readSettingsFromCamera();
  bool testSettings();
  
  void setupCapture();

  void initBuffer();
  size_t initBufferMmap();
  void initRingBuffer( size_t imsize);
  
  struct Framerate
  {
    Framerate()
            : numerator(1), denominator(1)
          {}
    Framerate( unsigned int numerator, unsigned int denominator)
            : numerator( numerator), denominator( denominator)
          {}
    bool operator==( const Framerate& rhs)
          {
            return (numerator == rhs.numerator) &&
                (denominator == rhs.denominator);
          }
    
    static double toDouble( const Framerate& f)
          {
            return double(f.denominator) / f.numerator;
          }
    
    unsigned int numerator, denominator;
  };

  struct Mode
  {
    Mode()
            : id( -1), colorCodingID(0)
            , nComponents( 0), bytePerPixel( -1)
            , width( 0), height( 0)
          {}
    Mode( int id, unsigned int colorCodingID,
          unsigned int nComponents, double bytePerPixel,
          unsigned int width, unsigned int height,
          const std::vector<Framerate> framerates=std::vector<Framerate>())
            : id( id), colorCodingID( colorCodingID)
            , nComponents( nComponents), bytePerPixel( bytePerPixel)
            , width( width), height( height), framerates( framerates)
           {}
    
    int id;
    unsigned int colorCodingID;
    unsigned int nComponents;
    double bytePerPixel;
    unsigned int width, height;
    std::vector<Framerate> framerates;
  };
      
  struct ColorCoding
  {
    ColorCoding()
            : id(-1), nComponents(0), bytePerPixel(-1)
          {}
    ColorCoding( unsigned int id, unsigned int nComponents, double bytePerPixel)
            : id( id), nComponents( nComponents), bytePerPixel( bytePerPixel)
          {}
    unsigned int id;
    unsigned int nComponents;
    double bytePerPixel;
  };
  
              
  void initAvailableModes();
  std::map<std::string, Mode> p_modes;

  std::vector<Framerate>
  availableFrameratesInMode(
      unsigned int pf, unsigned int width, unsigned int height);
  
  Mode p_currentMode;
  Framerate p_currentFramerate;
    
  void initAvailableParameters();
  std::map<std::string,unsigned int> p_features;

/*-------------------------------------------------------------------------
 *  Lookup tables
 *-------------------------------------------------------------------------*/
  static void initColorCodingMap();
  static std::map<std::string,ColorCoding>  p_colorCodings;

  int p_dev;
  std::string p_device;
  std::string p_vendor;
  std::string p_model;
  std::string p_guid;
  
  struct Buffer
  {
    void * start;
    size_t length;
  };
  Buffer* p_buffer;
  size_t p_nBuffers;
 
  unsigned char* p_ringBuffer;
  std::vector<unsigned char*> _vBufFrameStart;
  std::vector<size_t> _vBufUsed;
  unsigned int p_numBufferedFrames;
  bool p_dropFrames;
  pthread_t p_grabberThread;

  static void* grabberThreadStartFunctionRead( void* camera);
  static void* grabberThreadStartFunctionMmap( void* camera);
  static void* grabberThreadStartFunctionUserPtr( void* camera);
  
  std::vector<struct timeval> p_Filltimes;
  std::vector<struct timeval> p_Starttimes;
  
  AtomicCounter<long> p_readFrameCount;
  AtomicCounter<long> p_writtenFrameCount;
  AtomicCounter<long> p_lockedFrame;

  enum GrabberStatus {INIT, READING, COMPLETE, ERROR};
  GrabberStatus p_grabberStatus;

  std::vector<unsigned char*> _vExternalRingBuffer;
  
  friend class V4LCamBus;
  friend class V4LCamParamIntRange;
};

}

#include "V4LCam.icc"

#endif
