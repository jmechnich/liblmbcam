/**************************************************************************
**       Title: 
**    $RCSfile: FireCamBusRegistry.cc,v $
**   $Revision: 1.1 $$Name:  $
**       $Date: 2003/10/01 13:33:33 $
**   Copyright: GPL $Author: pigorsch $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: FireCamBusRegistry.cc,v $
**  Revision 1.1  2003/10/01 13:33:33  pigorsch
**  - initial revision
**
**
**
**************************************************************************/

#include "FireCamBus.hh"
#include "FireCamError.hh"
#include "FireCamBusRegistry.hh"

liblmbcam::FireCamBusRegistry* liblmbcam::FireCamBusRegistry::_instance=0;

liblmbcam::FireCamBusRegistry*
liblmbcam::FireCamBusRegistry::
instance()
{
  if (_instance==0)
  {
    _instance=new FireCamBusRegistry;
  }
  
  return _instance;
}

void
liblmbcam::FireCamBusRegistry::
registerBus(raw1394handle_t handle, FireCamBus* bus)
{
  std::pair<raw1394handle_t, FireCamBus*> p(handle, bus);
  _busses.push_back(p);
}

void 
liblmbcam::FireCamBusRegistry::
deregisterBus(liblmbcam::FireCamBus* bus)
{
  for (std::vector<std::pair<raw1394handle_t, FireCamBus*> >::iterator p=_busses.begin();
       p!=_busses.end();
       ++p)
  {
    if (p->second==bus)
    {
      _busses.erase(p);
      break;
    }
  }
}

liblmbcam::FireCamBus*
liblmbcam::FireCamBusRegistry::
bus(raw1394handle_t handle)
{
  for (std::vector<std::pair<raw1394handle_t, FireCamBus*> >::iterator p=_busses.begin();
       p!=_busses.end();
       ++p)
  {
    if (p->first==handle)
    {
      return p->second;
    }
  }
  
  FireCamBusError error("FireCamBusRegistry::bus requested bus with unknown handle");
  LMBErrorHandler::ErrorHandler()->handle( error);

  // just to get rid of a compiler warning
  return 0;
}

liblmbcam::FireCamBusRegistry::
FireCamBusRegistry()
{}
