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

#ifndef LMBCAMBUS_HH
#define LMBCAMBUS_HH

#include "LMBCam.hh"

#include <vector>

namespace liblmbcam
{

/*======================================================================*/
/*!
 *  \class LMBCamBus LMBCamBus.hh
 *  \brief The LMBCamBus class specifies the interface for all bus classes
 */
/*======================================================================*/
class LMBCamBus
{
public:
  LMBCamBus()
        {}
  
  virtual ~LMBCamBus()
        {}
  
  virtual unsigned int nCameras() const=0;
  virtual LMBCam* cameraByIndex( unsigned int index) const=0;
  virtual LMBCam* cameraByGUID( const std::string& guid) const=0;
};
 
}

#endif
