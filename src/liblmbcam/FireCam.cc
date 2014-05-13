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

#include "FireCam.hh"

#include "FireCamParam.hh"
#include "FireCamBus.hh"
#include "FireCamBusRegistry.hh"

#include <cstdio>
#include <pthread.h>
#include <stdlib.h>
#include <assert.h>

#if defined LIBLMBCAMDEBUG
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#endif

#define CHECK_FOR_SMP 0

/*-------------------------------------------------------------------------
 *  Initialize static members
 *-------------------------------------------------------------------------*/
std::map<std::string,int> liblmbcam::FireCam::p_features;
std::map<std::string,int> liblmbcam::FireCam::p_colorCodings;
std::map<quadlet_t,int> liblmbcam::FireCam::p_formats;
std::map<int,int> liblmbcam::FireCam::p_speeds;
std::map<double,liblmbcam::FireCam::Framerate> liblmbcam::FireCam::p_framerates;
std::map<std::string,liblmbcam::FireCam::Mode> liblmbcam::FireCam::p_modes;
std::map<int,int> liblmbcam::FireCam::p_triggerModes;

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCam::FireCam( raw1394handle_t handle,
 *                                            nodeid_t node)
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::FireCam::FireCam( raw1394handle_t handle, nodeid_t node,
                             unsigned int busNumber,
                             unsigned int channel,
                             pthread_mutex_t* libRawMutex)
        :p_handle( handle),
         p_node( node),
         p_channel( channel),
         p_format7Left( 0),
         p_format7Top( 0),
         p_format7Width( 0),
         p_format7MaxWidth( 0),
         p_format7Height( 0),
         p_format7MaxHeight( 0),
         p_format7PacketSize( 0),
         p_numBufferedFrames( 8),
         p_dropFrames(true),
         p_ringBuffer( 0),
         p_libRawMutex( libRawMutex),
         p_lockedFrame( -1),
         _packetLostInDMA(0),
         p_grabberStatus(INIT),
         supports_sequence_feature(false),
         sequence_feature_running(false)
{

  memset( &p_info, 0, sizeof( dc1394_camerainfo));
  memset( &p_camera, 0, sizeof( dc1394_cameracapture));
  
  /*-----------------------------------------------------------------------
   *  Initialize internal maps
   *-----------------------------------------------------------------------*/
  if( p_features.size() == 0)
  {
    initFeatureMap();
  }
  if( p_colorCodings.size() == 0)
  {
    initColorCodingMap();
  }
  if( p_formats.size() == 0)
  {
    initFormatMap();
  }
  if( p_speeds.size() == 0)
  {
    initSpeedMap();
  }
  if( p_framerates.size() == 0)
  {
    initFramerateMap();
  }
  if( p_modes.size() == 0)
  {
    initModeMap();
  }
  if( p_triggerModes.size() == 0)
  {
    initTriggermodeMap();
  }

  initAvailableParameters();
  
  initAvailableModes();

  readSettingsFromCamera();
  
  /*-----------------------------------------------------------------------
   *  Set path to bus device if possible
   *-----------------------------------------------------------------------*/
  std::stringstream deviceString;
  /*-------------------------------------------------------------------------
   *  1. Attempt: One subdir per channel
   *-------------------------------------------------------------------------*/
  deviceString << "/dev/video1394/" << busNumber;  
  FILE* device = fopen( deviceString.str().c_str(), "rb");

  /*-----------------------------------------------------------------------
   *  2. Attempt: Suffix -<n> per channel
   *-----------------------------------------------------------------------*/
  if (!device)
  {
    deviceString.str("");
    deviceString << "/dev/video1394-" << busNumber;  

    std::cerr << deviceString.str().c_str() << std::endl;
    
    device = fopen( deviceString.str().c_str(), "rb");
  }
  
  if( device)
  {
    fclose( device);
    p_deviceName = deviceString.str();
    /*-------------------------------------------------------------------
     *  Create DMA parameter
     *-------------------------------------------------------------------*/
    p_availableParameters["DMA"] = new LMBCamParamSwitch( "DMA");
#ifdef LIBLMBCAMDEBUG
    std::cerr << "Device for firecam: " << p_deviceName << std::endl;
#endif    
  }
  else
  {
#ifdef LIBLMBCAMDEBUG
    std::cerr << "No device video1394 found." << std::endl;
#endif    
    p_deviceName = "";
  }


  /*-----------------------------------------------------------------------
   *  Set timebase to 100us -- quick hack
   *-----------------------------------------------------------------------*/
  quadlet_t buffer = 0;
  int err = 0;

  /*-----------------------------------------------------------------------
   * Check if timebase is supported
   *-----------------------------------------------------------------------*/
  err = raw1394_read(p_handle, 0xffc0 | p_node,
                     AVTReg_Adv_Inq_1, 4, &buffer);
  usleep(1000);

#ifdef LIBLMBCAMDEBUG
  std::cerr << "Adv_Inq_1: " << std::hex << ntohl(buffer) << std::dec 
            << std::endl; 
#endif

  if ((err != 0) || ((ntohl(buffer) & 0x040000000UL) == 0))
  {
    std::cerr << "Error: Timebase not supported by camera.\n";
  }

  err = raw1394_read(p_handle, 0xffc0 | p_node,
                     AVTReg_Timebase, 4, &buffer);
  usleep(1000);
  if (err != 0)
  {
    std::cerr << "Error reading raw1394: " 
              << std::hex << ntohl(buffer) << std::dec << "\n";
  }
  else
  {
    /*---------------------------------------------------------------------
     *  Write 0110(binary) to register-bits 28-31 (means  bits 0..3 
     *  in our world)
     *---------------------------------------------------------------------*/
    buffer = 0x00000000UL | 6;
    buffer = htonl(buffer);
#ifdef LIBLMBCAMDEBUG
    std::cerr << "Writing to Timebase-Register: " << std::hex << ntohl(buffer) << std::dec << std::endl; 
#endif

    err = raw1394_write(p_handle, 0xffc0 | p_node,
                      AVTReg_Timebase, 4, &buffer);
    usleep(1000);

    if (err != 0)
    {
      std::cerr << "Error writing raw1394 to AVT register Timebase: " 
                << std::hex << htonl(buffer) << std::dec << "\n";
    }    
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCam::~FireCam()
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::FireCam::~FireCam()
{
  stopCamera();

  for( std::map<std::string,LMBCamParam*>::iterator it
           = p_availableParameters.begin();
       it != p_availableParameters.end(); ++it)
  {
    delete it->second;
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCam::param( const std::string& paramStr)
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::LMBCamParam*
liblmbcam::FireCam::param( const std::string& paramStr)
{
  LMBCamParam* ret = 0;
  
  std::map<std::string,LMBCamParam*>::iterator it
      = p_availableParameters.find( paramStr);
  if( it != p_availableParameters.end())
  {
    ret = it->second;
  }
  return ret;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCam::startCamera()
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::FireCam::startCamera()
{

//#ifdef LIBLMBCAMDEBUG 
//   std::stringstream fileName;
//   timeval tv;
//   gettimeofday(&tv, 0);
//   fileName << "filltimes_" << tv.tv_sec << "_"
//            << std::setfill('0') << std::setw(6)
// 	   << tv.tv_usec << ".log";
  
//   filltimeFile.open(fileName.str().c_str());
//   filltimeFile << "Starting Camera " << tv.tv_sec << "_"
//            << std::setfill('0') << std::setw(6)
// 	       << tv.tv_usec << "\n";
//#endif

  /*-----------------------------------------------------------------------
   *  If camera is running already stop camera and iso transmission stuff
   *-----------------------------------------------------------------------*/
#ifdef LIBLMBCAMDEBUG
    std::cerr << "Starting camera" << std::endl;
#endif    

  if(!sequence_feature_running)
  {
    stopCamera();
    setupCapture();
  }
  

  for( std::map<std::string,LMBCamParam*>::iterator it =
           p_availableParameters.begin();
       it!= p_availableParameters.end(); ++it)
  {
    /*---------------------------------------------------------------------
     *  Setup trigger mode if feature available
     *---------------------------------------------------------------------*/
    if( it->first == "trigger")
    {
      if( dc1394_set_trigger_mode( p_handle, p_node, TRIGGER_MODE_0)
          != DC1394_SUCCESS)
      {
        cleanup();
        FireCamError error( "unable to set trigger mode");
        LMBErrorHandler::ErrorHandler()->handle( error);
        return;
      }
    }
    
    /*---------------------------------------------------------------------
     *  Reset values of volatile features (e.g. absolute features)
     *---------------------------------------------------------------------*/
    it->second->resetValue();
  }
  
  /*-----------------------------------------------------------------------
   *  Setup ringbuffer
   *-----------------------------------------------------------------------*/
  _vBufFrameStart.resize(p_numBufferedFrames);
  
  if(_vExternalRingBuffer.size() == 0)
  {
    int buffSize = p_numBufferedFrames * (int)(width() 
                                               * height() 
                                               * bytePerPixel());
#ifdef LIBLMBCAMDEBUG
    std::cerr << "Using internal buffer of size " << buffSize << ".\n";
#endif    
    p_ringBuffer = new unsigned char[buffSize];
    
    for( unsigned int i = 0; i < p_numBufferedFrames; ++i)
    {
      _vBufFrameStart[i] = p_ringBuffer + i * (int)(width() * height() * bytePerPixel());
    }
  }
  else
  {
#ifdef LIBLMBCAMDEBUG
    std::cerr << "Using external buffer.\n";
#endif    
    for( unsigned int i = 0; i < p_numBufferedFrames; ++i)
    {
      _vBufFrameStart[i] = _vExternalRingBuffer[i];
    }   
  }
  
  p_Filltimes.resize(p_numBufferedFrames);
  p_Starttimes.resize(p_numBufferedFrames);

#ifdef LIBLMBCAMDEBUG
  std::cerr << "RingBuffer size:  " 
            << p_camera.quadlets_per_frame * 4 * p_numBufferedFrames << std::endl
            << "Number of frames in ringbuffer: " 
            << p_numBufferedFrames << std::endl
            << "Quadlets per Frame: " 
            << p_camera.quadlets_per_frame << std::endl
            << "Quadlets per Packet: " 
            << p_camera.quadlets_per_packet << std::endl;
#endif
  
  p_readFrameCount = 0;
  p_writtenFrameCount = 0;
  p_grabberStatus = INIT;
  
  /*---------------------------------------------------------------------
   *  Start data transmission
   *---------------------------------------------------------------------*/
  if( dc1394_start_iso_transmission( p_handle, p_node) != DC1394_SUCCESS)
  {
    cleanup();
    FireCamError error( "unable to start data transmission");
    LMBErrorHandler::ErrorHandler()->handle( error);
    return;
  } 
  
  p_cameraIsRunning = true;
  
  /*-----------------------------------------------------------------------
   *  Start grabber thread
   *-----------------------------------------------------------------------*/
  if( p_deviceName == "" || p_availableParameters["DMA"]->asInt() == 0)
  {
    pthread_create( &p_grabberThread, NULL, grabberThreadStartFunction, this);
  }
  else
  {

#ifdef LIBLMBCAMDEBUG
    std::cerr << "DMA ringbuffer size: " << p_camera.dma_buffer_size << "\n"
              << "DMA buffer frame size: " << p_camera.dma_frame_size << "\n"
              << "Number of frames: " << p_numBufferedFrames << std::endl;
#endif

    pthread_create( &p_grabberThread, NULL, dmaThreadStartFunction, this);
  }  
}
 
/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/ 
void liblmbcam::FireCam::setExternalRingBuffer(std::vector<unsigned char*> vRingBuffer)
{
    _vExternalRingBuffer = vRingBuffer;
    p_numBufferedFrames = vRingBuffer.size();
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCam::stopCamera()
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::FireCam::stopCamera()
{
  if (sequence_feature_running)
    {
      stopSequenceFeature();
    }

  if( p_cameraIsRunning)
  {

#ifdef LIBLMBCAMDEBUG
     std::cerr << "Stopping camera." << std::endl;
#endif    
    
    /*---------------------------------------------------------------------
     *  tell thread to finish
     *---------------------------------------------------------------------*/
    p_cameraIsRunning = false;
    
    /*-------------------------------------------------------------------
     *  Wait for grabber thread to finish
     *-------------------------------------------------------------------*/
    pthread_join( p_grabberThread, NULL);
    
    /*---------------------------------------------------------------------
     *  Stop data transmission from camera
     *---------------------------------------------------------------------*/
    dc1394bool_t isoStatus = DC1394_FALSE;
    if( dc1394_get_iso_status( p_handle, p_node, &isoStatus)!=DC1394_SUCCESS)
    {
      FireCamError error( "unable to get iso status");
      LMBErrorHandler::ErrorHandler()->handle( error);
    }
      
    if((isoStatus == DC1394_TRUE) &&
       (dc1394_stop_iso_transmission( p_handle, p_node)!=DC1394_SUCCESS))
    {
      FireCamError error( "unable to stop data transmission");
      LMBErrorHandler::ErrorHandler()->handle( error);
    }
    /*---------------------------------------------------------------------
     *  free ring buffer
     *---------------------------------------------------------------------*/
    freeBuffer();

    /*---------------------------------------------------------------------
     *  release the dc1394-resources
     *---------------------------------------------------------------------*/
    cleanup();
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCam::freeBuffer()
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::FireCam::freeBuffer()
{
    if( p_ringBuffer != 0)
    {
      delete[] p_ringBuffer;
    }
    
    _vExternalRingBuffer.clear();
  
    p_ringBuffer = 0;
    p_numBufferedFrames = 8;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCam::setMode( const std::string& mode)
 *  ==> see headerfile
 *=======================================================================*/
bool
liblmbcam::FireCam::setMode( const std::string& mode)
{
  stopCamera();
  bool ret = false;
  /*-----------------------------------------------------------------------
   *  Check in p_modes if mode is valid
   *-----------------------------------------------------------------------*/
  if( p_modes.find( mode) == p_modes.end())
  {
    std::stringstream errString;
    errString << mode << " is not a valid mode";
    FireCamError error( errString.str());
    LMBErrorHandler::ErrorHandler()->handle( error);
  }
  else
  {
    /*---------------------------------------------------------------------
     *  Check if camera supports mode
     *---------------------------------------------------------------------*/
    if( p_availableModes.find( mode) == p_availableModes.end())
    {
      std::stringstream errString;
      errString << "camera does not support mode " << mode;
      
      FireCamError error( errString.str());
      LMBErrorHandler::ErrorHandler()->handle( error);
    }
    else
    {
      /*-------------------------------------------------------------------
       *  Set mode if everything is ok
       *-------------------------------------------------------------------*/
      setMode( p_modes[mode]);
      ret = true;
    }
  }

  return ret;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCam::mode() const
 *  ==> see headerfile
 *=======================================================================*/
std::string
liblmbcam::FireCam::mode() const
{
  std::string ret;
  
  /*-----------------------------------------------------------------------
   *  Look up current mode name in p_modes
   *-----------------------------------------------------------------------*/
  for( std::map<std::string,Mode>::const_iterator it = p_modes.begin();
       it != p_modes.end(); ++it)
  {
    if( it->second.p_id == p_currentMode.p_id)
    {
      ret = it->first;
      break;
    }
  }

  return ret;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCam::setFramerate( double framerate,
 *                                                 bool allowRounding)
 *  ==> see headerfile
 *=======================================================================*/
bool
liblmbcam::FireCam::setFramerate( double framerate, bool allowRounding)
{
  stopCamera();
  bool ret = false;
  
  /*-----------------------------------------------------------------------
   *  Format7 framerate setup
   *-----------------------------------------------------------------------*/
  if( p_currentMode.p_formatID == FORMAT_SCALABLE_IMAGE_SIZE)
  {
    // calculate resulting packet size and round to multiple of 4
    p_format7PacketSize = ((unsigned int)( 125 * framerate * p_format7Width *
                                           p_format7Height *
                                           p_currentMode.p_bytePerPixel /
                                           1000000 / 4 + 0.5)) * 4;

    ret = true;
  }
  /*-----------------------------------------------------------------------
   *  Standard framerate setup
   *-----------------------------------------------------------------------*/
  else
  {
    p_format7PacketSize = 0;
    if( !allowRounding)
    {
      /*-------------------------------------------------------------------
       *  Check if framerate is valid in principle
       *-------------------------------------------------------------------*/
      std::map<double,Framerate>::const_iterator it
          = p_framerates.find( framerate);
      if( it == p_framerates.end())
      {
        std::stringstream errString;
        errString << framerate << " is not a valid framerate "
                  << "(rounding disabled)";
        FireCamError error( errString.str());
        LMBErrorHandler::ErrorHandler()->handle( error);
      }
      /*-------------------------------------------------------------------
       *  Check if framerate is supported in current mode
       *-------------------------------------------------------------------*/
      else
      {
        std::map<std::string,quadlet_t>::const_iterator it2 =
            p_availableModes.find( mode());
        
        if( it2 != p_availableModes.end() &&
            it2->second & it->second.p_framerateBit)
        {
          setFramerate( it->second);
          ret = true;
        }
        else
        {
          std::cerr << "Quadlet: " << it2->second << std::endl;
          std::stringstream errString;
          errString << "camera does not support framerate " << framerate
                    << " in mode " << mode() << " (rounding disabled)";
          FireCamError error( errString.str());
          LMBErrorHandler::ErrorHandler()->handle( error);
        }
      }
    }
    else // if rounding is allowed
    {
      /*-------------------------------------------------------------------
       *  Try to set framerate "as it is" by recursively calling this function
       *-------------------------------------------------------------------*/
      try 
      {
        ret = setFramerate( framerate, false);
      }
      catch( FireCamError& error) 
      {
        Framerate roundedFramerate;
        double diffSquare = -1;
        std::map<double,Framerate>::const_iterator it = p_framerates.begin();
        std::map<std::string,quadlet_t>::const_iterator it2 =
            p_availableModes.find( mode());
        
        if( it != p_framerates.end() &&
            it2 != p_availableModes.end())
        {
          /*-----------------------------------------------------------------
           *  Compute sigma square for all available framerates and take
           *  the one with minimal difference to the original framerate
           *-----------------------------------------------------------------*/
          while( it != p_framerates.end())
          {
            if( it->second.p_framerateBit & it2->second)
            {
              diffSquare = (framerate - it->first) * (framerate - it->first);
              roundedFramerate = it->second;
              ++it;
              
              double tmpDiffSquare;
              while( it != p_framerates.end())
              {
                tmpDiffSquare = (framerate - it->first) *
                    (framerate - it->first);
                if( tmpDiffSquare <= diffSquare &&
                    it->second.p_framerateBit & it2->second)
                {
                  diffSquare = tmpDiffSquare;
                  roundedFramerate = it->second;
                }
                ++it;
              }
            }
            else
            {
              ++it;
            }
          }
          
          if( diffSquare != -1)
          {
            setFramerate( roundedFramerate);
            
#ifdef LIBLMBCAMDEBUG
            if( framerate != FireCam::framerate())
            {
              std::cerr << "Rounded framerate from " 
                        << framerate << " to "
                        << FireCam::framerate() << std::endl;
            }
#endif
            ret = true;
          }
          else
          {
            std::cerr << "framerate rounding failed" << std::endl;
          }
        }
        else
        {
          std::cerr << "framerate rounding failed" << std::endl;
        }
      }
    }
  }
  return ret;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCam::framerate() const
 *  ==> see headerfile
 *=======================================================================*/
double
liblmbcam::FireCam::framerate() const
{
  double ret=0;
  
  if( p_currentMode.p_formatID == FORMAT_SCALABLE_IMAGE_SIZE)
  {
    ret = 1000000 * p_format7PacketSize/ (p_format7Width * p_format7Height *
                                          p_currentMode.p_bytePerPixel * 125);
  }
  else
  {
    for( std::map<double,Framerate>::const_iterator it = p_framerates.begin();
         it != p_framerates.end(); ++it)
    {
      if( it->second.p_id == p_currentFramerate.p_id)
      {
        ret = it->first;
        break;
      }
    }
  }
  
  return ret;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCam::setColorCoding( const std::string& colorCoding)
 *  ==> see headerfile
 *=======================================================================*/
bool
liblmbcam::FireCam::setColorCoding( const std::string& colorCoding)
{
  bool ret = false;
  
  std::string newMode, tmp = mode();
  std::stringstream modeStream;
  modeStream << tmp.substr( 0, tmp.find( " ")) << " " << colorCoding;
  
  if( p_availableModes.find( modeStream.str()) != p_availableModes.end())
  {
    setMode( modeStream.str());
    ret = true;
  }
  else
  {
    std::stringstream errString;
    errString << "Color coding " << colorCoding << " not available for mode "
              << tmp.substr( 0, tmp.find( " ")) << std::endl;
    FireCamError error( errString.str());
    LMBErrorHandler::ErrorHandler()->handle( error);
  }
  
  return ret;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCam::initAvailableParameters()
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::FireCam::initAvailableParameters()
{
  dc1394_feature_info feature;
  memset(&feature, 0, sizeof(dc1394_feature_info));
  
  for( std::map<std::string,int>::const_iterator it
           = p_features.begin(); it != p_features.end(); ++it)
  {
    
    feature.feature_id = it->second;
    
    if( dc1394_get_camera_feature( p_handle, p_node,
                                   &feature) != DC1394_SUCCESS)
    {
      std::cerr << "Error reading camera parameter " << it->first << ".\n";
    }
    else
    {
//       std::cerr << std::endl << "New Abs Parameter: " << it->first << std::endl
//                 << " Abs Value: " << feature.abs_value  << std::endl
//                 << " Abs Value min: " << feature.abs_min  << std::endl
//                 << " Abs Value max: " << feature.abs_max  << std::endl
//                 << " Value: " << feature.value << std::endl
//                 << " Value min: " << feature.min << std::endl
//                 << " Value max: " << feature.max  << std::endl
//                 << " Abs capable: " << feature.absolute_capable << std::endl
//                 << " Auto capable: " << feature.auto_capable << std::endl
//                 << " Available: " << feature.available << std::endl;    

      if( feature.available != 0)
      {
        if( it->first == "white_balance")
        {
          p_availableParameters[it->first + "_u"] =
              new FireCamParamWhiteBalanceU( this, it->first + "_u",
                                             feature, p_libRawMutex);
          p_availableParameters[it->first + "_v"] =
              new FireCamParamWhiteBalanceV( this, it->first + "_v",
                                             feature, p_libRawMutex);
        }
        else
        {
          p_availableParameters[it->first] =
              new FireCamParamIntRange( this, it->first, feature,
                                        p_libRawMutex);
          if( feature.absolute_capable)
          {
            std::ostringstream paramName;
            paramName << it->first << "_abs";
            p_availableParameters[paramName.str()] =
                new FireCamParamAbsRange( this, it->first, feature,
                                          p_libRawMutex);
            paramName << "_switch";
            p_availableParameters[paramName.str()] =
                new FireCamParamAbsSwitch( this, it->first, feature,
                                           p_libRawMutex);
          }          
        }
        
        if( feature.auto_capable)
        {
          std::ostringstream paramName;
          paramName << it->first << "_auto";
          p_availableParameters[paramName.str()] =
              new FireCamParamAutoSwitch( this, paramName.str(), feature,
                                          p_libRawMutex);
        }
      }
    }
    
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCam::initAvailableModes()
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::FireCam::initAvailableModes()
{
  if( p_availableModes.size() == 0)
  {
    quadlet_t supportedFormats=0;
    if( dc1394_query_supported_formats( p_handle, p_node, &supportedFormats)
        != DC1394_SUCCESS)
    {
      FireCamCriticalError error(
          "error while querying supported formats");
      LMBErrorHandler::ErrorHandler()->handle( error);
    }

    quadlet_t modes=0, colorCoding=0;
    for( std::map<quadlet_t,int>::const_iterator formatIt
             = p_formats.begin(); formatIt != p_formats.end(); ++formatIt)
    {
      if( ( supportedFormats & formatIt->first))
      {
        if( dc1394_query_supported_modes( p_handle, p_node,
                                          formatIt->second, &modes)
            != DC1394_SUCCESS)
        {
          FireCamCriticalError error(
              "error while querying supported modes");
          LMBErrorHandler::ErrorHandler()->handle( error);
        }

        for( std::map<std::string,Mode>::const_iterator modeIt
                 = p_modes.begin(); modeIt != p_modes.end(); ++modeIt)
        {
          if( ( modes & modeIt->second.p_modeBit) &&
              ( modeIt->second.p_formatID == formatIt->second))
          {
            if( modeIt->second.p_formatID == FORMAT_SCALABLE_IMAGE_SIZE)
            {
              if( dc1394_query_format7_color_coding( p_handle, p_node,
                                                     modeIt->second.p_id,
                                                     &colorCoding)
                  != DC1394_SUCCESS)
              {
                FireCamCriticalError error(
                    "error while querying color coding");
                LMBErrorHandler::ErrorHandler()->handle( error);
                return;
              }
              if( colorCoding & modeIt->second.p_colorBit)
              {
                p_availableModes[modeIt->first] = 0;
              }
            }
            else
            {
              quadlet_t supportedFramerates = 0;
              
              if( dc1394_query_supported_framerates(
                      p_handle, p_node,
                      modeIt->second.p_formatID,
                      modeIt->second.p_id,
                      &supportedFramerates)
                  != DC1394_SUCCESS)
              {
                FireCamCriticalError error(
                    "error while querying supported framerates");
                LMBErrorHandler::ErrorHandler()->handle( error);
              }
              else
              {
                p_availableModes[modeIt->first] = supportedFramerates;
              }
            }
          }
        }
      }
    }
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::FireCam::readSettingsFromCamera()
{
  /*---------------------------------------------------------------------
   *  Get camera info for vendor and model
   *---------------------------------------------------------------------*/
  memset(&p_info, 0, sizeof(dc1394_camerainfo));

  if( dc1394_get_camera_info( p_handle, p_node, &p_info) !=
      DC1394_SUCCESS)
  {
    FireCamError error( "Error querying camera info");
    LMBErrorHandler::ErrorHandler()->handle( error);
  }
  
  /*---------------------------------------------------------------------
   *  Get miscinfo
   *---------------------------------------------------------------------*/
  dc1394_miscinfo miscInfo = { 0 };
  if( dc1394_get_camera_misc_info( p_handle, p_node, &miscInfo) !=
      DC1394_SUCCESS)
  {
    FireCamError error( "Error querying camera miscinfo");
    LMBErrorHandler::ErrorHandler()->handle( error);
  }
    
  /*---------------------------------------------------------------------
   *  Check if currently set mode is valid; if yes then set the current
   *  mode to it
   *---------------------------------------------------------------------*/
  bool modeFound = false;
  for( std::map<std::string,Mode>::const_iterator it = p_modes.begin();
       it != p_modes.end(); ++it)
  {
    if( miscInfo.mode == it->second.p_id &&
        miscInfo.format == it->second.p_formatID)
    {
      setMode( it->second);
      modeFound = true;
      break;
    }
  }
  
  if( !modeFound)
  {
    std::map<std::string,quadlet_t>::const_iterator it = 
        p_availableModes.begin();
    if( it != p_availableModes.end())
    {
#ifdef LIBLMBCAMDEBUG
      std::cerr << "Camera is set to invalid mode, setting to mode "
                << it->first << " now" << std::endl;
#endif
      setMode( it->first);
    }
    else
    {
      FireCamError error( "Serious error: no modes available!");
      LMBErrorHandler::ErrorHandler()->handle( error);
    }
  }
  
  switch( miscInfo.format)
  {
  case( FORMAT_SCALABLE_IMAGE_SIZE):
      {
        if( dc1394_query_format7_image_position( p_handle, p_node,
                                                 miscInfo.mode,
                                                 &p_format7Left,
                                                 &p_format7Top) !=
            DC1394_SUCCESS)
        {
          FireCamError error( "Error querying format7 image position");
          LMBErrorHandler::ErrorHandler()->handle( error);
        }

        if( dc1394_query_format7_image_size( p_handle, p_node,
                                             miscInfo.mode,
                                             &p_format7Width,
                                             &p_format7Height) !=
            DC1394_SUCCESS)
        {
          FireCamError error( "Error querying format7 image size");
          LMBErrorHandler::ErrorHandler()->handle( error);
        }
    
        if( dc1394_query_format7_byte_per_packet( p_handle, p_node,
                                                  miscInfo.mode,
                                                  &p_format7PacketSize) !=
            DC1394_SUCCESS)
        {
          FireCamError error( "Error querying format7 packet size");
          LMBErrorHandler::ErrorHandler()->handle( error);
        }
      }
  default:
  {
    for( std::map<double,Framerate>::const_iterator it = p_framerates.begin();
         it != p_framerates.end(); ++it)
    {
      if( miscInfo.framerate == it->second.p_id)
      {
        if( p_availableModes[mode()] & it->second.p_framerateBit)
        {
          p_currentFramerate = it->second;
        }
        else
        {
          setFramerate(15, true);
        }
        
        break;
      }
    }
    break;
  }
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCam::testSettings()
 *  ==> see headerfile
 *=======================================================================*/
bool
liblmbcam::FireCam::testSettings()
{
  if( p_currentMode.p_formatID == FORMAT_SCALABLE_IMAGE_SIZE)
  {
    /*---------------------------------------------------------------------
     *  First query max image size in requested mode
     *---------------------------------------------------------------------*/
    if( p_format7Width == 0 ||
        p_format7Left + p_format7Width > p_format7MaxWidth )
    {
      std::cerr << "warning: requested width " << p_format7Width
                << " not allowed for given left border and mode. "
                << "resetting it to ";
      p_format7Width = p_format7MaxWidth - p_format7Left;
      std::cerr << p_format7Width << std::endl;
    }
    
    if( p_format7Height == 0 ||
        p_format7Top + p_format7Height > p_format7MaxHeight )
    {
      std::cerr << "warning: requested height " << p_format7Height
                << " not allowed for given left border and mode. "
                << "resetting it to ";
      p_format7Height = p_format7MaxHeight - p_format7Top;
      std::cerr << p_format7Height << std::endl;
    }
    
   
    
    return testFormat7PacketSize();
  }
  else
  {
    std::map<std::string,quadlet_t>::const_iterator it =
        p_availableModes.find( mode());
    if( it != p_availableModes.end())
    {
      if( !(it->second & p_currentFramerate.p_framerateBit))
      {
        setFramerate( 15, true);
      }
    }
    else
    {
      std::stringstream errString;
      errString << "Invalid mode " << mode();
      FireCamError error( errString.str());
      LMBErrorHandler::ErrorHandler()->handle( error);
      return false;
    }
  }
  
  return true;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCam::testFormat7PacketSize()
 *  ==> see headerfile
 *=======================================================================*/
bool
liblmbcam::FireCam::testFormat7PacketSize()
{
  std::cerr << " requesting packet size: " << p_format7PacketSize << std::endl;
  
  /*-----------------------------------------------------------------------
   *  to query packet size, we must first set requested width and height
   *-----------------------------------------------------------------------*/
  if( dc1394_set_format7_image_position(p_handle, p_node,
                                        p_currentMode.p_id,
                                        0, 0) != DC1394_SUCCESS)
  {
    FireCamError error( "Error setting format7 image position");
    LMBErrorHandler::ErrorHandler()->handle( error);
    return false;
  }
  
  if( dc1394_set_format7_image_size(p_handle, p_node,
                                    p_currentMode.p_id,
                                    p_format7Width, 
                                    p_format7Height) != DC1394_SUCCESS)
  {
    FireCamError error( "Error setting format7 image size");
    LMBErrorHandler::ErrorHandler()->handle( error);
    return false;
  }
  
  if( dc1394_set_format7_image_position(p_handle, p_node,
                                        p_currentMode.p_id,
                                        p_format7Left, 
                                        p_format7Top) != DC1394_SUCCESS)
  {
    FireCamError error( "Error setting format7 image position");
    LMBErrorHandler::ErrorHandler()->handle( error);
    return false;
  }
  

  /*-----------------------------------------------------------------------
   *  Now packet size could be queried
   *-----------------------------------------------------------------------*/
  unsigned int min_bytes, max_bytes;
  if( dc1394_query_format7_packet_para( p_handle, p_node,
                                        p_currentMode.p_id,
                                        &min_bytes,
                                        &max_bytes) != DC1394_SUCCESS)
  {
    FireCamError error( "Error querying format7 packet parameters");
    LMBErrorHandler::ErrorHandler()->handle( error);
    return false;
  }
  std::cerr << "min/max packet size is: " << min_bytes << " / " << max_bytes  << std::endl;
  
  if( p_format7PacketSize < min_bytes || p_format7PacketSize > max_bytes )
  {
    unsigned int bpp;
    if( dc1394_query_format7_recommended_byte_per_packet( p_handle, p_node,
                                                          p_currentMode.p_id,
                                                          &bpp)
        != DC1394_SUCCESS)
    {
      FireCamError error( "Error querying recommended packet size");
      LMBErrorHandler::ErrorHandler()->handle( error);
      return false;
    }

    if( bpp == 0)
    {
      bpp = max_bytes;
      std::cerr << "Packet size was 0, setting to max packet size: " << bpp << std::endl;
    }
    else
    {
      std::cerr << "Setting packet size to recommended packet size: " << bpp << std::endl;
    }
    
    p_format7PacketSize = bpp;
  }
  
  return true;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCam::availableFrameratesInMode(
 *                                                  const std::string& mode) const
 *  ==> see headerfile
 *=======================================================================*/
std::vector<double>
liblmbcam::FireCam::availableFrameratesInMode( const std::string& mode) const
{
  std::vector<double> ret;
  
  std::map<std::string,quadlet_t>::const_iterator modeIt =
      p_availableModes.find( mode);
  
  if( modeIt == p_availableModes.end())
  {
    std::stringstream errString;
    errString << "Camera does not support mode " << mode;
    FireCamError error( errString.str());
    LMBErrorHandler::ErrorHandler()->handle( error);
  }
  else
  {
    for( std::map<double,Framerate>::const_iterator it = p_framerates.begin();
         it != p_framerates.end(); ++it)
    {
      if( it->second.p_framerateBit & modeIt->second)
      {
        ret.push_back( it->first);
      }
    }
  }
  return ret;
}
  
/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCamParam::initFeatureMap()
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::FireCam::initFeatureMap()
{
  p_features["brightness"]      = FEATURE_BRIGHTNESS;
  p_features["exposure"]        = FEATURE_EXPOSURE;
  p_features["sharpness"]       = FEATURE_SHARPNESS;
  p_features["white_balance"]   = FEATURE_WHITE_BALANCE;
  p_features["hue"]             = FEATURE_HUE;
  p_features["saturation"]      = FEATURE_SATURATION;
  p_features["gamma"]           = FEATURE_GAMMA;
  p_features["shutter"]         = FEATURE_SHUTTER;
  p_features["gain"]            = FEATURE_GAIN;
  p_features["iris"]            = FEATURE_IRIS;
  p_features["focus"]           = FEATURE_FOCUS;
  p_features["temperature"]     = FEATURE_TEMPERATURE;
  p_features["trigger"]         = FEATURE_TRIGGER;
  p_features["zoom"]            = FEATURE_ZOOM;
  p_features["pan"]             = FEATURE_PAN;
  p_features["tilt"]            = FEATURE_TILT;
  p_features["optical_filter"]  = FEATURE_OPTICAL_FILTER;
  p_features["capture_size"]    = FEATURE_CAPTURE_SIZE;
  p_features["capture_quality"] = FEATURE_CAPTURE_QUALITY; 
  
  
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCam::initColorCodingMap()
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::FireCam::initColorCodingMap()
{
  p_colorCodings["MONO"]  = COLOR_FORMAT7_MONO8;
  p_colorCodings["YUV411"] = COLOR_FORMAT7_YUV411;
  p_colorCodings["YUV422"] = COLOR_FORMAT7_YUV422;
  p_colorCodings["YUV444"] = COLOR_FORMAT7_YUV444;
  p_colorCodings["RGB"]   = COLOR_FORMAT7_RGB8;
  p_colorCodings["MONO16"] = COLOR_FORMAT7_MONO16;
  p_colorCodings["RGB16"]  = COLOR_FORMAT7_RGB16;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCam::initFormatMap()
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::FireCam::initFormatMap()
{
  p_formats[V_FORMAT_0] = FORMAT_VGA_NONCOMPRESSED;
  p_formats[V_FORMAT_1] = FORMAT_SVGA_NONCOMPRESSED_1;
  p_formats[V_FORMAT_2] = FORMAT_SVGA_NONCOMPRESSED_2;
  p_formats[V_FORMAT_3] = 387;
  p_formats[V_FORMAT_4] = 388;
  p_formats[V_FORMAT_5] = 389;
  p_formats[V_FORMAT_6] = FORMAT_STILL_IMAGE;
  p_formats[V_FORMAT_7] = FORMAT_SCALABLE_IMAGE_SIZE;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCam::initSpeedMap()
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::FireCam::initSpeedMap()
{
  p_speeds[100] = SPEED_100;
  p_speeds[200] = SPEED_100;
  p_speeds[400] = SPEED_100;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCam::initFramerateMap()
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::FireCam::initFramerateMap()
{
  p_framerates[1.875] = Framerate( FRAMERATE_1_875, V_FRAMERATE_0);
  p_framerates[3.75]  = Framerate( FRAMERATE_3_75 , V_FRAMERATE_1);
  p_framerates[7.5]   = Framerate( FRAMERATE_7_5  , V_FRAMERATE_2);
  p_framerates[15]    = Framerate( FRAMERATE_15   , V_FRAMERATE_3);
  p_framerates[30]    = Framerate( FRAMERATE_30   , V_FRAMERATE_4);
  p_framerates[60]    = Framerate( FRAMERATE_60   , V_FRAMERATE_5);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCam::initModeMap()
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::FireCam::initModeMap()
{
  p_modes["160x120 YUV444"]   = Mode( MODE_160x120_YUV444, 
                                      COLOR_FORMAT7_YUV444, 0, 3.0,
                                      160, 120,
                                      FORMAT_VGA_NONCOMPRESSED, V_MODE_0);
  p_modes["320x240 YUV422"]   = Mode( MODE_320x240_YUV422,
                                      COLOR_FORMAT7_YUV422, 0, 2.0,
                                      320, 240,
                                      FORMAT_VGA_NONCOMPRESSED, V_MODE_1);
  p_modes["640x480 YUV411"]   = Mode( MODE_640x480_YUV411,
                                      COLOR_FORMAT7_YUV411, 0, 1.5,
                                      640, 480,
                                      FORMAT_VGA_NONCOMPRESSED, V_MODE_2);
  p_modes["640x480 YUV422"]   = Mode( MODE_640x480_YUV422,
                                      COLOR_FORMAT7_YUV422, 0, 2.0,
                                      640, 480,
                                      FORMAT_VGA_NONCOMPRESSED, V_MODE_3);
  p_modes["640x480 RGB"]      = Mode( MODE_640x480_RGB,
                                      COLOR_FORMAT7_RGB8, 3, 3.0,
                                      640, 480,
                                      FORMAT_VGA_NONCOMPRESSED, V_MODE_4);
  p_modes["640x480 MONO"]     = Mode( MODE_640x480_MONO,
                                      COLOR_FORMAT7_MONO8, 1, 1.0,
                                      640, 480,
                                      FORMAT_VGA_NONCOMPRESSED, V_MODE_5);
  p_modes["640x480 MONO16"]   = Mode( MODE_640x480_MONO16,
                                      COLOR_FORMAT7_MONO16, 1, 2.0,
                                      640, 480,
                                      FORMAT_VGA_NONCOMPRESSED, V_MODE_6);
  p_modes["800x600 YUV422"]   = Mode( MODE_800x600_YUV422,
                                      COLOR_FORMAT7_YUV422, 0, 2.0,
                                      800, 600,
                                      FORMAT_SVGA_NONCOMPRESSED_1, V_MODE_0);
  p_modes["800x600 RGB"]      = Mode( MODE_800x600_RGB,
                                      COLOR_FORMAT7_RGB8, 3, 3.0,
                                      800, 600,
                                      FORMAT_SVGA_NONCOMPRESSED_1, V_MODE_1);
  p_modes["800x600 MONO"]     = Mode( MODE_800x600_MONO,
                                      COLOR_FORMAT7_MONO8, 1, 1.0,
                                      800, 600,
                                      FORMAT_SVGA_NONCOMPRESSED_1, V_MODE_2);
  p_modes["1024x768 YUV422"]  = Mode( MODE_1024x768_YUV422,
                                      COLOR_FORMAT7_YUV422, 0, 2.0,
                                      1024, 768,
                                      FORMAT_SVGA_NONCOMPRESSED_1, V_MODE_3);
  p_modes["1024x768 RGB"]     = Mode( MODE_1024x768_RGB,
                                      COLOR_FORMAT7_RGB8, 3, 3.0,
                                      1024, 768,
                                      FORMAT_SVGA_NONCOMPRESSED_1, V_MODE_4);
  p_modes["1024x768 MONO"]    = Mode( MODE_1024x768_MONO,
                                      COLOR_FORMAT7_MONO8, 1, 1.0,
                                      1024, 768,
                                      FORMAT_SVGA_NONCOMPRESSED_1, V_MODE_5);
  p_modes["800x600 MONO16"]   = Mode( MODE_800x600_MONO16,
                                      COLOR_FORMAT7_MONO16, 1, 2.0,
                                      800, 600,
                                      FORMAT_SVGA_NONCOMPRESSED_1, V_MODE_6);
  p_modes["1024x768 MONO16"]  = Mode( MODE_1024x768_MONO16,
                                      COLOR_FORMAT7_MONO16, 1, 2.0,
                                      1024, 768,
                                      FORMAT_SVGA_NONCOMPRESSED_1, V_MODE_7);
  p_modes["1280x960 YUV422"]  = Mode( MODE_1280x960_YUV422,
                                      COLOR_FORMAT7_YUV422, 0, 2.0,
                                      1280, 960,
                                      FORMAT_SVGA_NONCOMPRESSED_2, V_MODE_0);
  p_modes["1280x960 RGB"]     = Mode( MODE_1280x960_RGB,
                                      COLOR_FORMAT7_RGB8, 3, 3.0,
                                      1280, 960,
                                      FORMAT_SVGA_NONCOMPRESSED_2, V_MODE_1);
  p_modes["1280x960 MONO"]    = Mode( MODE_1280x960_MONO,
                                      COLOR_FORMAT7_MONO8, 1, 1.0,
                                      1280, 960,
                                      FORMAT_SVGA_NONCOMPRESSED_2, V_MODE_2);
  p_modes["1600x1200 YUV422"] = Mode( MODE_1600x1200_YUV422,
                                      COLOR_FORMAT7_YUV422, 0, 2.0,
                                      1600, 1200,
                                      FORMAT_SVGA_NONCOMPRESSED_2, V_MODE_3);
  p_modes["1600x1200 RGB"]    = Mode( MODE_1600x1200_RGB,
                                      COLOR_FORMAT7_RGB8, 3, 3.0,
                                      1600, 1200,
                                      FORMAT_SVGA_NONCOMPRESSED_2, V_MODE_4);
  p_modes["1600x1200 MONO"]   = Mode( MODE_1600x1200_MONO,
                                      COLOR_FORMAT7_MONO8, 1, 1.0,
                                      1600, 1200,
                                      FORMAT_SVGA_NONCOMPRESSED_2, V_MODE_5);
  p_modes["1280x960 MONO16"]  = Mode( MODE_1280x960_MONO16,
                                      COLOR_FORMAT7_MONO16, 1, 2.0,
                                      1280, 960,
                                      FORMAT_SVGA_NONCOMPRESSED_2, V_MODE_6);
  p_modes["1600x1200 MONO16"] = Mode( MODE_1600x1200_MONO16,
                                      COLOR_FORMAT7_MONO16, 1, 2.0,
                                      1600, 1200,
                                      FORMAT_SVGA_NONCOMPRESSED_2, V_MODE_7);
  p_modes["Format6"]          = Mode( MODE_EXIF,
                                      COLOR_FORMAT7_MONO8, 1, 1.0,
                                      0, 0,
                                      FORMAT_STILL_IMAGE, V_MODE_0);
  p_modes["Format7Mode0 MONO"]   = Mode( MODE_FORMAT7_0,
                                         COLOR_FORMAT7_MONO8, 1, 1.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_0,
                                         V_COLOR_0);
  p_modes["Format7Mode0 YUV411"] = Mode( MODE_FORMAT7_0,
                                         COLOR_FORMAT7_YUV411, 0, 1.5,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_0,
                                         V_COLOR_1);
  p_modes["Format7Mode0 YUV422"] = Mode( MODE_FORMAT7_0,
                                         COLOR_FORMAT7_YUV422, 0, 2.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_0,
                                         V_COLOR_2);
  p_modes["Format7Mode0 YUV444"] = Mode( MODE_FORMAT7_0,
                                         COLOR_FORMAT7_YUV444, 0, 3.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_0,
                                         V_COLOR_3);
  p_modes["Format7Mode0 RGB"]    = Mode( MODE_FORMAT7_0,
                                         COLOR_FORMAT7_RGB8, 3, 3.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_0,
                                         V_COLOR_4);
  p_modes["Format7Mode0 MONO16"] = Mode( MODE_FORMAT7_0,
                                         COLOR_FORMAT7_MONO16, 1, 2.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_0,
                                         V_COLOR_5);
  p_modes["Format7Mode0 RGB16"]  = Mode( MODE_FORMAT7_0,
                                         COLOR_FORMAT7_RGB16, 3, 6.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_0,
                                         V_COLOR_6);
  p_modes["Format7Mode1 MONO"]   = Mode( MODE_FORMAT7_1,
                                         COLOR_FORMAT7_MONO8, 1, 1.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_1,
                                         V_COLOR_0);
  p_modes["Format7Mode1 YUV411"] = Mode( MODE_FORMAT7_1,
                                         COLOR_FORMAT7_YUV411, 0, 1.5,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_1,
                                         V_COLOR_1);
  p_modes["Format7Mode1 YUV422"] = Mode( MODE_FORMAT7_1,
                                         COLOR_FORMAT7_YUV422, 0, 2.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_1,
                                         V_COLOR_2);
  p_modes["Format7Mode1 YUV444"] = Mode( MODE_FORMAT7_1,
                                         COLOR_FORMAT7_YUV444, 0, 3.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_1,
                                         V_COLOR_3);
  p_modes["Format7Mode1 RGB"]    = Mode( MODE_FORMAT7_1,
                                         COLOR_FORMAT7_RGB8, 3, 3.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_1,
                                         V_COLOR_4);
  p_modes["Format7Mode1 MONO16"] = Mode( MODE_FORMAT7_1,
                                         COLOR_FORMAT7_MONO16, 1, 2.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_1,
                                         V_COLOR_5);
  p_modes["Format7Mode1 RGB16"]  = Mode( MODE_FORMAT7_1,
                                         COLOR_FORMAT7_RGB16, 3, 6.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_1,
                                         V_COLOR_6);
  p_modes["Format7Mode2 MONO"]   = Mode( MODE_FORMAT7_2,
                                         COLOR_FORMAT7_MONO8, 1, 1.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_2,
                                         V_COLOR_0);
  p_modes["Format7Mode2 YUV411"] = Mode( MODE_FORMAT7_2,
                                         COLOR_FORMAT7_YUV411, 0, 1.5,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_2,
                                         V_COLOR_1);
  p_modes["Format7Mode2 YUV422"] = Mode( MODE_FORMAT7_2,
                                         COLOR_FORMAT7_YUV422, 0, 2.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_2,
                                         V_COLOR_2);
  p_modes["Format7Mode2 YUV444"] = Mode( MODE_FORMAT7_2,
                                         COLOR_FORMAT7_YUV444, 0, 3.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_2,
                                         V_COLOR_3);
  p_modes["Format7Mode2 RGB"]    = Mode( MODE_FORMAT7_2,
                                         COLOR_FORMAT7_RGB8, 3, 3.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_2,
                                         V_COLOR_4);
  p_modes["Format7Mode2 MONO16"] = Mode( MODE_FORMAT7_2,
                                         COLOR_FORMAT7_MONO16, 1, 2.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_2,
                                         V_COLOR_5);
  p_modes["Format7Mode2 RGB16"]  = Mode( MODE_FORMAT7_2,
                                         COLOR_FORMAT7_RGB16, 3, 6.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_2,
                                         V_COLOR_6);
  p_modes["Format7Mode3 MONO"]   = Mode( MODE_FORMAT7_3,
                                         COLOR_FORMAT7_MONO8, 1, 1.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_3,
                                         V_COLOR_0);
  p_modes["Format7Mode3 YUV411"] = Mode( MODE_FORMAT7_3,
                                         COLOR_FORMAT7_YUV411, 0, 1.5,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_3,
                                         V_COLOR_1);
  p_modes["Format7Mode3 YUV422"] = Mode( MODE_FORMAT7_3,
                                         COLOR_FORMAT7_YUV422, 0, 2.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_3,
                                         V_COLOR_2);
  p_modes["Format7Mode3 YUV444"] = Mode( MODE_FORMAT7_3,
                                         COLOR_FORMAT7_YUV444, 0, 3.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_3,
                                         V_COLOR_3);
  p_modes["Format7Mode3 RGB"]    = Mode( MODE_FORMAT7_3,
                                         COLOR_FORMAT7_RGB8, 3, 3.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_3,
                                         V_COLOR_4);
  p_modes["Format7Mode3 MONO16"] = Mode( MODE_FORMAT7_3,
                                         COLOR_FORMAT7_MONO16, 1, 2.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_3,
                                         V_COLOR_5);
  p_modes["Format7Mode3 RGB16"]  = Mode( MODE_FORMAT7_3,
                                         COLOR_FORMAT7_RGB16, 3, 6.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_3,
                                         V_COLOR_6);
  p_modes["Format7Mode4 MONO"]   = Mode( MODE_FORMAT7_4,
                                         COLOR_FORMAT7_MONO8, 1, 1.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_4,
                                         V_COLOR_0);
  p_modes["Format7Mode4 YUV411"] = Mode( MODE_FORMAT7_4,
                                         COLOR_FORMAT7_YUV411, 0, 1.5,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_4,
                                         V_COLOR_1);
  p_modes["Format7Mode4 YUV422"] = Mode( MODE_FORMAT7_4,
                                         COLOR_FORMAT7_YUV422, 0, 2.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_4,
                                         V_COLOR_2);
  p_modes["Format7Mode4 YUV444"] = Mode( MODE_FORMAT7_4,
                                         COLOR_FORMAT7_YUV444, 0, 3.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_4,
                                         V_COLOR_3);
  p_modes["Format7Mode4 RGB"]    = Mode( MODE_FORMAT7_4,
                                         COLOR_FORMAT7_RGB8, 3, 3.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_4,
                                         V_COLOR_4);
  p_modes["Format7Mode4 MONO16"] = Mode( MODE_FORMAT7_4,
                                         COLOR_FORMAT7_MONO16, 1, 2.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_4,
                                         V_COLOR_5);
  p_modes["Format7Mode4 RGB16"]  = Mode( MODE_FORMAT7_4,
                                         COLOR_FORMAT7_RGB16, 3, 6.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_4,
                                         V_COLOR_6);
  p_modes["Format7Mode5 MONO"]   = Mode( MODE_FORMAT7_5,
                                         COLOR_FORMAT7_MONO8, 1, 1.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_5,
                                         V_COLOR_0);
  p_modes["Format7Mode5 YUV411"] = Mode( MODE_FORMAT7_5,
                                         COLOR_FORMAT7_YUV411, 0, 1.5,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_5,
                                         V_COLOR_1);
  p_modes["Format7Mode5 YUV422"] = Mode( MODE_FORMAT7_5,
                                         COLOR_FORMAT7_YUV422, 0, 2.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_5,
                                         V_COLOR_2);
  p_modes["Format7Mode5 YUV444"] = Mode( MODE_FORMAT7_5,
                                         COLOR_FORMAT7_YUV444, 0, 3.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_5,
                                         V_COLOR_3);
  p_modes["Format7Mode5 RGB"]    = Mode( MODE_FORMAT7_5,
                                         COLOR_FORMAT7_RGB8, 3, 3.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_5,
                                         V_COLOR_4);
  p_modes["Format7Mode5 MONO16"] = Mode( MODE_FORMAT7_5,
                                         COLOR_FORMAT7_MONO16, 1, 2.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_5,
                                         V_COLOR_5);
  p_modes["Format7Mode5 RGB16"]  = Mode( MODE_FORMAT7_5,
                                         COLOR_FORMAT7_RGB16, 3, 6.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_5,
                                         V_COLOR_6);
  p_modes["Format7Mode6 MONO"]   = Mode( MODE_FORMAT7_6,
                                         COLOR_FORMAT7_MONO8, 1, 1.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_6,
                                         V_COLOR_0);
  p_modes["Format7Mode6 YUV411"] = Mode( MODE_FORMAT7_6,
                                         COLOR_FORMAT7_YUV411, 0, 1.5,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_6,
                                         V_COLOR_1);
  p_modes["Format7Mode6 YUV422"] = Mode( MODE_FORMAT7_6,
                                         COLOR_FORMAT7_YUV422, 0, 2.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_6,
                                         V_COLOR_2);
  p_modes["Format7Mode6 YUV444"] = Mode( MODE_FORMAT7_6,
                                         COLOR_FORMAT7_YUV444, 0, 3.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_6,
                                         V_COLOR_3);
  p_modes["Format7Mode6 RGB"]    = Mode( MODE_FORMAT7_6,
                                         COLOR_FORMAT7_RGB8, 3, 3.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_6,
                                         V_COLOR_4);
  p_modes["Format7Mode6 MONO16"] = Mode( MODE_FORMAT7_6,
                                         COLOR_FORMAT7_MONO16, 1, 2.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_6,
                                         V_COLOR_5);
  p_modes["Format7Mode6 RGB16"]  = Mode( MODE_FORMAT7_6,
                                         COLOR_FORMAT7_RGB16, 3, 6.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_6,
                                         V_COLOR_6);
  p_modes["Format7Mode7 MONO"]   = Mode( MODE_FORMAT7_7,
                                         COLOR_FORMAT7_MONO8, 1, 1.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_7,
                                         V_COLOR_0);
  p_modes["Format7Mode7 YUV411"] = Mode( MODE_FORMAT7_7,
                                         COLOR_FORMAT7_YUV411, 0, 1.5,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_7,
                                         V_COLOR_1);
  p_modes["Format7Mode7 YUV422"] = Mode( MODE_FORMAT7_7,
                                         COLOR_FORMAT7_YUV422, 0, 2.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_7,
                                         V_COLOR_2);
  p_modes["Format7Mode7 YUV444"] = Mode( MODE_FORMAT7_7,
                                         COLOR_FORMAT7_YUV444, 0, 3.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_7,
                                         V_COLOR_3);
  p_modes["Format7Mode7 RGB"]    = Mode( MODE_FORMAT7_7,
                                         COLOR_FORMAT7_RGB8, 3, 3.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_7,
                                         V_COLOR_4);
  p_modes["Format7Mode7 MONO16"] = Mode( MODE_FORMAT7_7,
                                         COLOR_FORMAT7_MONO16, 1, 2.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_7,
                                         V_COLOR_5);
  p_modes["Format7Mode7 RGB16"]  = Mode( MODE_FORMAT7_7,
                                         COLOR_FORMAT7_RGB16, 3, 6.0,
                                         0, 0,
                                         FORMAT_SCALABLE_IMAGE_SIZE, V_MODE_7,
                                         V_COLOR_6);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCam::initTriggermodeMap()
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::FireCam::initTriggermodeMap()
{
  p_triggerModes[0] = TRIGGER_MODE_0;
  p_triggerModes[1] = TRIGGER_MODE_1;
  p_triggerModes[2] = TRIGGER_MODE_2;
  p_triggerModes[3] = TRIGGER_MODE_3;
}                               
                                
/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCam::dmaThreadStartFunction( void*)
 *  ==> see headerfile
 *=======================================================================*/
void*
liblmbcam::FireCam::dmaThreadStartFunction(void* camera)
{
  struct timeval currentTime;
  try
  {
    FireCam* cam=(FireCam*)camera;
      
    while( cam->p_cameraIsRunning &&
           cam->p_writtenFrameCount < cam->p_maxReadFrames)
    {
      if( cam->p_writtenFrameCount == cam->p_lockedFrame)
      {
        cam->ringBufferOverflow();
      }
      
      if( cam->p_writtenFrameCount - cam->p_readFrameCount >
          (long) cam->p_numBufferedFrames - 3)
      {
        cam->ringBufferCritical();
      }
    
      pthread_cond_broadcast( &(cam->p_frameStartCond));

      cam->p_grabberStatus = READING;

      cam->p_iso_handler_pos = 
          cam->_vBufFrameStart[cam->p_writtenFrameCount %
                               cam->p_numBufferedFrames];

      cam->frameReceiveStarted( cam->p_writtenFrameCount);

      gettimeofday( &currentTime, NULL);
    
      cam->p_Starttimes[cam->p_writtenFrameCount
                       %cam->p_numBufferedFrames] = currentTime;
      
      if (dc1394_dma_single_capture( &(cam->p_camera)) != DC1394_SUCCESS)
      {
        cam->_packetLostInDMA = true;
	pthread_cond_broadcast( &(cam->p_frameFinishCond));
        return NULL;
      }
         
      cam->p_Filltimes[cam->p_writtenFrameCount
                       %cam->p_numBufferedFrames] = cam->p_camera.filltime;
 
      memcpy( cam->p_iso_handler_pos, cam->p_camera.capture_buffer,
              (unsigned int) ( cam->width() * cam->height() *
                               cam->bytePerPixel()));

      if (dc1394_dma_done_with_buffer( &(cam->p_camera)) != DC1394_SUCCESS)
      {
        cam->_packetLostInDMA = true;
	pthread_cond_broadcast( &(cam->p_frameFinishCond));
        return NULL;
      }    
      
      cam->frameReceiveFinished( cam->p_writtenFrameCount);
      ++(cam->p_writtenFrameCount);
      pthread_cond_broadcast( &(cam->p_frameFinishCond));

      cam->p_grabberStatus = COMPLETE;      

    }
  }
  catch( FireCamError& error)
  {
    std::cerr << error.str() << std::endl;
  }

  return NULL;
}



/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCam::grabberThreadStartFunction( void*)
 *  ==> see headerfile
 *=======================================================================*/
void*
liblmbcam::FireCam::grabberThreadStartFunction(void* camera)
{
  try
  {
    FireCam* cam=(FireCam*)camera;
  
    if( raw1394_set_iso_handler( cam->p_handle,
                                 cam->p_channel,
                                 cam->IsoHandler) < 0)
    {
      FireCamError error( "could not register iso handler");
      LMBErrorHandler::ErrorHandler()->handle( error);
    }
    else if( raw1394_start_iso_rcv( cam->p_handle,
                                    cam->p_channel) < 0)
    {
      raw1394_set_iso_handler( cam->p_handle,
                               cam->p_channel, NULL);
      FireCamError error( "could not start iso receive");
      LMBErrorHandler::ErrorHandler()->handle( error);
    }
    else
    {
      while( cam->p_cameraIsRunning &&
             cam->p_writtenFrameCount < cam->p_maxReadFrames)
      {
        MutexLocker locker( cam->p_libRawMutex);
        raw1394_loop_iterate( cam->p_handle);
      }
      
      raw1394_stop_iso_rcv( cam->p_handle,
                            cam->p_channel);
      raw1394_set_iso_handler( cam->p_handle,
                               cam->p_channel, NULL);
    }
  }
  catch( FireCamError& error)
  {
    std::cerr << error.str() << std::endl;
  }
  
  return NULL;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCam::IsoHandler( raw1394handle_t handle,
 *                                               int channel,
 *                                               size_t length,
 *                                               quadlet_t* data)
 *  ==> see headerfile
 *=======================================================================*/
int 
liblmbcam::FireCam::IsoHandler( raw1394handle_t handle, int channel,
                                size_t length, quadlet_t* data)
{
  FireCam* camera = (FireCam*) (FireCamBusRegistry::instance()->
                                bus(handle)->cameraByIndex(channel));
  
  struct timeval currentTime;
   
  if( *data & 0x01000000UL)
  {
    /*---------------------------------------------------------------------
     *  Check for buffer overflow
     *---------------------------------------------------------------------*/
    gettimeofday( &currentTime, NULL);   
    camera->p_Starttimes[camera->p_writtenFrameCount
                       %camera->p_numBufferedFrames] = currentTime;
 
    if( camera->p_writtenFrameCount == camera->p_lockedFrame)
    {
      camera->ringBufferOverflow();
    }
      
    if( camera->p_writtenFrameCount - camera->p_readFrameCount 
        > (long) camera->p_numBufferedFrames - 3)
    {
      camera->ringBufferCritical();
    }
    
    pthread_cond_broadcast( &(camera->p_frameStartCond));

    camera->p_grabberStatus = READING;

    camera->p_iso_handler_start = 
        camera->p_iso_handler_pos = 
        camera->_vBufFrameStart[camera->p_writtenFrameCount %
                                camera->p_numBufferedFrames];

    camera->frameReceiveStarted(camera->p_writtenFrameCount);
    
    memcpy( camera->p_iso_handler_pos, (unsigned char*)(data+1),
            4 * camera->p_camera.quadlets_per_packet);
    
    camera->p_iso_handler_pos += 4 * camera->p_camera.quadlets_per_packet;

  }
  else if( camera->p_grabberStatus == READING)
  {
    int copy_n_bytes;

    if(camera->p_iso_handler_pos + (4 * camera->p_camera.quadlets_per_packet) 
       >= camera->p_iso_handler_start + (4 * camera->p_camera.quadlets_per_frame))
    {
//       copy_n_bytes = 4 * camera->p_camera.quadlets_per_frame -
//           ( camera->p_iso_handler_pos - camera->p_iso_handler_start);
       copy_n_bytes =  (int)(camera->width() * camera->height() *  camera->bytePerPixel()) -
           ( camera->p_iso_handler_pos - camera->p_iso_handler_start);

      camera->p_grabberStatus = COMPLETE;  
 
      gettimeofday( &currentTime, NULL);
      camera->p_Filltimes[camera->p_writtenFrameCount
                       %camera->p_numBufferedFrames] = currentTime;
    }
    else
    {
      copy_n_bytes = 4 * camera->p_camera.quadlets_per_packet;
    }
   
    /*---------------------------------------------------------------------
     *  Copy calculated amount of data 
     *---------------------------------------------------------------------*/
    memcpy(camera->p_iso_handler_pos, (unsigned char*)(data+1), copy_n_bytes);
    
    camera->p_iso_handler_pos += copy_n_bytes;

    if( camera->p_grabberStatus == COMPLETE)
    {
      camera->frameReceiveFinished( camera->p_writtenFrameCount);
      ++(camera->p_writtenFrameCount);
      pthread_cond_broadcast( &(camera->p_frameFinishCond));
    }
    
  }

  return 1;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION: initSequenceFeature(char count, bool autorewind)
 *  ==> see headerfile
 *=======================================================================*/
bool liblmbcam::FireCam::initSequenceFeature(char count, bool autorewind)
{
  quadlet_t buffer = 0;
  int err = 0;

  /*-----------------------------------------------------------------------
   * Check if sequence-feature is enabled
   *-----------------------------------------------------------------------*/
  err = raw1394_read(handle(), 0xffc0 | node(),
                     AVTReg_Adv_Inq_1, 4, &buffer);
  usleep(1000);

// #ifdef LIBLMBCAMDEBUG
//   std::cerr << "Buffer: " << std::hex << ntohl(buffer) << std::endl; 
// #endif

  if ((err != 0) || ((ntohl(buffer) & 0x04000000UL) == 0)){
    std::cerr << "Error: Sequence feature not supported by camera.\n";
    return false;
  }

  supports_sequence_feature = true;

  if (p_cameraIsRunning)
  {
    std::cerr << "Camera mustn't run when initialising sequence feature!\n";
    return false;
  }
  
  stopSequenceFeature();
  
//Todo: Schreiben des Vendor-Strings!

#ifdef LIBLMBCAMDEBUG
   std::cerr << "Initialising sequence feature" << std::endl; 
#endif


  setupCapture();

  err = raw1394_read(handle(), 0xffc0 | node(),
                     AVTReg_Sequence_Ctrl, 4, &buffer);
  usleep(1000);
  if (err != 0){
    std::cerr << "Error reading raw1394: " << std::hex << ntohl(buffer) << "\n";
    return false;
  }

  char maxLength = (ntohl(buffer) & 0xff00) >> 8;
  if (count > maxLength)
  {
    std::cerr << "Count parameter is bigger than " << (int) maxLength << ".\n";
    return false;
  }

  /*-----------------------------------------------------------------------
   * Set ON/OFF bit in Sequence_Ctrl-Register on and set
   * the length of the sequence
   *-----------------------------------------------------------------------*/
  buffer = 0x00000000UL | (1 << 25) | (autorewind << 26);
  buffer = ntohl(buffer);
  err = raw1394_write(handle(), 0xffc0 | node(),
                      AVTReg_Sequence_Ctrl, 4, &buffer);
  usleep(1000);
  if (err != 0){
    std::cerr << "Error writing raw1394 to AVT register Sequence_Ctrl: " 
              << std::hex << htonl(buffer) << std::dec << "\n";
    return false;
  }

  buffer = 0x00000000UL | (1 << 25) | (autorewind << 26) | count;
  buffer = ntohl(buffer);
  err = raw1394_write(handle(), 0xffc0 | node(),
                      AVTReg_Sequence_Ctrl, 4, &buffer);
  usleep(1000);
  if (err != 0){
    std::cerr << "Error writing raw1394 to AVT register Sequence_Ctrl: " 
              << std::hex << htonl(buffer) << std::dec << "\n";
    return false;
  }

  buffer = 0;
  err = raw1394_write(handle(), 0xffc0 | node(),
                      AVTReg_Sequence_Param, 4, &buffer);
  usleep(1000);

  if (err != 0){
    std::cerr << "Error writing raw1394 to AVT register Sequence_Ctrl: " 
              << std::hex << htonl(buffer) << std::dec << "\n";
    return false;
  }

  sequence_feature_running = true;
  
  return true;

}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION: stopSequenceFeature()
 *  ==> see headerfile
 *=======================================================================*/
bool liblmbcam::FireCam::stopSequenceFeature()
{
  if (sequence_feature_running)
  {
    /*****************************************************
     * Switch off sequence feature
     ****************************************************/
    quadlet_t buffer = 0x00000000UL;
    int err = 0;

#ifdef LIBLMBCAMDEBUG
   std::cerr << "Stopping sequence feature" << std::endl; 
#endif

    err = raw1394_write(handle(), 0xffc0 | node(),
                        AVTReg_Sequence_Ctrl, 4, &buffer);
    usleep(100000);
    if (err != 0){
      std::cerr << "Error writing raw1394: " << std::hex << htonl(buffer) << std::dec << "\n";
      return false;
    }

    sequence_feature_running = false;
  }
  
  return true;
}


/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
bool liblmbcam::FireCam::selectSequenceRegister(char position)
{
  quadlet_t buffer = 0;
  int err = 0;
  buffer = htonl(position);
  err = raw1394_write(handle(), 0xffc0 | node(),
                      AVTReg_Sequence_Param, 4, &buffer);
  usleep(1000);

  if (err != 0){
    std::cerr << "Error writing raw1394 to AVT Register Sequence_Param: " 
              << std::hex << htonl(buffer) << std::dec << "\n";
    return false;
  }
  return true;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION: applySettingsToSequenceRegister()
 *  ==> see headerfile
 *=======================================================================*/

bool liblmbcam::FireCam::applySettingsToSequenceRegister()
{
  quadlet_t buffer = 0;
  int err = 0;

  if (!supports_sequence_feature)
  {
    std::cerr << "Sequence feature is not supported by camera. Can't apply parameters.\n";
    return false;
  }

  err = raw1394_read(handle(), 0xffc0 | node(),
                     AVTReg_Sequence_Param, 4, &buffer);
  usleep(1000);
  if (err != 0){
    std::cerr << "Error reading raw1394: " << std::hex << ntohl(buffer) << std::dec << "\n";
    return false;
  }

  buffer = htonl( ntohl(buffer) | 0x04000000UL);
  
  //apply parameters, no auto-increment
  err = raw1394_write(handle(), 0xffc0 | node(),
                      AVTReg_Sequence_Param, 4, &buffer);
  usleep(1000);

  if (err != 0){
    std::cerr << "Error writing raw1394 to AVT Register Sequence_Param: " << std::hex << htonl(buffer) << std::dec << "\n";
    return false;
  }

  return true;
}


/*=========================================================================
 *  DESCRIPTION OF FUNCTION: applyParametersToSequence
 *  ==> see headerfile
 *=======================================================================*/
bool liblmbcam::FireCam::applyParametersToSequence(char position)
{
  quadlet_t buffer = 0;
  int err = 0;

  if (!supports_sequence_feature)
  {
    std::cerr << "Sequence feature is not supported by camera. Can't apply parameters.\n";
    return false;
  }

  err = raw1394_read(handle(), 0xffc0 | node(),
                     AVTReg_Sequence_Ctrl, 4, &buffer);
  usleep(1000);
  if (err != 0){
    std::cerr << "Error reading raw1394: " << std::hex << ntohl(buffer) << std::dec <<  "\n";
    return false;
  }

  /* sequence activated? */
  if ((ntohl(buffer) & 0x40000000UL == 0))
  {
    std::cerr << "Sequence feature is not activated. Can't apply parameters. \n";
    return false;
  }   

  //apply parameters, no auto-increment
  buffer = htonl(position | 0x04000000UL);
  err = raw1394_write(handle(), 0xffc0 | node(),
                      AVTReg_Sequence_Param, 4, &buffer);
  usleep(1000);

  buffer = htonl(position + 1);
  err = raw1394_write(handle(), 0xffc0 | node(),
                      AVTReg_Sequence_Param, 4, &buffer);
  usleep(1000);

  if (err != 0){
    std::cerr << "Error writing raw1394 to AVT Register Sequence_Param: " << std::hex << htonl(buffer) << std::dec << "\n";
    return false;
  }

  return true;
}


/*=========================================================================
 *  DESCRIPTION OF FUNCTION: setupCapture
 *  ==> see headerfile
 *=======================================================================*/ 
void liblmbcam::FireCam::setupCapture()
{
  if( !testSettings())
  {
    FireCamError error("invalid settings, can't start camera");
    LMBErrorHandler::ErrorHandler()->handle( error);
    return;
  }

#if CHECK_FOR_SMP
  /*-----------------------------------------------------------------------
   * Get kernel info
   *-----------------------------------------------------------------------*/
  std::cerr << "Counting CPUs..." << std::flush;
  FILE * nprocs;
  char os[80];
  int lastchar;

  nprocs = popen("grep -c '^processor[^:]*: [0-9]*$' /proc/cpuinfo", "r");
  lastchar = fread(os, 1, 80, nprocs);
  pclose(nprocs);
  os[lastchar] = '\0';

  std::cerr << "found " << os << std::endl;
  if ( atoi(os) > 1)
  {
    if (!(p_deviceName != "" && p_availableParameters["DMA"]->asInt() == 1))
    {
    /*---------------------------------------------------------------------
     *  SMP-kernel. No raw-capturing supported.
     *---------------------------------------------------------------------*/
      std::cerr << "raw-capturing not available in smp kernel." << std::endl;
      if (p_deviceName != "") 
      {
        std::cerr << " Switching to dma." << std::endl;
        p_availableParameters["DMA"]->setValue(1);
      }
      else
      {
        std::cerr << " Terminating application." << std::endl;
        cleanup();
        
        exit(0);
      }
    }
  }
#endif

  /*-----------------------------------------------------------------------
   *  DMA capture
   *-----------------------------------------------------------------------*/
  if( p_deviceName != "" && p_availableParameters["DMA"]->asInt() == 1)
  {
    if( p_currentMode.p_formatID == FORMAT_SCALABLE_IMAGE_SIZE)
    {
#ifdef LIBLMBCAMDEBUG
      std::cerr << "Setting up DMA capture in Format7:" << std::endl
                << "Format:    " << p_currentMode.p_formatID << std::endl
                << "Mode:        " << p_currentMode.p_id << " ("
                << mode() << ")" << std::endl
                << "Packet Size: " << p_format7PacketSize << std::endl
                << "Framerate:   " << framerate() << " fps" << std::endl
                << "Left:        " << p_format7Left << std::endl
                << "Top:         " << p_format7Top << std::endl
                << "Width:       " << p_format7Width << std::endl
                << "Height:      " << p_format7Height << std::endl
                << "CCR_Offset:      " << p_info.ccr_offset << std::endl;
#endif

      if( dc1394_dma_setup_format7_capture( p_handle, p_node,
                                            p_channel /*channel*/,
                                            p_currentMode.p_id,
                                            SPEED_400,
                                            p_format7PacketSize,
                                            p_format7Left,
                                            p_format7Top,
                                            p_format7Width,
                                            p_format7Height,
                                            NUM_DMA_BUFFERS, /* num dma buffers */
                                            ((p_dropFrames == true) ? 1 : 0),
                                            p_deviceName.c_str(),
                                            &p_camera) != DC1394_SUCCESS)
      {
//         FireCamError error( "unable to setup camera");
//         LMBErrorHandler::ErrorHandler()->handle( error);
//         return;

        std::ostringstream os;
        os << "unable to setup camera in DMA format 7 with following parameters:\n"
           << "p_currentMode.p_id:         " << p_currentMode.p_id << std::endl
           << "p_format7PacketSize (Byte): " <<  p_format7PacketSize << std::endl
           << "p_format7Left:              " <<  p_format7Left << std::endl
           << "p_format7Top:               " <<  p_format7Top << std::endl
           << "p_format7Width:             " <<  p_format7Width << std::endl
           << "p_format7Height:            " << p_format7Height  << std::endl
           << "DMA-Buffers:                " << NUM_DMA_BUFFERS << std::endl;
        FireCamError error( os.str());
        
        LMBErrorHandler::ErrorHandler()->handle( error);
        return;
      }
    }
    else
    {
#ifdef LIBLMBCAMDEBUG
      std::cerr << "Setting up DMA standard capture:" << std::endl
                << "Format:    " << p_currentMode.p_formatID << std::endl
                << "Mode:      " << p_currentMode.p_id << " ("
                << mode() << ")" << std::endl
                << "Framerate: " << p_currentFramerate.p_id << " ("
                << framerate() << " fps)" << std::endl
                << "Device:    " << p_deviceName << std::endl
                << "CCR_Offset:      " << p_info.ccr_offset << std::endl;


#endif
      
      if( dc1394_dma_setup_capture( p_handle, p_node, p_channel /*channel*/,
                                    p_currentMode.p_formatID,
                                    p_currentMode.p_id,
                                    SPEED_400,
                                    p_currentFramerate.p_id,
                                    NUM_DMA_BUFFERS,
                                    ((p_dropFrames == true) ? 1 : 0),
                                    p_deviceName.c_str(),
                                    &p_camera) != DC1394_SUCCESS) 
      {
//         FireCamError error( "unable to setup camera");
//         LMBErrorHandler::ErrorHandler()->handle( error);
//         return;

        std::ostringstream os;
        os << "unable to setup camera DMA standard capture"
              "with following parameters:\n"
           << "p_currentMode.p_formatID:         " << p_currentMode.p_formatID << std::endl
           << "p_currentMode.p_id:         " << p_currentMode.p_id << std::endl
           << "DMA-Buffers:                " << NUM_DMA_BUFFERS << std::endl;
       
        FireCamError error( os.str());
        
        LMBErrorHandler::ErrorHandler()->handle( error);
        return;
      }
    }
  }
  
  /*-----------------------------------------------------------------------
   *  Standard capture (non-DMA)
   *-----------------------------------------------------------------------*/
  else
  {
    if( p_currentMode.p_formatID == FORMAT_SCALABLE_IMAGE_SIZE)
    {
      
#ifdef LIBLMBCAMDEBUG
      std::cerr << "Setting up standard capture in Format7:" << std::endl
                << "Format:    " << p_currentMode.p_formatID << std::endl
                << "Mode:        " << p_currentMode.p_id << " ("
                << mode() << ")" << std::endl
                << "Packet Size: " << p_format7PacketSize << std::endl
                << "Framerate:   " << framerate() << " fps" << std::endl
                << "Top:         " << p_format7Top << std::endl
                << "Left:        " << p_format7Left << std::endl
                << "Width:       " << p_format7Width << std::endl
                << "Height:      " << p_format7Height << std::endl
                << "CCR_Offset:      " << p_info.ccr_offset << std::endl;
#endif
      
      if( dc1394_setup_format7_capture( p_handle, p_node,
                                        p_channel /*channel*/,
                                        p_currentMode.p_id,
                                        SPEED_400,
                                        p_format7PacketSize,
                                        p_format7Left,
                                        p_format7Top,
                                        p_format7Width,
                                        p_format7Height,
                                        &p_camera) != DC1394_SUCCESS)
      {
        std::ostringstream os;
        os << "unable to setup camera format 7 with following parameters:\n"
           << "p_currentMode.p_id:         " << p_currentMode.p_id << std::endl
           << "p_format7PacketSize (Byte): " <<  p_format7PacketSize << std::endl
           << "p_format7Left:              " <<  p_format7Left << std::endl
           << "p_format7Top:               " <<  p_format7Top << std::endl
           << "p_format7Width:             " <<  p_format7Width << std::endl
           << "p_format7Height:             " << p_format7Height  << std::endl;
        FireCamError error( os.str());
        
        LMBErrorHandler::ErrorHandler()->handle( error);
        return;
      }
    }
    else
    {
#ifdef LIBLMBCAMDEBUG
      std::cerr << "Setting up standard capture:" << std::endl
                << "Format:    " << p_currentMode.p_formatID << std::endl
                << "Mode:      " << p_currentMode.p_id << " ("
                << mode() << ")" << std::endl
                << "Framerate: " << p_currentFramerate.p_id << " ("
                << framerate() << " fps)" << std::endl
                << "CCR_Offset:      " << p_info.ccr_offset << std::endl;
#endif
      
      if( dc1394_setup_capture( p_handle, p_node,
                                p_channel, /* channel */ 
                                p_currentMode.p_formatID,
                                p_currentMode.p_id,
                                SPEED_400,
                                p_currentFramerate.p_id,
                                &p_camera) != DC1394_SUCCESS) 
      {
        FireCamError error( "unable to setup camera");
        LMBErrorHandler::ErrorHandler()->handle( error);
        return;
      }
    }
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION: getFrameFilltime
 *  ==> see headerfile
 *=======================================================================*/ 
struct timeval liblmbcam::FireCam::getFrameFilltime(long number)
{
  if(  number < 0  
       ||number < p_writtenFrameCount - (long)p_numBufferedFrames )
  {
    return timeval();
  }
  else
  {
    return p_Filltimes[number%p_numBufferedFrames];
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
dc1394_cameracapture  liblmbcam::FireCam::getCameraCapture()
{
  return p_camera;
}
  


/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/ 
struct timeval liblmbcam::FireCam::getFrameStarttime(long number)
{ 
  /*-----------------------------------------------------------------------
   *  Only times inside buffer-window are saved. 
   *-----------------------------------------------------------------------*/
  if(  number < 0 || 
       number < p_writtenFrameCount - (long)p_numBufferedFrames)
  {
    return timeval();
  }
  else
  {
    return p_Starttimes[number%p_numBufferedFrames];
  }  
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
std::vector<timeval> liblmbcam::FireCam::measureRelativeFrameStarttimes(int no)
{
  std::vector<timeval> vResult;
  
  timeval start;
  timeval tmp1, tmp2;
  gettimeofday(&start, 0);
  

  startCameraRunNFrames(no);
  int nPacketsPerFrame = p_camera.quadlets_per_frame 
    / p_camera.quadlets_per_packet;  
#ifdef LIBLMBCAMDEBUG
  std::cerr << "Measuring camera frame duration....Packets per Frame:" 
	    << nPacketsPerFrame << std::endl;

#endif
  for (int i = 0; i < no; ++i)
  {
    waitForFrameFinished(i);
    tmp1 = getFrameFilltime(i);
    timersub(&tmp1, &start, &tmp2);
    vResult.push_back(tmp2);
  }
  stopCamera();

  /*---------------------------------------------------------------------
   * Subtract frame duration from filltime  
   *---------------------------------------------------------------------*/
  timeval frameTransDuration;
  frameTransDuration.tv_usec = nPacketsPerFrame * 125;
  frameTransDuration.tv_sec = 0;
  

  for (unsigned int i = 0; i < vResult.size(); ++i)
  {
       timersub(&vResult[i], &frameTransDuration, &vResult[i]);
       assert (vResult[i].tv_sec >= 0 && vResult[i].tv_usec >= 0);
  }

#ifdef LIBLMBCAMDEBUG
  std::cerr << "...measuring done." << std::endl;
#endif
  
  return vResult;
  
}
