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

#ifndef EQUALIZER_HH
#define EQUALIZER_HH

#include <cmath>

namespace libqlmbcam
{
  inline
  void EqualizeImage( unsigned char *src,
                      unsigned char *dest,
                      int sx, int sy,
                      int lambda,
                      int bytePerPixel)
  {
    float lambdaNormed = lambda / 1000.0;
    unsigned char* src_copy = src;
    unsigned char* dest_copy = dest;
    int sxHalf = sx/2;
    int syHalf = sy/2;
        
    for( int y=0; y < sy; ++y)
    {
      for( int x=0; x < sx; ++x)
      {
        float normP = ( float( x - sxHalf) * float( x - sxHalf) +
                        float( y - syHalf) * float( y - syHalf)) /
            float(sxHalf * sxHalf);
        
        //float factor = 1 + lambdaNormed * normP;
        float factor = (1 - sqrt( 1 - 4*lambdaNormed * normP)) /
            (2*lambdaNormed*normP);
        int srcX = (int) roundf( (x - sxHalf) / factor) + sxHalf;
        int srcY = (int) roundf( (y - syHalf) / factor) + syHalf;
        
        srcX = srcX < 0 ? 0 : srcX;
        srcY = srcY < 0 ? 0 : srcY;
        srcX = srcX < sx ? srcX : sx;
        srcY = srcY < sy ? srcY : sy;
        
        src_copy += ( srcX + srcY * sx) * bytePerPixel;
        
        for( int i=0; i < bytePerPixel; ++i)
        {
          if( srcX == 0 || srcX == sx || srcY == 0 || srcY == sy)
          {
            *(dest_copy+i) = 0;
            continue;
          }
          
          *(dest_copy+i) = *(src_copy+i);
        }

        src_copy = src;
        dest_copy += bytePerPixel;
      }
    }
  }
}

#endif
