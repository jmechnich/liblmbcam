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

#ifndef LMBCAMERROR_HH
#define LMBCAMERROR_HH

#include "LMBError.hh"

#include <string>

namespace liblmbcam
{
  
/*======================================================================*/
/*!
 *  \class LMBCamError LMBCamError.hh
 *  \brief The LMBCamError class is the base class of all camera error classes
 */
/*======================================================================*/
  class LMBCamError : public LMBError
  {
  public:
    LMBCamError()
            :LMBError()
          {}

    LMBCamError( const std::string& s)
            :LMBError( s)
          {}
  };

/*======================================================================*/
/*!
 *  \class LMBCamCriticalError LMBCamError.hh
 *  \brief The LMBCamCriticalError class is the base class of all critical 
 *         camera error classes
 */
/*======================================================================*/
  class LMBCamCriticalError : public LMBCriticalError
  {
public:
    LMBCamCriticalError()
            :LMBCriticalError()
          {}

    LMBCamCriticalError( const std::string& s)
            :LMBCriticalError( s)
          {}
  };

/*======================================================================*/
/*!
 *  \class LMBCamBusError LMBCamError.hh
 *  \brief The LMBCamBusError class is the base class for all bus error classes
 */
/*======================================================================*/
  class LMBCamBusError : public LMBCamCriticalError
  {
  public:
    LMBCamBusError()
            :LMBCamCriticalError()
          {}

    LMBCamBusError( const std::string& s)
            :LMBCamCriticalError( s)
          {}
  };
  
/*======================================================================*/
/*!
 *  \class LMBCamFileIOError LMBCamError.hh
 *  \brief The LMBCamFileIOError class is the base class for all IO error
 *         classes
 */
/*======================================================================*/
  class LMBCamFileIOError : public LMBCamError
  {
  public:
    LMBCamFileIOError()
            :LMBCamError()
          {}

    LMBCamFileIOError( const std::string& s)
            :LMBCamError( s)
          {}
  };
}

#endif
