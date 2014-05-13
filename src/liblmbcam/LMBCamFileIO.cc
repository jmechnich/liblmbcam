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

#include "LMBCamFileIO.hh"

#include <algorithm>

std::vector<std::string> liblmbcam::LMBCamFileIO::_formats;

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
unsigned char*
liblmbcam::LMBCamFileIO::loadImageFile( std::istream& is,
                                        std::string format)
{
  if( format == "auto")
  {
  }
  else
  {
    std::vector<std::string>::iterator it = 
        std::find( _formats.begin(), _formats.end(), format);
    if( it != _formats.end())
    {
    }
  }
  
  return 0;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
bool
liblmbcam::LMBCamFileIO::saveImageFile( std::ostream& os, unsigned char* data,
                                        unsigned int width,
                                        unsigned int height,
                                        std::string colorCoding,
                                        double bytePerPixel,
                                        std::string format)
{
  if( format == "auto")
  {
  }
  else
  {
    std::vector<std::string>::iterator it = 
        std::find( _formats.begin(), _formats.end(), format);
    if( it != _formats.end())
    {
    }
  }
  
  return false;
}


