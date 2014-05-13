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

#ifndef FIRECAM_HH
#define FIRECAM_HH

#include "LMBCam.hh"
#include "LMBCamParam.hh"
#include "LMBErrorHandler.hh"
#include "FireCamError.hh"
#include "AtomicCounter.hh"

#include <libraw1394/raw1394.h>
#include <libdc1394/dc1394_control.h>

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
  class FireCamParam;

/*======================================================================*/
/*!
 *  \class FireCam FireCam.hh
 *  \brief The FireCam class implements an ieee1394 camera object
 *
 *  (description)
 */
/*======================================================================*/
class FireCam : public LMBCam
{
public:
  std::string vendor() const;
  std::string model() const;
  std::string guid() const;
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
  int  mostRecentRingBufferIndex();
  long waitForFrameStarted( long frameIndex);
  long waitForFrameFinished( long frameIndex);
  bool tryLockFrame( long frameIndex);
  unsigned char* getFrame( long frameIndex);
  void unlockFrame();
  
  bool isCompressed() const;
  unsigned int nComponents() const;
  double bytePerPixel() const;

  bool initSequenceFeature(char count = 16, bool autorewind = false);
  bool applyParametersToSequence(char position);
  bool stopSequenceFeature();

  bool applySettingsToSequenceRegister();
  bool selectSequenceRegister(char position);
  
  struct timeval getFrameFilltime (long number);
  struct timeval getFrameStarttime (long number);
  

/*-------------------------------------------------------------------------
 *  This functions are deprecated and should be replaced in a future version
 *  of liblmbcam
 *-------------------------------------------------------------------------*/
  void capture();
  unsigned char* rawImage();
  void doneWithBuffer();

  void setExternalRingBuffer(std::vector<unsigned char*>);
  std::vector<timeval> measureRelativeFrameStarttimes(int no);
  
protected:
/*-----------------------------------------------------------------------
 * Number of DMA-buffers for setup() 
 *-----------------------------------------------------------------------*/
#define NUM_DMA_BUFFERS 4

/*-------------------------------------------------------------------------
 *  Define bit masks
 *-------------------------------------------------------------------------*/
  static const unsigned long V_FORMAT_0=0x80000000UL; // VGA_NONCOMPRESSED
  static const unsigned long V_FORMAT_1=0x40000000UL; // SVGA_NONCOMPRESSED_1
  static const unsigned long V_FORMAT_2=0x20000000UL; // SVGA_NONCOMPRESSED_2
  static const unsigned long V_FORMAT_3=0x10000000UL; // reserved
  static const unsigned long V_FORMAT_4=0x08000000UL; // reserved
  static const unsigned long V_FORMAT_5=0x04000000UL; // reserved
  static const unsigned long V_FORMAT_6=0x02000000UL; // STILL_IMAGE
  static const unsigned long V_FORMAT_7=0x01000000UL; // SCALABLE_IMAGE_SIZE
  
  static const unsigned long V_MODE_0=0x80000000UL;
  static const unsigned long V_MODE_1=0x40000000UL;
  static const unsigned long V_MODE_2=0x20000000UL;
  static const unsigned long V_MODE_3=0x10000000UL;
  static const unsigned long V_MODE_4=0x08000000UL;
  static const unsigned long V_MODE_5=0x04000000UL;
  static const unsigned long V_MODE_6=0x02000000UL;
  static const unsigned long V_MODE_7=0x01000000UL;
  
  static const unsigned long V_COLOR_0=0x80000000UL; // MONO
  static const unsigned long V_COLOR_1=0x40000000UL; // YUV411
  static const unsigned long V_COLOR_2=0x20000000UL; // YUV422
  static const unsigned long V_COLOR_3=0x10000000UL; // YUV444
  static const unsigned long V_COLOR_4=0x08000000UL; // RGB
  static const unsigned long V_COLOR_5=0x04000000UL; // MONO16
  static const unsigned long V_COLOR_6=0x02000000UL; // RGB16
  
  static const unsigned long V_FRAMERATE_0=0x80000000UL; //  1.875 fps
  static const unsigned long V_FRAMERATE_1=0x40000000UL; //  3.75  fps
  static const unsigned long V_FRAMERATE_2=0x20000000UL; //  7.5   fps
  static const unsigned long V_FRAMERATE_3=0x10000000UL; // 15     fps
  static const unsigned long V_FRAMERATE_4=0x08000000UL; // 30     fps
  static const unsigned long V_FRAMERATE_5=0x04000000UL; // 60     fps

  static const unsigned long long AVTReg_Sequence_Ctrl =  0xFFFFF1000220ULL;
  static const unsigned long long AVTReg_Sequence_Param = 0xFFFFF1000224ULL;
  static const unsigned long long AVTReg_Adv_Inq_1 =      0xFFFFF1000040ULL;
  static const unsigned long long AVTReg_Timebase =       0xFFFFF1000208ULL;
  
