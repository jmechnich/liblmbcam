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

#ifndef CONVERTERS_HH
#define CONVERTERS_HH

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

  void ConvertYUV420( unsigned char *src,
                      unsigned char *dest,
                      unsigned int sx, unsigned int sy);
  void ConvertYVU420( unsigned char *src,
                      unsigned char *dest,
                      unsigned int sx, unsigned int sy);
  void ConvertJPEG( unsigned char* src, unsigned int nbytes,
                    unsigned char* dest);
}

#endif
