/**************************************************************************
**       Title: 
**    $RCSfile: FireCamBus.cc,v $
**   $Revision: 1.13 $$Name:  $
**       $Date: 2004/01/26 21:33:41 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: FireCamBus.cc,v $
**  Revision 1.13  2004/01/26 21:33:41  mechnich
**  added FireCamBus::available() function
**
**  Revision 1.12  2003/10/01 13:32:34  pigorsch
**  - register/unregister bus in constructor/destructor
**
**  Revision 1.11  2003/08/05 14:40:34  mechnich
**  call dc1394_destroy_handle() instead of raw1394_destroy_handle()
**
**  Revision 1.10  2003/07/03 15:25:17  mechnich
**  - added comments
**  - improved Format7 capabilities
**  - added boolean return values to set...() functions
**
**  Revision 1.9  2003/06/12 16:13:19  mechnich
**  added comments
**
**  Revision 1.8  2003/05/09 14:22:00  ronneber
**  - releases bus correctly when exception in constructor occurs
**  - corrected typo in error message
**
**  Revision 1.7  2003/05/07 16:14:31  ronneber
**  - extended error message for Constructor
**
**  Revision 1.6  2003/05/07 00:06:29  mechnich
**  intorduced a wrapper mutex for critical libraw1394 functions,
**  liblmbcam should now be completely thread safe
**
**  Revision 1.5  2003/04/23 13:57:31  mechnich
**  removed deprecated includes
**
**  Revision 1.4  2003/04/23 13:28:52  mechnich
**  - fixed parameter queries for all tested cameras
**  - fixed bug in iso handler function
**  - moved p_cameraIsRunning with affiliated functions to LMBCam
**
**  Revision 1.3  2003/01/03 16:23:12  mechnich
**  corrected minor bugs
**
**  Revision 1.2  2002/12/10 02:42:06  mechnich
**  added dma related functions in LMBCam
**  fixed bugs with PYRO WEBCAM API-200
**
**  Revision 1.1  2002/12/04 13:17:46  mechnich
**  initial revision
**
**
**
**************************************************************************/

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