  FireCam( raw1394handle_t handle, nodeid_t node, unsigned int portNumber,
           unsigned int channel, pthread_mutex_t* libRawMutex);
  ~FireCam();

  void startCamera();

  void freeBuffer();
  void cleanup();

  void readSettingsFromCamera();
  bool testSettings();
  
  bool testFormat7PacketSize();
  raw1394handle_t handle();
  dc1394_cameracapture getCameraCapture();
  nodeid_t node();
  void setupCapture();

  raw1394handle_t p_handle;
  nodeid_t p_node;
  unsigned int p_channel;
  std::string p_deviceName;
  
  dc1394_camerainfo p_info;
  dc1394_cameracapture p_camera;

  void initAvailableParameters();
  void initAvailableModes();
  
  std::map<std::string, quadlet_t> p_availableModes;

  struct Mode
  {
    Mode()
            :p_id( -1), p_colorCodingID( -1), 
             p_nComponents( 0), p_bytePerPixel( -1),
             p_width( 0), p_height( 0),
             p_formatID( -1), p_modeBit( 0),
             p_colorBit( 0)
          {}
    Mode( int id, int colorCodingID,
          unsigned int nComponents, double bytePerPixel,
          unsigned int width, unsigned int height,
          int formatID, quadlet_t modeBit, quadlet_t colorBit=0)
            :p_id( id), p_colorCodingID( colorCodingID), 
             p_nComponents( nComponents), p_bytePerPixel( bytePerPixel),
             p_width( width), p_height( height),
             p_formatID( formatID), p_modeBit( modeBit),
             p_colorBit( colorBit)
           {}
    
    int p_id, p_colorCodingID;
    unsigned int p_nComponents;
    double p_bytePerPixel;
    unsigned int p_width, p_height;
    int p_formatID;
    quadlet_t p_modeBit, p_colorBit;
  };
      
  struct Framerate
  {
    Framerate()
        :p_id( -1), p_framerateBit( 0)
          {}
    Framerate( int id, quadlet_t framerateBit)
        :p_id( id), p_framerateBit( framerateBit)
          {}
    
    int p_id;
    quadlet_t p_framerateBit;
  };
  
  void setMode( const Mode& mode);
  void setFramerate( const Framerate& framerate);
    
  Mode p_currentMode;
  Framerate p_currentFramerate;

  unsigned int p_format7Left;
  unsigned int p_format7Top;
  unsigned int p_format7Width;
  unsigned int p_format7MaxWidth;
  unsigned int p_format7Height;
  unsigned int p_format7MaxHeight;
  unsigned int p_format7PacketSize;
  
  static void initFeatureMap();
  static void initColorCodingMap();
  static void initFormatMap();
  static void initSpeedMap();
  static void initFramerateMap();
  static void initModeMap();
  static void initTriggermodeMap();
    
  static std::map<std::string,int> p_features;
  static std::map<std::string,int> p_colorCodings;
  static std::map<quadlet_t,int> p_formats;
  static std::map<int,int> p_speeds;
  static std::map<double,Framerate> p_framerates;
  static std::map<std::string,Mode> p_modes;
  static std::map<int,int> p_triggerModes;

  static void* dmaThreadStartFunction( void* camera);
  static void* grabberThreadStartFunction( void* camera);
  static int IsoHandler( raw1394handle_t handle, int channel,
                         size_t length, quadlet_t* data);
  
  unsigned char* p_iso_handler_pos;
  unsigned char* p_iso_handler_start;
  
  unsigned int p_numBufferedFrames;
  bool p_dropFrames;
  pthread_t p_grabberThread;

  unsigned char* p_ringBuffer;
  std::vector<unsigned char*> _vBufFrameStart;
  pthread_mutex_t* p_libRawMutex;

  std::vector<struct timeval> p_Filltimes;
  std::vector<struct timeval> p_Starttimes;
  
  AtomicCounter<long> p_readFrameCount;
  AtomicCounter<long> p_writtenFrameCount;
  AtomicCounter<long> p_lockedFrame;

  bool _packetLostInDMA;
  
  enum GrabberStatus {INIT, READING, COMPLETE};
  GrabberStatus p_grabberStatus;

  bool supports_sequence_feature;
  bool sequence_feature_running;
  
  std::vector<unsigned char*> _vExternalRingBuffer;
  
  friend class FireCamBus;
  friend class FireCamParamIntRange;
  friend class FireCamParamAbsRange;

// #ifdef LIBLMBCAMDEBUG
//   std::ofstream filltimeFile;
// #endif
  

};

}

#include "FireCam.icc"

#endif
