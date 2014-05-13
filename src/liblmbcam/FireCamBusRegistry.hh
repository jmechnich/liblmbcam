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

#ifndef FIRECAMBUSREGISTRY_HH
#define FIRECAMBUSREGISTRY_HH

#include <map>
#include <utility>

#include <libdc1394/dc1394_control.h>

namespace liblmbcam
{
  class FireCamBus;
  
  /*======================================================================*/
  /*!
   *  \class FireCamBusRegistry FireCamBusRegistry.hh
   *  \brief The FireCamBusRegistry class tracks instances of FireCamBus
   #         objects
   *
   *  (description)
   */
  /*======================================================================*/
  class FireCamBusRegistry
  {
  public:
    static FireCamBusRegistry* instance();
    
    void registerBus(raw1394handle_t handle, FireCamBus* bus);
    void deregisterBus(FireCamBus* bus);

    FireCamBus* bus(raw1394handle_t handle);
    
  protected:
    FireCamBusRegistry();
    
  private:
    static FireCamBusRegistry* _instance;
    std::vector<std::pair<raw1394handle_t, FireCamBus*> > _busses;
  };

}


#endif // FIRECAMBUSREGISTRY_HH
