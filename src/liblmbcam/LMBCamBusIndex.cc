/**************************************************************************
**       Title: 
**    $RCSfile: LMBCamBusIndex.cc,v $
**   $Revision: 1.3 $$Name:  $
**       $Date: 2004/06/29 16:27:53 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: LMBCamBusIndex.cc,v $
**  Revision 1.3  2004/06/29 16:27:53  mechnich
**  added getFirstCamera()
**
**  Revision 1.2  2004/06/06 17:03:21  mechnich
**  *** empty log message ***
**
**  Revision 1.1  2004/05/30 20:54:20  mechnich
**  initial revision
**
**
**
**************************************************************************/

#include "LMBCamBusIndex.hh"

#ifdef HAVE_LIBDC1394
#include "FireCamBus.hh"
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
