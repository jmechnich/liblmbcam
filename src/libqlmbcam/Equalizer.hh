/**************************************************************************
**       Title: 
**    $RCSfile: Equalizer.hh,v $
**   $Revision: 1.3 $$Name:  $
**       $Date: 2003/10/23 14:50:33 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: Equalizer.hh,v $
**  Revision 1.3  2003/10/23 14:50:33  mechnich
**   fixed error in Equalizer
**
**  Revision 1.2  2003/10/21 17:14:51  mechnich
**  finished equalizer function
**
**  Revision 1.1  2003/10/17 22:49:56  mechnich
**  initial revision
**
**
**
**************************************************************************/

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
