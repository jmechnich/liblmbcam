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

#include "LMBCamBusIndex.hh"

#ifdef HAVE_LIBDC1394
#include "FireCamBus.hh"
#endif

#ifdef HAVE_V4L2
#include "V4LCamBus.hh"
#endif

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::LMBCamBusIndex::LMBCamBusIndex( BusType type)
        : _type( type)
{
  if( _type & FireCam)
  {
#ifdef HAVE_LIBDC1394
    /*---------------------------------------------------------------------
     *  Search for firewire busses
     *---------------------------------------------------------------------*/
    raw1394handle_t handle = raw1394_new_handle();
    int nPorts = raw1394_get_port_info( handle, 0, 0);
    raw1394_destroy_handle( handle);
    
    for( int i=0; i < nPorts; ++i)
    {
      std::ostringstream oss;
      oss << "FireCamBus_" << i;
      _busses[oss.str()] = new FireCamBus( i);
    }
#endif
  }

  if( _type & V4LCam)
  {
#ifdef HAVE_V4L2
    _busses["V4LCamBus"] = new V4LCamBus;
#endif
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::LMBCamBusIndex::~LMBCamBusIndex()
{
  for( std::map<std::string,LMBCamBus*>::iterator it=_busses.begin();
       it!=_busses.end(); ++it)
  {
    delete it->second;
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
unsigned int liblmbcam::LMBCamBusIndex::nCameras() const
{
  unsigned int ret = 0;
  for( std::map<std::string,LMBCamBus*>::const_iterator it=_busses.begin();
       it!=_busses.end(); ++it)
  {
    ret += it->second->nCameras();
  }
  return ret;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::LMBCam*
liblmbcam::LMBCamBusIndex::cameraByIndex( unsigned int index) const
{
  for( std::map<std::string,LMBCamBus*>::const_iterator it=_busses.begin();
       it!=_busses.end(); ++it)
  {
    unsigned int ncams = it->second->nCameras();
    if( ncams <= index)
    {
      index -= ncams;
      continue;
    }
    return it->second->cameraByIndex( index);
  }
  return 0;
}
/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::LMBCam*
liblmbcam::LMBCamBusIndex::cameraByGUID( const std::string& guid) const
{
  for( std::map<std::string,LMBCamBus*>::const_iterator it=_busses.begin();
       it!=_busses.end(); ++it)
  {
    unsigned int ncams = it->second->nCameras();
    for( unsigned int i=0; i < ncams; ++i)
    {
      LMBCam* cam = it->second->cameraByIndex( i);
      if( cam->guid() == guid)
          return cam;
    }
  }
  return 0;
}
