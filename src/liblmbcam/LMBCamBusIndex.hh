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

#ifndef LMBCAMBUSINDEX_HH
#define LMBCAMBUSINDEX_HH

#include <string>
#include <map>

#include <LMBCamBus.hh>

namespace liblmbcam
{
  class LMBCamBusIndex : public LMBCamBus
  {
  public:
    enum BusType
    {
        None=0x0,
        FireCam=0x1,
        V4LCam=0x2,
        All=0x3
    };
    
    LMBCamBusIndex( BusType type=All);
    ~LMBCamBusIndex();

    unsigned int nCameras() const;
    LMBCam* cameraByIndex( unsigned int index) const;
    LMBCam* cameraByGUID( const std::string& guid) const;

  private:
    std::map<std::string,LMBCamBus*> _busses;
    BusType _type;
  };
}

#endif
