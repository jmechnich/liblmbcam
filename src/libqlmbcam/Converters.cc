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

#include "Converters.hh"

#include <PixelConverter.hh>

#include <cstdlib>
#include <cstdio>
#include <string>

#ifdef HAVE_LIBJPEG
#include <jpeglib.h>
#endif

/*-------------------------------------------------------------------------
 * Taken from coriander-0.27.1, src/conversion.c 
 *-------------------------------------------------------------------------*/

/****************************************************************
 *     Color conversion functions for cameras that can          * 
 * output raw-Bayer pattern images, such as some Basler cameras *
 * Most of the algos presented here com from                    *
 * http://ise0.Stanford.EDU/~tingchen/main.htm                  *
 * and have been converted from Matlab to C                     *
 ****************************************************************/

#define CLIP(in, out)\
{\
   in = in < 0 ? 0 : in;\
   in = in > 255 ? 255 : in;\
   out=in;\
}
  
/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::BayerNearestNeighbor( unsigned char *src,
                                  unsigned char *dest,
                                  int sx, int sy,
                                  bayer_pattern_t type)
{
  unsigned char *outR, *outG, *outB;
  register int i,j;

  // sx and sy should be even
  switch (type)
  {
  case BAYER_PATTERN_GRBG:
  case BAYER_PATTERN_BGGR:
    outR=&dest[0];
    outG=&dest[1];
    outB=&dest[2];
    break;
  case BAYER_PATTERN_GBRG:
  case BAYER_PATTERN_RGGB:
    outR=&dest[2];
    outG=&dest[1];
    outB=&dest[0];
    break;
  default:
    outR=NULL;outG=NULL;outB=NULL;
    break;
  }
  
  switch (type)
  {
  case BAYER_PATTERN_GRBG: //-------------------------------------------
  case BAYER_PATTERN_GBRG:
    // copy original RGB data to output images
    for (i=0;i<sy;i+=2)
	for (j=0;j<sx;j+=2)
        {
          outG[(i*sx+j)*3]=src[i*sx+j];
          outG[((i+1)*sx+(j+1))*3]=src[(i+1)*sx+(j+1)];
          outR[(i*sx+j+1)*3]=src[i*sx+j+1];
          outB[((i+1)*sx+j)*3]=src[(i+1)*sx+j];
        }
    
    // R channel
    for (i=0;i<sy;i+=2)
	for (j=0;j<sx-1;j+=2)
        {
          outR[(i*sx+j)*3]=outR[(i*sx+j+1)*3];
          outR[((i+1)*sx+j+1)*3]=outR[(i*sx+j+1)*3];
          outR[((i+1)*sx+j)*3]=outR[(i*sx+j+1)*3];
        }
    
    // B channel
    for (i=0;i<sy-1;i+=2)//every two lines
	for (j=0;j<sx-1;j+=2)
        {
          outB[(i*sx+j)*3]=outB[((i+1)*sx+j)*3];
          outB[(i*sx+j+1)*3]=outB[((i+1)*sx+j)*3];
          outB[((i+1)*sx+j+1)*3]=outB[((i+1)*sx+j)*3];
        }
    
    // using lower direction for G channel
    
    // G channel
    for (i=0;i<sy-1;i+=2)//every two lines
	for (j=1;j<sx;j+=2)
            outG[(i*sx+j)*3]=outG[((i+1)*sx+j)*3];
    
    for (i=1;i<sy-2;i+=2)//every two lines
	for (j=0;j<sx-1;j+=2)
            outG[(i*sx+j)*3]=outG[((i+1)*sx+j)*3];
    
    // copy it for the next line
    for (j=0;j<sx-1;j+=2)
	outG[((sy-1)*sx+j)*3]=outG[((sy-2)*sx+j)*3];
    
    break;
  case BAYER_PATTERN_BGGR: //-------------------------------------------
  case BAYER_PATTERN_RGGB:
    // copy original data
    for (i=0;i<sy;i+=2)
	for (j=0;j<sx;j+=2)
        {
          outB[(i*sx+j)*3]=src[i*sx+j];
          outR[((i+1)*sx+(j+1))*3]=src[(i+1)*sx+(j+1)];
          outG[(i*sx+j+1)*3]=src[i*sx+j+1];
          outG[((i+1)*sx+j)*3]=src[(i+1)*sx+j];
        }
    
    // R channel
    for (i=0;i<sy;i+=2)
	for (j=0;j<sx-1;j+=2)
        {
          outR[(i*sx+j)*3]=outR[((i+1)*sx+j+1)*3];
          outR[(i*sx+j+1)*3]=outR[((i+1)*sx+j+1)*3];
          outR[((i+1)*sx+j)*3]=outR[((i+1)*sx+j+1)*3];
        }
    
    // B channel
    for (i=0;i<sy-1;i+=2)//every two lines
	for (j=0;j<sx-1;j+=2)
        {
          outB[((i+1)*sx+j)*3]=outB[(i*sx+j)*3];
          outB[(i*sx+j+1)*3]=outB[(i*sx+j)*3];
          outB[((i+1)*sx+j+1)*3]=outB[(i*sx+j)*3];
        }
    
    // using lower direction for G channel
    
    // G channel
    for (i=0;i<sy-1;i+=2)//every two lines
	for (j=0;j<sx-1;j+=2)
            outG[(i*sx+j)*3]=outG[((i+1)*sx+j)*3];
    
    for (i=1;i<sy-2;i+=2)//every two lines
	for (j=0;j<sx-1;j+=2)
            outG[(i*sx+j+1)*3]=outG[((i+1)*sx+j+1)*3];
    
    // copy it for the next line
    for (j=0;j<sx-1;j+=2)
	outG[((sy-1)*sx+j+1)*3]=outG[((sy-2)*sx+j+1)*3];
    
    break;
    
  default:  //-------------------------------------------
    fprintf(stderr,"Bad Bayer pattern ID\n");
    break;
  }
}


