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

#include "FireCamBus.hh"

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::LMBCamBusIndex::LMBCamBusIndex()
{
  /*-----------------------------------------------------------------------
   *  Search for firewire busses
   *-----------------------------------------------------------------------*/
  raw1394handle_t handle = raw1394_new_handle();
  int nPorts = raw1394_get_port_info( handle, 0, 0);
  raw1394_destroy_handle( handle);

  for( int i=0; i < nPorts; ++i)
  {
    std::ostringstream oss;
    oss << "FireCamBus_" << i;
    _busses[oss.str()] = new FireCamBus( i);
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::LMBCamBusIndex::~LMBCamBusIndex()
{
// This should be done but causes the program to hang on deletion of the
//   raw1394 handle, FIX ME

//   for( std::map<std::string,LMBCamBus*>::iterator it=_busses.begin();
//        it!=_busses.end(); ++it)
//   {
//     delete it->second;
//   }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::LMBCam*
liblmbcam::LMBCamBusIndex::findCamera( const std::string& guid) const
{
  for( std::map<std::string,LMBCamBus*>::const_iterator it=_busses.begin();
       it!=_busses.end(); ++it)
  {
    try
    {
      return it->second->cameraByGUID( guid);
    }
    catch(...)
    {
    }
  }
  return 0;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::LMBCam*
liblmbcam::LMBCamBusIndex::getFirstCamera() const
{
  for( std::map<std::string,LMBCamBus*>::const_iterator it=_busses.begin();
       it!=_busses.end(); ++it)
  {
    try
    {
      return it->second->cameraByIndex( 0);
    }
    catch(...)
    {
    }
  }
  return 0;
}

