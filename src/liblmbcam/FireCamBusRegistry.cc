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

#include "FireCamBusRegistry.hh"

#include "FireCamBus.hh"
#include "FireCamError.hh"

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
