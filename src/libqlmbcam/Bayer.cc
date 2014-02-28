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

#include "Bayer.hh"

#include <cstdlib>
#include <cstdio>
#include <string>

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
