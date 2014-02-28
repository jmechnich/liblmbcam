/**************************************************************************
**       Title: 
**    $RCSfile: Bayer.hh,v $
**   $Revision: 1.1 $$Name:  $
**       $Date: 2003/10/06 13:43:37 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: Bayer.hh,v $
**  Revision 1.1  2003/10/06 13:43:37  mechnich
**  initial revision
**
**
**
**************************************************************************/

/*-------------------------------------------------------------------------
 * Taken from coriander-0.27.1, src/conversion.h  
 *-------------------------------------------------------------------------*/

#ifndef BAYER_HH
#define BAYER_HH

namespace libqlmbcam
{
  
  typedef enum
  {
      NO_BAYER_DECODING,
      BAYER_DECODING_NEAREST,
      BAYER_DECODING_EDGE_SENSE,
      BAYER_DECODING_DOWNSAMPLE
  } bayer_decoding_t;
  
  typedef enum
  {
      NO_STEREO_DECODING,
      STEREO_DECODING
  } stereo_decoding_t;
  
  typedef enum
  {
      BAYER_PATTERN_BGGR,
      BAYER_PATTERN_GRBG,
      BAYER_PATTERN_RGGB,
      BAYER_PATTERN_GBRG
  } bayer_pattern_t;
  
// BAYER -> RGB
  void BayerNearestNeighbor( unsigned char *src,
                             unsigned char *dest,
                             int sx, int sy,
                             bayer_pattern_t type);
  
  void BayerEdgeSense( unsigned char *src,
                       unsigned char *dest,
                       int sx, int sy,
                       bayer_pattern_t type);
  
  void BayerDownsample( unsigned char *src,
                        unsigned char *dest,
                        int sx, int sy,
                        bayer_pattern_t type);
  
  void StereoDecode(unsigned char *src,
                    unsigned char *dest,
                    int NumPixels);
}

#endif
