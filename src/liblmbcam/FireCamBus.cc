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

#include "FireCamBus.hh"

#include "FireCamBusRegistry.hh"

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCamBus::FireCamBus( unsigned int busIndex)
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::FireCamBus::FireCamBus( unsigned int busIndex)
        : LMBCamBus(), p_busIndex( busIndex)
{
  pthread_mutex_init( &p_libRawMutex, NULL);
  
  /*-----------------------------------------------------------------------
   *  Open ohci and asign handle to it
   *-----------------------------------------------------------------------*/
  p_handle = dc1394_create_handle( p_busIndex);

  if ( p_handle == NULL)
  {
    std::stringstream errString;
    errString << "Unable to aquire a raw1394 handle for bus " << busIndex
              << "\nDid you 'insmod' the appropriate kernel modules?";
    
    FireCamBusError error( errString.str());
    LMBErrorHandler::ErrorHandler()->handle( error);
  }
  
  FireCamBusRegistry::instance()->registerBus(p_handle, this);
  
  rescan( true);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCamBus::~FireCamBus()
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::FireCamBus::~FireCamBus()
{
  if( p_cameras.size() != 0)
  {
    for( std::vector<FireCam*>::iterator it = p_cameras.begin();
         it != p_cameras.end(); ++it)
    {
      delete (*it);
    }

    p_cameras.clear();
  }
  
  FireCamBusRegistry::instance()->deregisterBus(this);
  
  if( p_handle != NULL)
  {
    raw1394_destroy_handle( p_handle);
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCamBus::rescan()
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::FireCamBus::rescan( bool killHandleIfHighestNode)
{
  if( p_cameras.size() != 0)
  {
    for( std::vector<FireCam*>::iterator it = p_cameras.begin();
         it != p_cameras.end(); ++it)
    {
      delete (*it);
    }

    p_cameras.clear();
  }
  
  /*-----------------------------------------------------------------------
   *  Get the camera nodes
   *-----------------------------------------------------------------------*/
  int numNodes = raw1394_get_nodecount( p_handle);
  int numCameras;
  nodeid_t* camera_nodes = dc1394_get_camera_nodes( p_handle, &numCameras, 0);
  
  for( int i=0; i < numCameras; ++i)
  {
    if( camera_nodes[i] == numNodes-1)
    {
      free( camera_nodes);

      std::stringstream err;
      err << "Camera " << i << " is highest node";

      if( killHandleIfHighestNode && p_handle != NULL)
      {
        dc1394_destroy_handle( p_handle);
      }
      
      FireCamBusError error( err.str());
      LMBErrorHandler::ErrorHandler()->handle( error);
    }
    p_cameras.push_back( new FireCam( p_handle, camera_nodes[i], p_busIndex, i,
                                      &p_libRawMutex));
  }

  free( camera_nodes);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::LMBCam*
liblmbcam::FireCamBus::cameraByGUID( const std::string& guid) const
{
  LMBCam* ret=0;
  
  for( std::vector<FireCam*>::const_iterator it = p_cameras.begin();
       it != p_cameras.end(); ++it)
  {
    if( (*it)->guid() == guid)
    {
      ret = *it;
    }
  }
  
  if( ret == 0)
  {
    std::stringstream errString;
    errString << "no camera with guid " << guid << " attached to bus "
              << p_busIndex;

    FireCamBusError error( errString.str());
    LMBErrorHandler::ErrorHandler()->handle( error);
  }

  return ret;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
std::vector<unsigned int>
liblmbcam::FireCamBus::available()
{
  std::vector<unsigned int> ret;
  
  try
  {
    for( unsigned int i=0; ; ++i)
    {
      raw1394handle_t handle = dc1394_create_handle( i);

      if( handle == NULL)
      {
        throw;
      }
      
      ret.push_back( i);
      raw1394_destroy_handle( handle);
    }
  }
  catch(...)
  {
  }
  return ret;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::LMBCam*
liblmbcam::FireCamBus::cameraByIndex( unsigned int index) const
{
  if( index >= p_cameras.size())
  {
    std::stringstream errString;
    errString << "no camera with index " << index << " attached to bus "
              << p_busIndex;

    FireCamBusError error( errString.str());
    LMBErrorHandler::ErrorHandler()->handle( error);
  }
  
  return p_cameras[index];
}

