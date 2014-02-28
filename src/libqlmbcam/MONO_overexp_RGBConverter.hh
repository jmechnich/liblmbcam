/**************************************************************************
**       Title: 
**    $RCSfile: MONO_overexp_RGBConverter.hh,v $
**   $Revision: 1.4 $$Name:  $
**       $Date: 2005/07/14 09:36:35 $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: MONO_overexp_RGBConverter.hh,v $
**  Revision 1.4  2005/07/14 09:36:35  tschmidt
**  - added SubSampling capabilities
**
**  Revision 1.3  2005/02/28 00:13:19  mechnich
**  added subdir scripts Makefile.am
**
**  Revision 1.2  2005/02/25 19:26:11  mechnich
**  - derive from PixelConverter class
**  - changed fucntion names
**
**  Revision 1.1  2004/12/14 10:30:38  ronneber
**  - initial revision
**
**
**
**************************************************************************/

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
