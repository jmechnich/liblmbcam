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

#ifndef V4LCAMERROR_HH
#define V4LCAMERROR_HH

/*-------------------------------------------------------------------------
 *  Own includes
 *-------------------------------------------------------------------------*/
#include "LMBCamError.hh"

/*-------------------------------------------------------------------------
 *  STL includes
 *-------------------------------------------------------------------------*/
#include <string>

namespace liblmbcam
{
  
/*======================================================================*/
/*!
 *  \class V4LCamError V4LCamError.hh
 *  \brief The V4LCamError class is the standard class for V4LCam errors
 */
/*======================================================================*/
class V4LCamError : public LMBCamError
{
public:
/*======================================================================*/
/*! 
 *   Constructor
 *
 *   \param errmsg error message
 */
/*======================================================================*/
  V4LCamError()
          :LMBCamError()
        {}

  V4LCamError( const std::string& s)
          :LMBCamError( s)
        {}
};
 
/*======================================================================*/
/*!
 *  \class V4LCamCriticalError V4LCamError.hh
 *  \brief The V4LCamCriticalError class is the standard class for critical
 *         V4LCam errors.
 */
/*======================================================================*/
class V4LCamCriticalError : public LMBCamCriticalError
{
public:
/*======================================================================*/
/*! 
 *   Constructor
 *
 *   \param errmsg error message
 */
/*======================================================================*/
  V4LCamCriticalError()
          :LMBCamCriticalError()
        {}

  V4LCamCriticalError( const std::string& s)
          :LMBCamCriticalError( s)
        {}
};
 
/*======================================================================*/
/*!
 *  \class V4LCamBusError V4LCamError.hh
 *  \brief The V4LCamBusError class is the standard class for
 *         V4LCamBus errors.
 */
/*======================================================================*/
class V4LCamBusError : public LMBCamBusError
{
public:
/*======================================================================*/
/*! 
 *   Constructor
 *
 *   \param errmsg error message
 */
/*======================================================================*/
  V4LCamBusError()
          :LMBCamBusError()
        {}

  V4LCamBusError( const std::string& s)
          :LMBCamBusError( s)
        {}
};
}

#endif

