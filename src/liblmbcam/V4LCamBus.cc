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

#include "V4LCamBus.hh"

#include <libudev.h>
#include <cstdlib>
#include <cstdio>
#include <cstring>

#include "V4LCam.hh"

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:V4LCamBus::V4LCamBus( unsigned int busIndex)
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::V4LCamBus::V4LCamBus()
        : LMBCamBus()
{
  rescan();
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:V4LCamBus::~V4LCamBus()
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::V4LCamBus::~V4LCamBus()
{
  clearCameras();
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::V4LCamBus::clearCameras()
{
  if(p_cameras.size())
  {
    for( std::vector<V4LCam*>::iterator it = p_cameras.begin();
         it != p_cameras.end(); ++it)
    {
      (*it)->stopCamera();
      delete (*it);
    }
    p_cameras.clear();
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:V4LCamBus::rescan()
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::V4LCamBus::rescan()
{
  clearCameras();
  
  struct udev *udev;
  udev = udev_new();
  
  for( size_t counter=0; ; ++counter)
  {
    std::ostringstream os;
    os << "/dev/video" << counter;
    std::string name( os.str());

    struct stat statbuf={0};
    char type='u';
    
    if( stat(name.c_str(), &statbuf) < 0) break;
    
    if      (S_ISBLK(statbuf.st_mode)) type = 'b';
    else if (S_ISCHR(statbuf.st_mode)) type = 'c';
    else break;
    
    udev_device* device =
        udev_device_new_from_devnum(udev, type, statbuf.st_rdev);

    if( !device)
    {
      V4LCamBusError error;
      error << "no udev device for " << name;
      LMBErrorHandler::ErrorHandler()->handle( error);
      continue;
    }
    
    std::string v4lcaps =
        udev_device_get_property_value( device, "ID_V4L_CAPABILITIES");

    if( !v4lcaps.size() || v4lcaps.find(":capture:") == std::string::npos)
        continue;
    
    std::string vendor =
        udev_device_get_property_value( device, "ID_VENDOR");
    std::string vendor_id =
        udev_device_get_property_value( device, "ID_VENDOR_ID");
    std::string model =
        udev_device_get_property_value( device, "ID_MODEL");
    std::string model_id =
        udev_device_get_property_value( device, "ID_MODEL_ID");
    std::string revision =
        udev_device_get_property_value( device, "ID_REVISION");
    udev_device_unref( device);

    p_cameras.push_back(
        new V4LCam( name, vendor, model, vendor_id+model_id+revision));
  }

  udev_unref(udev);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::LMBCam*
liblmbcam::V4LCamBus::cameraByGUID( const std::string& guid) const
{
  LMBCam* ret=0;
  
  for( std::vector<V4LCam*>::const_iterator it = p_cameras.begin();
       it != p_cameras.end(); ++it)
  {
    if( (*it)->guid() == guid)
        ret = *it;
  }
  
  if( ret == 0)
  {
    V4LCamBusError error;
    error << "no camera with guid " << guid;
    LMBErrorHandler::ErrorHandler()->handle( error);
  }

  return ret;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
unsigned int
liblmbcam::V4LCamBus::nCameras() const
{
  return p_cameras.size();
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::LMBCam*
liblmbcam::V4LCamBus::cameraByIndex( unsigned int index) const
{
  LMBCam* ret=0;

  if( index < p_cameras.size())
  {
    ret = p_cameras[index];
  }
  else
  {
    V4LCamBusError error;
    error << "no camera with index " << index;
    LMBErrorHandler::ErrorHandler()->handle( error);
  }
  
  return ret;
}
