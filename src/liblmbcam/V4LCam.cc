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

#include "V4LCam.hh"

#include "V4LCamParam.hh"
#include "V4LCamBus.hh"

#include <cstdio>
#include <cstdlib>
#include <cerrno>
#include <cstring>
#include <cmath>
#include <fcntl.h>
#include <cassert>
#include <sys/mman.h>

#include <libv4l2.h>
#include <linux/videodev2.h>

/*-------------------------------------------------------------------------
 *  Initialize static members
 *-------------------------------------------------------------------------*/
std::map<std::string,liblmbcam::V4LCam::ColorCoding>
liblmbcam::V4LCam::p_colorCodings;

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:V4LCam::V4LCam( raw1394handle_t handle,
 *                                            nodeid_t node)
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::V4LCam::V4LCam( const std::string& device, const std::string& vendor,
                           const std::string& model, const std::string& guid)
        : p_dev(-1)
        , p_device( device)
        , p_vendor( vendor)
        , p_model(   model)
        , p_guid(     guid)
        , p_buffer( 0)
        , p_nBuffers( 0)
        , p_ringBuffer( 0)
        , p_numBufferedFrames( 8)
        , p_dropFrames( false)
        , p_lockedFrame( -1)
        , p_grabberStatus( INIT)
{
  if( !p_colorCodings.size())
      initColorCodingMap();
  
  deviceOpen();
  deviceQueryCaps();
  initAvailableModes();
  initAvailableParameters();
  readSettingsFromCamera();
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:V4LCam::~V4LCam()
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::V4LCam::~V4LCam()
{
  stopCamera();

  for( std::map<std::string,LMBCamParam*>::iterator it
           = p_availableParameters.begin();
       it != p_availableParameters.end(); ++it)
  {
    delete it->second;
  }
  p_availableParameters.clear();
  
  deviceClose();
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::V4LCam::deviceOpen()
{
  int max_try=5;
  //try to open the video device
  while ((max_try) && (p_dev==-1)) 
  {
    MutexLocker ml( &p_devMutex);
    p_dev = v4l2_open( p_device.c_str(), O_RDWR, 0);
    if( p_dev == -1) 
    {
      if( !--max_try) 
      {
        V4LCamError error( "Can't open device");
        LMBErrorHandler::ErrorHandler()->handle( error);
      }
      sleep(1);
    }
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::V4LCam::deviceClose()
{
  //close device
  if( p_dev< 0) return;
  
  MutexLocker ml( &p_devMutex);
  if( -1 == v4l2_close(p_dev))
  {
    V4LCamError error;
    error << "Can't close device " << p_device << ": " << strerror (errno);
    LMBErrorHandler::ErrorHandler()->handle( error);
  }
  
  p_dev = -1;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::V4LCam::deviceQueryCaps()
{
  v4l2_capability vid_caps = {0};
  if( xioctl( VIDIOC_QUERYCAP, &vid_caps) == -1) 
  {
    V4LCamError error( "VIDIOC_QUERYCAP failed");
    LMBErrorHandler::ErrorHandler()->handle( error);
  }

  //check if device supports capturing
  if ((vid_caps.capabilities & V4L2_CAP_VIDEO_CAPTURE)==0)
  {
    V4LCamError error( "Video device does NOT support capturing");
    LMBErrorHandler::ErrorHandler()->handle( error);
  }
  if (!(vid_caps.capabilities & V4L2_CAP_STREAMING))
  {
    V4LCamError error( "Video device does NOT support streaming i/o");
    LMBErrorHandler::ErrorHandler()->handle( error);
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:V4LCam::param( const std::string& paramStr)
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::LMBCamParam*
liblmbcam::V4LCam::param( const std::string& paramStr)
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
 *  DESCRIPTION OF FUNCTION:V4LCam::startCamera()
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::V4LCam::startCamera()
{
  if( p_cameraIsRunning) return;
  
  setupCapture();
  initBuffer();
  
  p_readFrameCount    = 0;
  p_writtenFrameCount = 0;
  p_grabberStatus     = INIT;
  p_cameraIsRunning   = true;

  pthread_create( &p_grabberThread, NULL, grabberThreadStartFunctionMmap, this);
}
 
/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/ 
void liblmbcam::V4LCam::setExternalRingBuffer(std::vector<unsigned char*> vRingBuffer)
{
#ifdef LIBLMBCAMDEBUG
  std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif

  _vExternalRingBuffer = vRingBuffer;
  p_numBufferedFrames = vRingBuffer.size();
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:V4LCam::stopCamera()
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::V4LCam::stopCamera()
{
  if( p_cameraIsRunning)
  {
    /*---------------------------------------------------------------------
     *  tell thread to finish
     *---------------------------------------------------------------------*/
    p_cameraIsRunning = false;
    pthread_join( p_grabberThread, NULL);
    
    /*---------------------------------------------------------------------
     *  Stop data transmission from camera
     *---------------------------------------------------------------------*/
    v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl( VIDIOC_STREAMOFF, &type))
    {
      V4LCamError error( "VIDIOC_STREAMOFF failed");
      LMBErrorHandler::ErrorHandler()->handle( error);
    }

    freeBuffer();
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:V4LCam::freeBuffer()
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::V4LCam::freeBuffer()
{
  freeBufferMmap();
  freeRingBuffer();
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:V4LCam::freeBufferMmap()
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::V4LCam::freeBufferMmap()
{
  if( p_buffer)
  {
    for (size_t i = 0; i < p_nBuffers; ++i)
    {
      MutexLocker ml( &p_devMutex);
      if( -1 == v4l2_munmap( p_buffer[i].start, p_buffer[i].length))
      {
        V4LCamError error( "munmap failed");
        LMBErrorHandler::ErrorHandler()->handle( error);
      }
    }
    delete[] p_buffer;
    p_buffer   = 0;
    p_nBuffers = 0;
  }
 }

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:V4LCam::freeRingBuffer()
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::V4LCam::freeRingBuffer()
{
  if( p_ringBuffer)
  {
    delete[] p_ringBuffer;
    p_ringBuffer = 0;
  }
  _vExternalRingBuffer.clear();
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:V4LCam::setMode( const std::string& mode)
 *  ==> see headerfile
 *=======================================================================*/
bool
liblmbcam::V4LCam::setMode( const std::string& mode)
{
  stopCamera();
  bool ret = false;
  std::map<std::string,Mode>::const_iterator it = p_modes.find( mode);
  if( it == p_modes.end())
  {
    V4LCamError error;
    error << mode << " is not a valid mode";
    LMBErrorHandler::ErrorHandler()->handle( error);
  }
  else
  {
    /*-------------------------------------------------------------------
     *  Set mode if everything is ok
     *-------------------------------------------------------------------*/
    p_currentMode = it->second;
    ret = true;
  }

  return ret;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:V4LCam::mode() const
 *  ==> see headerfile
 *=======================================================================*/
std::string
liblmbcam::V4LCam::mode() const
{
  std::string ret;
  
  /*-----------------------------------------------------------------------
   *  Look up current mode name in p_modes
   *-----------------------------------------------------------------------*/
  for( std::map<std::string,Mode>::const_iterator it = p_modes.begin();
       it != p_modes.end(); ++it)
  {
    if( it->second.id == p_currentMode.id)
    {
      ret = it->first;
      break;
    }
  }

  return ret;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:V4LCam::setFramerate( double framerate,
 *                                                 bool allowRounding)
 *  ==> see headerfile
 *=======================================================================*/
bool
liblmbcam::V4LCam::setFramerate( double framerate, bool allowRounding)
{
  stopCamera();

  int index = -1;
  double val = framerate;
  for( size_t i=0; i < p_currentMode.framerates.size(); ++i)
  {
    double current = std::fabs( (p_currentMode.framerates[i].denominator /
                                 p_currentMode.framerates[i].numerator) -
                                framerate);
    if(  current < val)
    {
      val = current;
      index = i;
    }
  }
  
  if( index < 0)
  {
    V4LCamError error;
    error << "Could not find framerate " << framerate;
    LMBErrorHandler::ErrorHandler()->handle( error);
    return false;
  }
  
  
  if( val < 1)
  {
    p_currentFramerate = p_currentMode.framerates[index];
  }
  else if( allowRounding)
  {
    p_currentFramerate = p_currentMode.framerates[index];
  }
  else
  {
    V4LCamError error;
    error << "Could not set framerate " << framerate;
    LMBErrorHandler::ErrorHandler()->handle( error);
    return false;
  }
  
  return true;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:V4LCam::framerate() const
 *  ==> see headerfile
 *=======================================================================*/
double
liblmbcam::V4LCam::framerate() const
{
  return double(p_currentFramerate.denominator)/p_currentFramerate.numerator;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:V4LCam::setColorCoding( const std::string& colorCoding)
 *  ==> see headerfile
 *=======================================================================*/
bool
liblmbcam::V4LCam::setColorCoding( const std::string& colorCoding)
{
  std::string tmp = mode();
  std::stringstream modeStream;
  modeStream << tmp.substr( 0, tmp.find( " ")) << " " << colorCoding;
   
  return setMode(modeStream.str());
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:V4LCam::initAvailableParameters()
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::V4LCam::initAvailableParameters()
{
  v4l2_queryctrl queryctrl={0};
  for (queryctrl.id = V4L2_CID_BASE;
       queryctrl.id < V4L2_CID_LASTP1;
       queryctrl.id++)
  {
    if (0 == xioctl( VIDIOC_QUERYCTRL, &queryctrl))
    {
      if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) continue;
      std::string name = reinterpret_cast<const char*>(queryctrl.name);
      switch( queryctrl.type)
      {
      case V4L2_CTRL_TYPE_INTEGER:
        p_availableParameters[name] =
            new V4LCamParamIntRange( this, name, queryctrl);
        break;
      case V4L2_CTRL_TYPE_BOOLEAN:
        p_availableParameters[name] =
            new V4LCamParamSwitch( this, name, queryctrl);
        break;
      default:
        p_availableParameters[name] =
            new V4LCamParamIntRange( this, name, queryctrl);
        break;
      }
    }
    else
    {
      if (errno == EINVAL) continue;
      V4LCamError error( "VIDIOC_QUERYCTRL failed");
      LMBErrorHandler::ErrorHandler()->handle( error);
      return;
    }
  }
  
  for( queryctrl.id = V4L2_CID_PRIVATE_BASE; ; queryctrl.id++)
  {
    if (0 == xioctl( VIDIOC_QUERYCTRL, &queryctrl))
    {
      if (queryctrl.flags & V4L2_CTRL_FLAG_DISABLED) continue;
      std::string name = reinterpret_cast<const char*>(queryctrl.name);
      switch( queryctrl.type)
      {
      case V4L2_CTRL_TYPE_INTEGER:
        p_availableParameters[name] =
            new V4LCamParamIntRange( this, name, queryctrl);
        break;
      case V4L2_CTRL_TYPE_BOOLEAN:
        p_availableParameters[name] =
            new V4LCamParamSwitch( this, name, queryctrl);
        break;
      default:
        p_availableParameters[name] =
            new V4LCamParamIntRange( this, name, queryctrl);
        break;
      }
    }
    else
    {
      if(errno == EINVAL) break;
      V4LCamError error( "VIDIOC_QUERYCTRL failed");
      LMBErrorHandler::ErrorHandler()->handle( error);
      return;
    }
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:V4LCam::initAvailableModes()
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::V4LCam::initAvailableModes()
{
  if( p_modes.size())
      p_modes.clear();
  
  v4l2_fmtdesc fmtdesc = {0};
  v4l2_frmsizeenum frmsize = {0};
 
  fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  char fourcc[5] = {0};
  size_t counter=0;
  while (0 == xioctl( VIDIOC_ENUM_FMT, &fmtdesc))
  {
    strncpy(fourcc, (char *)&fmtdesc.pixelformat, 4);
    std::pair<std::string,ColorCoding> cc;
    for( std::map<std::string,ColorCoding>::const_iterator it =
             p_colorCodings.begin(); it != p_colorCodings.end(); ++it)
    {
      if( it->second.id == fmtdesc.pixelformat)
          cc = *it;
    }
    if( cc.second.id == 0)
    {
      V4LCamError error;
      error << "Unsupported pixel format '" << fourcc << "'";
      LMBErrorHandler::ErrorHandler()->handle( error);
    }
          
    frmsize.pixel_format = fmtdesc.pixelformat;
    frmsize.index = 0;
    while( xioctl( VIDIOC_ENUM_FRAMESIZES, &frmsize) == 0)
    {
      if( frmsize.type == V4L2_FRMSIZE_TYPE_DISCRETE)
      {
        std::ostringstream oss;
        oss << frmsize.discrete.width << "x"
            << frmsize.discrete.height << " "
            << cc.first;
        Mode mode( counter, frmsize.pixel_format,
                   cc.second.nComponents, cc.second.bytePerPixel,
                   frmsize.discrete.width, frmsize.discrete.height,
                   availableFrameratesInMode( frmsize.pixel_format,
                                              frmsize.discrete.width,
                                              frmsize.discrete.height));
        p_modes[oss.str()] = mode;
        ++counter;
      }
      else if( frmsize.type == V4L2_FRMSIZE_TYPE_STEPWISE)
      {
      }
      frmsize.index++;
    }
    fmtdesc.index++;
  }
  if( errno != EINVAL)
  {
    V4LCamError error( "Error enumerating formats");
    LMBErrorHandler::ErrorHandler()->handle( error);
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
std::vector<liblmbcam::V4LCam::Framerate>
liblmbcam::V4LCam::availableFrameratesInMode(
    unsigned int pf, unsigned int width, unsigned int height)
{
  std::vector<Framerate> ret;
  
  v4l2_frmivalenum frmival = {0};
  frmival.index        = 0;
  frmival.pixel_format = pf;
  frmival.width        = width;
  frmival.height       = height;
  if( 0 == xioctl( VIDIOC_ENUM_FRAMEINTERVALS, &frmival))
  {
    if( frmival.type == V4L2_FRMIVAL_TYPE_DISCRETE)
    {
      ret.push_back( Framerate(frmival.discrete.numerator,
                               frmival.discrete.denominator));
      while( true)
      {
        frmival.index+=1;
        if( -1 == xioctl( VIDIOC_ENUM_FRAMEINTERVALS, &frmival))
        {
          if( errno == EINVAL) break;
        }
        Framerate f(frmival.discrete.numerator, frmival.discrete.denominator);
        if( std::find( ret.begin(), ret.end(), f) == ret.end())
            ret.push_back( f);
      }
    }
    else
    {
      if( frmival.type == V4L2_FRMIVAL_TYPE_CONTINUOUS)
      {
        V4LCamError error( "V4L2_FRMIVAL_TYPE_CONTINUOUS not implemented");
        LMBErrorHandler::ErrorHandler()->handle( error);
      }
      else if( frmival.type == V4L2_FRMIVAL_TYPE_STEPWISE)
      {
        V4LCamError error( "V4L2_FRMIVAL_TYPE_STEPWISE not implemented");
        LMBErrorHandler::ErrorHandler()->handle( error);
      }
    }
  }
  if( !ret.size())
      ret.push_back(Framerate(1,30));

  return ret;
}

      
/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::V4LCam::readSettingsFromCamera()
{
  v4l2_format fmt={0};
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (-1 == xioctl( VIDIOC_G_FMT, &fmt))
  {
    V4LCamError error;
    error << __PRETTY_FUNCTION__ << ": VIDIOC_G_FMT failed";
    LMBErrorHandler::ErrorHandler()->handle( error);
  }
  else
  {
    std::ostringstream oss;
    oss << fmt.fmt.pix.width << "x" << fmt.fmt.pix.height << " ";
    for( std::map<std::string,ColorCoding>::const_iterator it =
             p_colorCodings.begin(); it != p_colorCodings.end(); ++it)
    {
      if( it->second.id == fmt.fmt.pix.pixelformat)
      {
        oss << it->first;
        std::map<std::string,Mode>::const_iterator modeIt =
            p_modes.find(oss.str());
        if( modeIt != p_modes.end())
        {
          p_currentMode = modeIt->second;
          break;
        }
        else
        {
          V4LCamError error;
          error << __PRETTY_FUNCTION__
                << ": mode from camera invalid (" << oss.str() << ")";
          LMBErrorHandler::ErrorHandler()->handle( error);
        }
      }
    }
  }

  v4l2_streamparm sp={0};
  sp.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if( -1 == xioctl( VIDIOC_G_PARM, &sp))
  {
    V4LCamError error;
    error << __PRETTY_FUNCTION__ << ": VIDIOC_G_PARM failed";
    LMBErrorHandler::ErrorHandler()->handle( error);
  }
  else
  {
    Framerate f(1,30);
    if( sp.parm.capture.capability & V4L2_CAP_TIMEPERFRAME)
    {
      f.numerator   = sp.parm.capture.timeperframe.numerator;
      f.denominator = sp.parm.capture.timeperframe.denominator;
    }
    p_currentFramerate = f;
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:V4LCam::testSettings()
 *  ==> see headerfile
 *=======================================================================*/
bool
liblmbcam::V4LCam::testSettings()
{
  return true;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:V4LCam::availableFrameratesInMode(
 *                                                  const std::string& mode) const
 *  ==> see headerfile
 *=======================================================================*/
std::vector<double>
liblmbcam::V4LCam::availableFrameratesInMode( const std::string& mode) const
{
  std::vector<double> ret;

  std::map<std::string,Mode>::const_iterator it = p_modes.find(mode);
  if( it!= p_modes.end())
  {
    std::transform( it->second.framerates.begin(), it->second.framerates.end(),
                    std::back_inserter( ret), Framerate::toDouble);
  }
  return ret;
}
  
/*=========================================================================
 *  DESCRIPTION OF FUNCTION:V4LCam::initColorCodingMap()
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::V4LCam::initColorCodingMap()
{
  p_colorCodings["RGB"]    = ColorCoding(V4L2_PIX_FMT_RGB24,  3,  3.0);
  p_colorCodings["BGR"]    = ColorCoding(V4L2_PIX_FMT_BGR24,  3,  3.0);
  p_colorCodings["YUYV"]   = ColorCoding(V4L2_PIX_FMT_YUYV,   0,  2.0);
  p_colorCodings["JPEG"]   = ColorCoding(V4L2_PIX_FMT_JPEG,   0, -1.0);
  p_colorCodings["YUV420"] = ColorCoding(V4L2_PIX_FMT_YUV420, 0,  1.5);
  p_colorCodings["YVU420"] = ColorCoding(V4L2_PIX_FMT_YVU420, 0,  1.5);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION: setupCapture
 *  ==> see headerfile
 *=======================================================================*/ 
void liblmbcam::V4LCam::setupCapture()
{
  if( !testSettings())
  {
    V4LCamError error("invalid settings, can't start camera");
    LMBErrorHandler::ErrorHandler()->handle( error);
    return;
  }
  
  // VIDIOC_S_FMT fails otherwise
  deviceClose();
  deviceOpen();

  v4l2_cropcap cropcap={0};
  v4l2_crop crop={0};
  cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (0 == xioctl( VIDIOC_CROPCAP, &cropcap)) 
  {
    crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    crop.c = cropcap.defrect;
    /* reset to default */
    if (-1 == xioctl( VIDIOC_S_CROP, &crop)) 
    {
      switch (errno) 
      {
      case EINVAL:
        /* Cropping not supported. */
        break;
      default:
        /* Errors ignored. */
        break;
      }
    }
  }

  v4l2_format fmt={0};
  fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if( -1 == xioctl( VIDIOC_G_FMT, &fmt))
  {
    V4LCamError error;
    error << __PRETTY_FUNCTION__ << ": VIDIOC_G_FMT failed";
    LMBErrorHandler::ErrorHandler()->handle( error);
  }
  
  fmt.fmt.pix.width  = p_currentMode.width;
  fmt.fmt.pix.height = p_currentMode.height;
  fmt.fmt.pix.field  = V4L2_FIELD_INTERLACED;
  fmt.fmt.pix.pixelformat = p_currentMode.colorCodingID;
  
  if( -1 == xioctl( VIDIOC_S_FMT, &fmt))
  {
    switch( errno)
    {
    case EBUSY:
      deviceClose();
      deviceOpen();
      if( -1 == xioctl( VIDIOC_S_FMT, &fmt))
      {
        V4LCamError error;
        error << __PRETTY_FUNCTION__ << ": VIDIOC_S_FMT failed after restart";
        LMBErrorHandler::ErrorHandler()->handle( error);
      }
      break;
    default:
    {
      V4LCamError error;
      error << __PRETTY_FUNCTION__ << ": VIDIOC_S_FMT failed";
      LMBErrorHandler::ErrorHandler()->handle( error);
      break;
    }
    }
  }
  
  // sanity check
  if( -1 == xioctl( VIDIOC_G_FMT, &fmt))
  {
    V4LCamError error;
    error << __PRETTY_FUNCTION__ << ": VIDIOC_G_FMT failed";
    LMBErrorHandler::ErrorHandler()->handle( error);
  }
  
  if( fmt.fmt.pix.width != p_currentMode.width)
      std::cout << fmt.fmt.pix.width << "!=" << p_currentMode.width
                << std::endl;
  if( fmt.fmt.pix.height != p_currentMode.height)
      std::cout << fmt.fmt.pix.height << "!=" << p_currentMode.height
                << std::endl;
  if( fmt.fmt.pix.pixelformat != p_currentMode.colorCodingID)
      std::cout << fmt.fmt.pix.pixelformat << "!="
                << p_currentMode.colorCodingID << std::endl;

  v4l2_streamparm frameint={0};
  frameint.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if( frameint.parm.capture.capability & V4L2_CAP_TIMEPERFRAME)
  {
    frameint.parm.capture.timeperframe.numerator  =
        p_currentFramerate.numerator;
    frameint.parm.capture.timeperframe.denominator =
        p_currentFramerate.denominator;
  }
  if( -1 == xioctl( VIDIOC_S_PARM, &frameint))
  {
    V4LCamError error;
    error << __PRETTY_FUNCTION__ << ": unable to set frame interval";
    LMBErrorHandler::ErrorHandler()->handle( error);
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::V4LCam::initBuffer()
{
  size_t imsize = initBufferMmap();
  initRingBuffer( imsize);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::V4LCam::initRingBuffer( size_t imsize)
{
  if( p_ringBuffer)
  {
    V4LCamError error( "ring buffer already allocated");
    LMBErrorHandler::ErrorHandler()->handle( error);
    return;
  }
  
  if( !imsize)
  {
    V4LCamError error( "can't allocate empty ring buffer");
    LMBErrorHandler::ErrorHandler()->handle( error);
    return;
  }
  _vBufFrameStart.resize(p_numBufferedFrames);
  _vBufUsed.resize(p_numBufferedFrames);
  if(_vExternalRingBuffer.size() == 0)
  {
    p_ringBuffer = new unsigned char[imsize*p_numBufferedFrames];
    for( unsigned int i = 0; i < p_numBufferedFrames; ++i)
    {
      _vBufFrameStart[i] = p_ringBuffer + i*imsize;
      _vBufUsed[i] = imsize;
    }   
  }
  else
  {
    for( unsigned int i = 0; i < p_numBufferedFrames; ++i)
    {
      _vBufFrameStart[i] = _vExternalRingBuffer[i];
      _vBufUsed[i] = p_currentMode.bytePerPixel*width()*height();
    }   
  }
  
  p_Filltimes.resize(p_numBufferedFrames);
  p_Starttimes.resize(p_numBufferedFrames);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
size_t
liblmbcam::V4LCam::initBufferMmap()
{
  if( p_ringBuffer)
  {
    V4LCamError error( "mmap buffer already allocated");
    LMBErrorHandler::ErrorHandler()->handle( error);
    return 0;
  }
  
  size_t bufsizemax=0;
  
  v4l2_requestbuffers req={0};
  req.count               = 4;
  req.type                = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  req.memory              = V4L2_MEMORY_MMAP;
  
  if( -1 == xioctl( VIDIOC_REQBUFS, &req)) 
  {
    V4LCamError error;
    switch( errno)
    {
    case EINVAL:
      error << p_device << " does not support memory mapping";
      LMBErrorHandler::ErrorHandler()->handle( error);
      break;
    default:
      error << __PRETTY_FUNCTION__ << ": VIDIOC_REQBUFS failed";
      LMBErrorHandler::ErrorHandler()->handle( error);
      break;
    }
  }
  
  if( req.count < 2) 
  {
    V4LCamError error;
    error << __PRETTY_FUNCTION__ << ": insufficient buffer memory for "
          << req.count << " frames on " << p_device;
    LMBErrorHandler::ErrorHandler()->handle( error);
  }

  p_buffer = new Buffer[req.count];
  if (!p_buffer)
  {
    V4LCamError error;
    error << __PRETTY_FUNCTION__ << ": out of memory";
    LMBErrorHandler::ErrorHandler()->handle( error);
  }
  p_nBuffers = req.count;
  
  for( size_t i = 0; i < p_nBuffers; ++i) 
  {
    v4l2_buffer buf={0};
    buf.type        = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    buf.memory      = V4L2_MEMORY_MMAP;
    buf.index       = i;
    
    if( -1 == xioctl( VIDIOC_QUERYBUF, &buf))
    {
      V4LCamError error;
      error << __PRETTY_FUNCTION__ << ": VIDIOC_QUERYBUF failed";
      LMBErrorHandler::ErrorHandler()->handle( error);
    }
    
    if( buf.length > bufsizemax)
        bufsizemax = buf.length;
    p_buffer[i].length = buf.length;
    {
      MutexLocker ml( &p_devMutex);
      p_buffer[i].start =
          v4l2_mmap (NULL /* start anywhere */,
                     buf.length,
                     PROT_READ | PROT_WRITE /* required */,
                     MAP_SHARED /* recommended */,
                     p_dev, buf.m.offset);
    }
    
    if( MAP_FAILED == p_buffer[i].start)
    {
      V4LCamError error;
      error << __PRETTY_FUNCTION__ << ": v4l2_mmap failed";
      LMBErrorHandler::ErrorHandler()->handle( error);
    }
    
    if (-1 == xioctl( VIDIOC_QBUF, &buf))
    {
      V4LCamError error;
      error << __PRETTY_FUNCTION__ << ": VIDIOC_QBUF failed";
      LMBErrorHandler::ErrorHandler()->handle( error);
    }
  }

  v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
  if (-1 == xioctl( VIDIOC_STREAMON, &type))
  {
    V4LCamError error( "VIDIOC_STREAMON failed");
    LMBErrorHandler::ErrorHandler()->handle( error);
  }
  
  return bufsizemax;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCam::grabberThreadStartFunction( void*)
 *  ==> see headerfile
 *=======================================================================*/
void*
liblmbcam::V4LCam::grabberThreadStartFunctionMmap(void* camera)
{
  try
  {
    V4LCam* cam=(V4LCam*)camera;
    if( !cam)
    {
      V4LCamError error("started grabber without camera");
      LMBErrorHandler::ErrorHandler()->handle( error);
    }
    
    while( cam->p_cameraIsRunning &&
           cam->p_writtenFrameCount < cam->p_maxReadFrames)
    {
      std::cout << "read: " << cam->p_readFrameCount
                << ", written: " << cam->p_writtenFrameCount
                << ", locked: " << cam->p_lockedFrame
                << std::endl;
      
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
      cam->frameReceiveStarted( cam->p_writtenFrameCount);

      timeval currentTime={0};
      gettimeofday( &currentTime, NULL);
      size_t frameIndex = cam->p_writtenFrameCount % cam->p_numBufferedFrames;
      cam->p_Starttimes[frameIndex] = currentTime;
      
      v4l2_buffer buf={0};
      buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
      buf.memory = V4L2_MEMORY_MMAP;
      while(-1 == cam->xioctl( VIDIOC_DQBUF, &buf)) 
      {
        switch (errno) 
        {
        case EAGAIN:
          break;
        case EIO:
          /* Could ignore EIO, see spec. */
        default:
        {
          pthread_cond_broadcast( &(cam->p_frameFinishCond));
          V4LCamError error("VIDIOC_DQBUF failed");
          LMBErrorHandler::ErrorHandler()->handle( error);
          return 0;
          break;
        }
        }
      }
      
      memcpy( cam->_vBufFrameStart[frameIndex], cam->p_buffer[buf.index].start,
              buf.bytesused);
      cam->_vBufUsed[frameIndex] = buf.bytesused;

      cam->p_grabberStatus = COMPLETE;      
      cam->frameReceiveFinished( cam->p_writtenFrameCount);
      ++(cam->p_writtenFrameCount);
      pthread_cond_broadcast( &(cam->p_frameFinishCond));

      if (-1 == cam->xioctl( VIDIOC_QBUF, &buf))
      {
        V4LCamError error("VIDIOC_QBUF failed");
        LMBErrorHandler::ErrorHandler()->handle( error);
      }
    }
  }
  catch( LMBCamError& error)
  {
    std::cerr << error.str() << std::endl;
    V4LCam* cam=(V4LCam*)camera;
    if( cam)
    {
      cam->p_grabberStatus = ERROR;
      pthread_cond_broadcast( &(cam->p_frameStartCond));
      pthread_cond_broadcast( &(cam->p_frameFinishCond));
    }
  }
  
  return 0;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
std::vector<timeval> liblmbcam::V4LCam::measureRelativeFrameStarttimes(int no)
{
#ifdef LIBLMBCAMDEBUG
  std::cout << __PRETTY_FUNCTION__ << std::endl;
#endif

  std::vector<timeval> vResult;
  
  return vResult;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
int liblmbcam::V4LCam::xioctl(int request, void *arg)
{
  MutexLocker l( &p_devMutex);
  
  int r;
  do r = v4l2_ioctl (p_dev, request, arg);
  while (-1 == r && EINTR == errno);
  
  return r;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:V4LCam::reset()
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::V4LCam::reset()
{
  for( std::map<std::string,LMBCamParam*>::iterator it
           = p_availableParameters.begin();
       it != p_availableParameters.end(); ++it)
      it->second->resetValue();
}
/*=========================================================================
 *  DESCRIPTION OF FUNCTION:V4LCam::reset()
 *  ==> see headerfile
 *=======================================================================*/
size_t
liblmbcam::V4LCam::bytesUsed( long frameIndex) const
{
  if( p_numBufferedFrames != _vBufUsed.size()) return 0;
  return _vBufUsed[frameIndex%p_numBufferedFrames];
}
