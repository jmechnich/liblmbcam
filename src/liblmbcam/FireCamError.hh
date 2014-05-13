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

#ifndef FIRECAMERROR_HH
#define FIRECAMERROR_HH

#include "LMBCamError.hh"

#include <string>

namespace liblmbcam
{
  
/*======================================================================*/
/*!
 *  \class FireCamError FireCamError.hh
 *  \brief The FireCamError class is the standard class for FireCam errors
 */
/*======================================================================*/
class FireCamError : public LMBCamError
{
public:
  FireCamError()
          :LMBCamError()
        {}

  FireCamError( const std::string& s)
          :LMBCamError( s)
        {}
};
 
/*======================================================================*/
/*!
 *  \class FireCamCriticalError FireCamError.hh
 *  \brief The FireCamCriticalError class is the standard class for critical
 *         FireCam errors.
 */
/*======================================================================*/
class FireCamCriticalError : public LMBCamCriticalError
{
public:
  FireCamCriticalError()
          :LMBCamCriticalError()
        {}

  FireCamCriticalError( const std::string& s)
          :LMBCamCriticalError( s)
        {}
};
 
/*======================================================================*/
/*!
 *  \class FireCamBusError FireCamError.hh
 *  \brief The FireCamBusError class is the standard class for
 *         FireCamBus errors.
 */
/*======================================================================*/
class FireCamBusError : public LMBCamBusError
{
public:
  FireCamBusError()
          :LMBCamBusError()
        {}

  FireCamBusError( const std::string& s)
          :LMBCamBusError( s)
        {}
};

/*======================================================================*/
/*!
 *  \class FireCamLostDMAPacketError
 *  \brief The FireCamBusError class is the standard class for
 *         FireCamBus errors.
 */
/*======================================================================*/
class FireCamLostDMAPacketError : FireCamError
{
public:
  
  FireCamLostDMAPacketError()
        {}
  
  FireCamLostDMAPacketError(const std::string& s) 
      : FireCamError(s)
        {}
};

 
}

#endif
