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

#ifndef MONO_OVEREXP_RGBCONVERTER_HH
#define MONO_OVEREXP_RGBCONVERTER_HH

#include <PixelConverter.hh>

class MONO_overexp_RGBConverter : public PixelConverter
{
public:
  MONO_overexp_RGBConverter()
      {}

  void lineToRgb8( unsigned char* src,
                   unsigned char* trg,
                   unsigned int width,
                   unsigned int subSampling = 1) const
        {
          while( width > 0)
          {
            if( *src == 255)
            {
              *(trg++) = 255;
              *(trg++) = 0;
              *(trg++) = 0;
            }
            else
            {
              *(trg++) = *src;
              *(trg++) = *src;
              *(trg++) = *src;
            }
            
            src += subSampling;
            --width;
          }
        }

  void lineToBgr8( unsigned char* src,
                   unsigned char* trg,
                   unsigned int width,
                   unsigned int padding,
                   unsigned int subSampling = 1) const
        {
          while( width > 0)
          {
            if( *src == 255)
            {
              *(trg++) = 0;
              *(trg++) = 0;
              *(trg++) = 255;
            }
            else
            {
              *(trg++) = *src;
              *(trg++) = *src;
              *(trg++) = *src;
            }
            
            src += subSampling;
            trg += padding;
            --width;
          }
        }

  void lineToRgba8( unsigned char* src,
                    unsigned char* trg,
                    unsigned int width,
                   unsigned int subSampling = 1) const
        {
          unsigned int* trgCopy = reinterpret_cast<unsigned int*>(trg);
          while( width > 0)
          {
            if( *src == 255)
            {
              *trgCopy = 255 << 16;
            }
            else
            {
              *trgCopy = *src << 16 | *src << 8 | *src << 0;
            }
            
            src += subSampling;
            ++trgCopy;
            --width;
          }
        }

  void lineToRgb16( unsigned char* src,
                    unsigned short* trg,
                    unsigned int width,
                   unsigned int subSampling = 1) const
        {
          while( width > 0)
          {
            if( *src == 255)
            {
              *(trg++) = _convTable8To16[255];
              *(trg++) = 0;
              *(trg++) = 0l;
            }
            else
            {
              unsigned short val = _convTable8To16[*(src)];
              *(trg++) = val;
              *(trg++) = val;
              *(trg++) = val;
            }
            src += subSampling;
            --width;
          }
        }

  float srcBytePerPixel() const
        {
          return 1;
        }

};

#endif
