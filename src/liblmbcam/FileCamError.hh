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

#ifndef FILECAMERROR_HH
#define FILECAMERROR_HH

#include "LMBCamError.hh"

#include <string>

namespace liblmbcam
{
  
/*======================================================================*/
/*!
 *  \class FileCamError FileCamError.hh
 *  \brief The FileCamError class is the standard class for FileCam errors
 */
/*======================================================================*/
  class FileCamError : public LMBCamError
  {
  public:
    FileCamError( const std::string& errmsg)
            :LMBCamError( errmsg)
          {}
  };
}

#endif // FILECAMERROR_HH
