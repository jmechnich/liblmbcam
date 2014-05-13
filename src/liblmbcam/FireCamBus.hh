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

#ifndef FIRECAMBUS_HH
#define FIRECAMBUS_HH

#include "LMBCamBus.hh"
#include "LMBErrorHandler.hh"
#include "FireCam.hh"
#include "FireCamError.hh"

#include <libdc1394/dc1394_control.h>

#include <vector>
#include <sstream>

#include <pthread.h>

namespace liblmbcam
{
/*======================================================================*/
/*!
 *  \class FireCamBus FireCamBus.hh
 *  \brief The FireCamBus class represents a ieee1394 bus which allows
 *         access to connected ieee1394 cameras.
 *
 *  
 */
/*======================================================================*/
class FireCamBus : public LMBCamBus
{
public:
  FireCamBus( unsigned int busIndex=0);
  ~FireCamBus();
  
  void rescan( bool killHandleIfHighestNode=false);
  LMBCam* cameraByIndex( unsigned int index) const;
  LMBCam* cameraByGUID( const std::string& guid) const;
  static std::vector<unsigned int> available();
  unsigned int nCameras() const {return p_cameras.size();}
  
private:
  FireCamBus( const FireCamBus&);
  void operator=( const FireCamBus&);

  unsigned int p_busIndex;
  std::vector<FireCam*> p_cameras;
  raw1394handle_t p_handle;
  pthread_mutex_t p_libRawMutex;
};

}

#include "FireCamBus.icc"

#endif
