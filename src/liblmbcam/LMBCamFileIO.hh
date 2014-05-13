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

#ifndef LMBCAMFILEIO_HH
#define LMBCAMFILEIO_HH

#include <iostream>
#include <vector>

namespace liblmbcam
{
  class LMBCamFileIO
  {
  public:
    static unsigned char* loadImageFile( std::istream& is,
                                         std::string format="auto");

    static bool saveImageFile( std::ostream& os, unsigned char* data,
                               unsigned int width, unsigned int height,
                               std::string colorCoding, double bytePerPixel,
                               std::string format="auto");
  private:
    LMBCamFileIO()
          {
            _formats.push_back( "pgm");
            _formats.push_back( "ppm");
          }
    
    static std::vector<std::string> _formats;
    
    static LMBCamFileIO _singleton;
  };
}

#endif // LMBCAMFILEIO_HH