/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::BayerEdgeSense( unsigned char *src,
                            unsigned char *dest,
                            int sx, int sy,
                            bayer_pattern_t type)
{
  unsigned char *outR, *outG, *outB;
  register int i,j;
  int dh, dv;
  int tmp;
  
  // sx and sy should be even
  switch (type)
  {
  case BAYER_PATTERN_GRBG:
  case BAYER_PATTERN_BGGR:
    outR=&dest[0];
    outG=&dest[1];
    outB=&dest[2];
    break;
  case BAYER_PATTERN_GBRG:
  case BAYER_PATTERN_RGGB:
    outR=&dest[2];
    outG=&dest[1];
    outB=&dest[0];
    break;
  default:
    outR=NULL;outG=NULL;outB=NULL;
    break;
  }
  
  switch (type)
  {
  case BAYER_PATTERN_GRBG://---------------------------------------------------------
  case BAYER_PATTERN_GBRG:
    // copy original RGB data to output images
    for (i=0;i<sy;i+=2)
	for (j=0;j<sx;j+=2)
        {
          outG[(i*sx+j)*3]=src[i*sx+j];
          outG[((i+1)*sx+(j+1))*3]=src[(i+1)*sx+(j+1)];
          outR[(i*sx+j+1)*3]=src[i*sx+j+1];
          outB[((i+1)*sx+j)*3]=src[(i+1)*sx+j];
        }
    
    // process GREEN channel
    for (i=3;i<sy-2;i+=2)
	for (j=2;j<sx-3;j+=2)
        {
          dh=abs((outB[(i*sx+j-2)*3]+outB[(i*sx+j+2)*3])/2-outB[(i*sx+j)*3]);
          dv=abs((outB[((i-2)*sx+j)*3]+outB[((i+2)*sx+j)*3])/2-outB[(i*sx+j)*3]);
          if (dh<dv)
	      tmp=(outG[(i*sx+j-1)*3]+outG[(i*sx+j+1)*3])/2;
          else
	      if (dh>dv)
                  tmp=(outG[((i-1)*sx+j)*3]+outG[((i+1)*sx+j)*3])/2;
	      else
                  tmp=(outG[(i*sx+j-1)*3]+outG[(i*sx+j+1)*3]+outG[((i-1)*sx+j)*3]+outG[((i+1)*sx+j)*3])/4;
          CLIP(tmp,outG[(i*sx+j)*3]);
        }
    
    for (i=2;i<sy-3;i+=2)
	for (j=3;j<sx-2;j+=2)
	  {
	    dh=abs((outR[(i*sx+j-2)*3]+outR[(i*sx+j+2)*3])/2-outR[(i*sx+j)*3]);
	    dv=abs((outR[((i-2)*sx+j)*3]+outR[((i+2)*sx+j)*3])/2-outR[(i*sx+j)*3]);
	    if (dh<dv)
                tmp=(outG[(i*sx+j-1)*3]+outG[(i*sx+j+1)*3])/2;
	    else
                if (dh>dv)
                    tmp=(outG[((i-1)*sx+j)*3]+outG[((i+1)*sx+j)*3])/2;
                else
                    tmp=(outG[(i*sx+j-1)*3]+outG[(i*sx+j+1)*3]+outG[((i-1)*sx+j)*3]+outG[((i+1)*sx+j)*3])/4;
	    CLIP(tmp,outG[(i*sx+j)*3]);
	  }
    
    // process RED channel
    for (i=0;i<sy-1;i+=2)
	for (j=2;j<sx-1;j+=2)
        {
          tmp=outG[(i*sx+j)*3]+(outR[(i*sx+j-1)*3]-outG[(i*sx+j-1)*3]+
                                outR[(i*sx+j+1)*3]-outG[(i*sx+j+1)*3])/2;
          CLIP(tmp,outR[(i*sx+j)*3]);
        }
    for (i=1;i<sy-2;i+=2)
    {
      for (j=1;j<sx;j+=2)
      {
        tmp=outG[(i*sx+j)*3]+(outR[((i-1)*sx+j)*3]-outG[((i-1)*sx+j)*3]+
                              outR[((i+1)*sx+j)*3]-outG[((i+1)*sx+j)*3])/2;
        CLIP(tmp,outR[(i*sx+j)*3]);
      }
      for (j=2;j<sx-1;j+=2)
      {
        tmp=outG[(i*sx+j)*3]+(outR[((i-1)*sx+j-1)*3]-outG[((i-1)*sx+j-1)*3]+
                              outR[((i-1)*sx+j+1)*3]-outG[((i-1)*sx+j+1)*3]+
                              outR[((i+1)*sx+j-1)*3]-outG[((i+1)*sx+j-1)*3]+
                              outR[((i+1)*sx+j+1)*3]-outG[((i+1)*sx+j+1)*3])/4;
        CLIP(tmp,outR[(i*sx+j)*3]);
      }
    }
    
    // process BLUE channel
    for (i=1;i<sy;i+=2)
	for (j=1;j<sx-2;j+=2)
        {
          tmp=outG[(i*sx+j)*3]+(outB[(i*sx+j-1)*3]-outG[(i*sx+j-1)*3]+
                                outB[(i*sx+j+1)*3]-outG[(i*sx+j+1)*3])/2;
          CLIP(tmp,outB[(i*sx+j)*3]);
        }
    
    for (i=2;i<sy-1;i+=2)
    {
      for (j=0;j<sx-1;j+=2)
      {
        tmp=outG[(i*sx+j)*3]+(outB[((i-1)*sx+j)*3]-outG[((i-1)*sx+j)*3]+
                              outB[((i+1)*sx+j)*3]-outG[((i+1)*sx+j)*3])/2;
        CLIP(tmp,outB[(i*sx+j)*3]);
      }
      for (j=1;j<sx-2;j+=2)
      {
        tmp=outG[(i*sx+j)*3]+(outB[((i-1)*sx+j-1)*3]-outG[((i-1)*sx+j-1)*3]+
                              outB[((i-1)*sx+j+1)*3]-outG[((i-1)*sx+j+1)*3]+
                              outB[((i+1)*sx+j-1)*3]-outG[((i+1)*sx+j-1)*3]+
                              outB[((i+1)*sx+j+1)*3]-outG[((i+1)*sx+j+1)*3])/4;
        CLIP(tmp,outB[(i*sx+j)*3]);
      }
    }
    break;
    
  case BAYER_PATTERN_BGGR: //---------------------------------------------------------
  case BAYER_PATTERN_RGGB:
    // copy original RGB data to output images
    for (i=0;i<sy;i+=2)
	for (j=0;j<sx;j+=2)
        {
          outB[(i*sx+j)*3]=src[i*sx+j];
          outR[((i+1)*sx+(j+1))*3]=src[(i+1)*sx+(j+1)];
          outG[(i*sx+j+1)*3]=src[i*sx+j+1];
          outG[((i+1)*sx+j)*3]=src[(i+1)*sx+j];
        }
    
    // process GREEN channel
    for (i=2;i<sy-2;i+=2)
	for (j=2;j<sx-3;j+=2)
        {
          dh=abs((outB[(i*sx+j-2)*3]+outB[(i*sx+j+2)*3])/2-outB[(i*sx+j)*3]);
          dv=abs((outB[((i-2)*sx+j)*3]+outB[((i+2)*sx+j)*3])/2-outB[(i*sx+j)*3]);
          if (dh<dv)
	      tmp=(outG[(i*sx+j-1)*3]+outG[(i*sx+j+1)*3])/2;
          else
	      if (dh>dv)
                  tmp=(outG[((i-1)*sx+j)*3]+outG[((i+1)*sx+j)*3])/2;
	      else
                  tmp=(outG[(i*sx+j-1)*3]+outG[(i*sx+j+1)*3]+outG[((i-1)*sx+j)*3]+outG[((i+1)*sx+j)*3])/4;
          CLIP(tmp,outG[(i*sx+j)*3]);
        }
    
    for (i=3;i<sy-3;i+=2)
	for (j=3;j<sx-2;j+=2)
        {
          dh=abs((outR[(i*sx+j-2)*3]+outR[(i*sx+j+2)*3])/2-outR[(i*sx+j)*3]);
          dv=abs((outR[((i-2)*sx+j)*3]+outR[((i+2)*sx+j)*3])/2-outR[(i*sx+j)*3]);
          if (dh<dv)
	      tmp=(outG[(i*sx+j-1)*3]+outG[(i*sx+j+1)*3])/2;
          else
	      if (dh>dv)
                  tmp=(outG[((i-1)*sx+j)*3]+outG[((i+1)*sx+j)*3])/2;
	      else
                  tmp=(outG[(i*sx+j-1)*3]+outG[(i*sx+j+1)*3]+outG[((i-1)*sx+j)*3]+outG[((i+1)*sx+j)*3])/4;
          CLIP(tmp,outG[(i*sx+j)*3]);
        }
    
    // process RED channel
    for (i=1;i<sy-1;i+=2) // G-points (1/2)
	for (j=2;j<sx-1;j+=2)
        {
          tmp=outG[(i*sx+j)*3]+(outR[(i*sx+j-1)*3]-outG[(i*sx+j-1)*3]+
                                outR[(i*sx+j+1)*3]-outG[(i*sx+j+1)*3])/2;
          CLIP(tmp,outR[(i*sx+j)*3]);
        }
    for (i=2;i<sy-2;i+=2) 
    {
      for (j=1;j<sx;j+=2) // G-points (2/2)
      {
        tmp=outG[(i*sx+j)*3]+(outR[((i-1)*sx+j)*3]-outG[((i-1)*sx+j)*3]+
                              outR[((i+1)*sx+j)*3]-outG[((i+1)*sx+j)*3])/2;
        CLIP(tmp,outR[(i*sx+j)*3]);
      }
	  for (j=2;j<sx-1;j+=2) // B-points
          {
            tmp=outG[(i*sx+j)*3]+(outR[((i-1)*sx+j-1)*3]-outG[((i-1)*sx+j-1)*3]+
                                  outR[((i-1)*sx+j+1)*3]-outG[((i-1)*sx+j+1)*3]+
                                  outR[((i+1)*sx+j-1)*3]-outG[((i+1)*sx+j-1)*3]+
                                  outR[((i+1)*sx+j+1)*3]-outG[((i+1)*sx+j+1)*3])/4;
            CLIP(tmp,outR[(i*sx+j)*3]);
	    }
    }
    
    // process BLUE channel
    for (i=0;i<sy;i+=2)
	for (j=1;j<sx-2;j+=2)
        {
          tmp=outG[(i*sx+j)*3]+(outB[(i*sx+j-1)*3]-outG[(i*sx+j-1)*3]+
				  outB[(i*sx+j+1)*3]-outG[(i*sx+j+1)*3])/2;
          CLIP(tmp,outB[(i*sx+j)*3]);
        }
    
    for (i=1;i<sy-1;i+=2)
    {
      for (j=0;j<sx-1;j+=2)
      {
        tmp=outG[(i*sx+j)*3]+(outB[((i-1)*sx+j)*3]-outG[((i-1)*sx+j)*3]+
                              outB[((i+1)*sx+j)*3]-outG[((i+1)*sx+j)*3])/2;
        CLIP(tmp,outB[(i*sx+j)*3]);
      }
      for (j=1;j<sx-2;j+=2)
      {
        tmp=outG[(i*sx+j)*3]+(outB[((i-1)*sx+j-1)*3]-outG[((i-1)*sx+j-1)*3]+
                              outB[((i-1)*sx+j+1)*3]-outG[((i-1)*sx+j+1)*3]+
                              outB[((i+1)*sx+j-1)*3]-outG[((i+1)*sx+j-1)*3]+
                              outB[((i+1)*sx+j+1)*3]-outG[((i+1)*sx+j+1)*3])/4;
        CLIP(tmp,outB[(i*sx+j)*3]);
      }
    }
    break;
  default: //---------------------------------------------------------
    fprintf(stderr,"Bad bayer pattern ID\n");
    break;
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::BayerDownsample( unsigned char *src,
                             unsigned char *dest,
                             int sx, int sy,
                             bayer_pattern_t type)
{
  unsigned char *outR, *outG, *outB;
  register int i,j;
  int tmp;
  
  // sx and sy should be even
  
  switch (type)
  {
  case BAYER_PATTERN_GRBG:
  case BAYER_PATTERN_BGGR:
    outR=&dest[0];
    outG=&dest[1];
    outB=&dest[2];
    break;
  case BAYER_PATTERN_GBRG:
  case BAYER_PATTERN_RGGB:
    outR=&dest[2];
    outG=&dest[1];
    outB=&dest[0];
    break;
  default:
    outR=NULL;outG=NULL;outB=NULL;
    break;
  }
  
  switch (type)
  {
  case BAYER_PATTERN_GRBG://---------------------------------------------------------
  case BAYER_PATTERN_GBRG:
    for (i=0;i<sy;i+=2)
	for (j=0;j<sx;j+=2)
        {
          tmp=((src[i*sx+j]+src[(i+1)*sx+(j+1)])>>1);
          CLIP(tmp,outG[(((i*sx)>>2)+(j>>1))*3]);
          tmp=src[i*sx+j+1];
          CLIP(tmp,outR[(((i*sx)>>2)+(j>>1))*3]);
          tmp=src[(i+1)*sx+j];
          CLIP(tmp,outB[(((i*sx)>>2)+(j>>1))*3]);
        }
    break;
  case BAYER_PATTERN_BGGR://---------------------------------------------------------
  case BAYER_PATTERN_RGGB:
    for (i=0;i<sy;i+=2)
	for (j=0;j<sx;j+=2)
        {
          tmp=((src[(i+1)*sx+j]+src[i*sx+(j+1)])>>1);
          CLIP(tmp,outG[(((i*sx)>>2)+(j>>1))*3]);
          tmp=src[(i+1)*sx+j+1];
          CLIP(tmp,outR[(((i*sx)>>2)+(j>>1))*3]);
          tmp=src[i*sx+j];
          CLIP(tmp,outB[(((i*sx)>>2)+(j>>1))*3]);
        }
    break;
  default: //---------------------------------------------------------
    fprintf(stderr,"Bad bayer pattern ID\n");
    break;
  }
  
}

// change a 16bit stereo image (8bit/channel) into two 8bit images on top
// of each other
/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::StereoDecode( unsigned char *src,
                          unsigned char *dest,
                          int NumPixels)
{
  register int i = (NumPixels<<1)-1;
  register int j = NumPixels-1;
  register int k = (NumPixels<<1)-1;
  
  while (i > 0)
  {
    dest[k--] = src[i--];
    dest[j--] = src[i--];
  }
}

void
libqlmbcam::ConvertYUV420( unsigned char *src, unsigned char *dest,
                           unsigned int width, unsigned int height)
{
  unsigned char r,g,b,Y,Cr,Cb;
  
  size_t p1,p2,p3,p4;
  int Cr_start = width*height;
  int Cb_start = Cr_start + (width*height/4);
  
  for (size_t y=0;y<height;y++)
  {
    for (size_t x=0;x<width;x++)
    {
      p1=(y*width) + (x);
      p2=((y/2)*width/2) + (x/2) + Cr_start;
      p3=((y/2)*width/2) + (x/2) + Cb_start;
      p4=(y*width*3) + (x*3);
      
      Y=src[p1];
      Cr=src[p2];
      Cb=src[p3];
      yuv2rgb (Y,Cr,Cb,&r,&g,&b);
      
      dest[p4]=r;
      dest[p4+1]=g;
      dest[p4+2]=b;
    }
  }
}

void
libqlmbcam::ConvertYVU420( unsigned char *src, unsigned char *dest,
                           unsigned int width, unsigned int height)
{
  unsigned char r,g,b,Y,Cr,Cb;
  
  size_t p1,p2,p3,p4;
  int Cb_start = width*height;
  int Cr_start = Cb_start + (width*height/4);
  
  for (size_t y=0;y<height;y++)
  {
    for (size_t x=0;x<width;x++)
    {
      p1=(y*width) + (x);
      p2=((y/2)*width/2) + (x/2) + Cr_start;
      p3=((y/2)*width/2) + (x/2) + Cb_start;
      p4=(y*width*3) + (x*3);
      
      Y=src[p1];
      Cr=src[p2];
      Cb=src[p3];
      yuv2rgb (Y,Cr,Cb,&r,&g,&b);
      
      dest[p4]=r;
      dest[p4+1]=g;
      dest[p4+2]=b;
    }
  }
}

#ifdef HAVE_LIBJPEG
void
libqlmbcam::ConvertJPEG( unsigned char* src, unsigned int nbytes,
                         unsigned char* dst)
{
  struct jpeg_decompress_struct cinfo;
  struct jpeg_error_mgr jerr;

  cinfo.err = jpeg_std_error(&jerr);	
  jpeg_create_decompress(&cinfo);
  jpeg_mem_src(&cinfo, src, nbytes);
  int rc = jpeg_read_header(&cinfo, TRUE);
  if (rc != 1)
  {
    fprintf(stderr, "File does not seem to be a normal JPEG\n");
    exit(EXIT_FAILURE);
  }
  jpeg_start_decompress(&cinfo);
  size_t width  = cinfo.output_width;
  size_t row_stride = width * cinfo.output_components;
  size_t bmp_size = row_stride;
  unsigned char* bmp_buffer = new unsigned char[bmp_size];
  unsigned char* buffer_array[1];
  buffer_array[0] = bmp_buffer;
  while (cinfo.output_scanline < cinfo.output_height)
  {
    jpeg_read_scanlines(&cinfo, buffer_array, 1);
    unsigned char* pos = buffer_array[0];
    memcpy( dst, pos, row_stride);
    dst += row_stride;
  }
  delete[] bmp_buffer;
  
  jpeg_finish_decompress(&cinfo);
  jpeg_destroy_decompress(&cinfo);
}
#else
void
libqlmbcam::ConvertJPEG( unsigned char* src, unsigned int nbytes,
                         unsigned char* dst)
{
  std::cout << "libjpeg not available" << std::endl;
}
#endif
