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

#ifndef V4LCAMBUS_HH
#define V4LCAMBUS_HH

#include "LMBCamBus.hh"
#include "LMBErrorHandler.hh"
#include "V4LCam.hh"
#include "V4LCamError.hh"

#include <vector>
#include <sstream>

#include <pthread.h>

namespace liblmbcam
{
/*======================================================================*/
/*!
 *  \class V4LCamBus V4LCamBus.hh
 *  \brief The V4LCamBus class represents a V4L2 bus which allows
 *         access to connected cameras.
 *
 *  
 */
/*======================================================================*/
class V4LCamBus : public LMBCamBus
{
public:
  V4LCamBus();
  ~V4LCamBus();
  
  void rescan();
  unsigned int nCameras() const;
  LMBCam* cameraByIndex( unsigned int index) const;
  LMBCam* cameraByGUID( const std::string& guid) const;
  
private:
  V4LCamBus( const V4LCamBus&);
  void operator=( const V4LCamBus&);
  
  void clearCameras();
  
  std::vector<V4LCam*> p_cameras;
};
}

#endif
