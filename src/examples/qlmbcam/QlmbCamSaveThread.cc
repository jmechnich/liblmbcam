/**************************************************************************
**       Title: 
**    $RCSfile: QlmbCamSaveThread.cc,v $
**   $Revision: 1.4 $$Name:  $
**       $Date: 2005/05/23 07:41:30 $
**   Copyright: GPL $Author: ckeller $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: QlmbCamSaveThread.cc,v $
**  Revision 1.4  2005/05/23 07:41:30  ckeller
**  Added progress bars
**
**  Revision 1.3  2005/05/10 20:57:30  ckeller
**  Added progress bar
**
**  Revision 1.2  2005/05/04 12:09:05  ckeller
**  Now using thread to save images
**
**  Revision 1.1  2005/05/03 18:53:36  ckeller
**  initial version
**
**  Revision 1.1  2001/04/27 13:44:19  ckeller
**  initial revision
**
**
**************************************************************************/
#include "QlmbCamSaveThread.hh"
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>
#include <qapplication.h>
#include <assert.h>

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
libqlmbcam::QlmbCamSaveThread::QlmbCamSaveThread(unsigned char* ngrab_buffer,  
                                                 QString filename,
                                                 unsigned int grab_frameSize, 
                                                 unsigned int grab_numberFrames,
                                                 unsigned int width,
                                                 unsigned int height,
                                                 std::string colorCoding,
                                                 QObject* recv )
        : QThread(), _ngrab_buffer(ngrab_buffer), _filename(filename),
         _grab_frameSize(grab_frameSize), _grab_numberFrames(grab_numberFrames),
          _width(width), _height(height), _colorCoding(colorCoding),_recv(recv)
{}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamSaveThread::run()
{
  saveAsTIFF();
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void 
libqlmbcam::QlmbCamSaveThread::saveAsPGM()
{
  if ( _ngrab_buffer != 0 && !_filename.isEmpty() )
  {
    unsigned char* corpos = 0;
    std::string file_extension = "pgm";
    
    for (unsigned int actual_frame = 0; actual_frame < _grab_numberFrames;
         ++actual_frame) 
    {
    
      /*------------------------------------------------------------------
       * set pointer to the actual frame
       *------------------------------------------------------------------*/
      assert ( _ngrab_buffer != 0);
      
      corpos = _ngrab_buffer + (actual_frame * _grab_frameSize);
      
      if ( _colorCoding == "MONO" )
      {
        std::stringstream filename2;
        std::stringstream ext_length;
        ext_length << _grab_numberFrames;
      
        filename2 << _filename << "_";
        filename2.width(ext_length.str().length());
        filename2.fill('0');
        filename2 << actual_frame;
        filename2.width(0);
        filename2 << "." << file_extension ;
      
        std::ofstream file(filename2.str().c_str());
      
      
        file << "P5" << std::endl
             << _width << " " << _height << " 255" << std::endl;
      
        for ( unsigned int pos=0; pos < _grab_frameSize; ++pos)
        {
          file << corpos[pos];
        } //finished saving one image
        QApplication::postEvent(_recv,new SaveDoneEvent(actual_frame+1,_grab_numberFrames));
      }
      else
      {
        std::cerr << "File format not supportet" << std::endl;
      }
    } //finished saving all frames
    QApplication::postEvent(_recv,new SaveDoneEvent());
    assert ( _ngrab_buffer != 0);
    delete[] _ngrab_buffer; //We don't need the buffer anymore
  }
  else //no filename or buffer
  {
    QApplication::postEvent(_recv,new SaveDoneEvent());
    assert ( _ngrab_buffer != 0);
    delete[] _ngrab_buffer;
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamSaveThread::saveAsTIFF()
{
  assert( _ngrab_buffer !=0);
  if ( _ngrab_buffer !=0 )
  {
    unsigned char* current_image = 0;

    std::string file_extension = "tif";
    std::string filename = _filename;
    TIFF *tiff = TIFFOpen(filename.append(".").append(file_extension).c_str(),"w");

    if (tiff)
    {
      /*--------------------------------------------------------------------
       *  Start saving all frames
       *--------------------------------------------------------------------*/
      for ( unsigned int actual_frame = 0; actual_frame < _grab_numberFrames;
            ++actual_frame)
      {
        /*------------------------------------------------------------------
         *  Set basic tiff tags
         *-----------------------------------------------------------------*/
        TIFFSetField(tiff, TIFFTAG_IMAGEWIDTH, _width);
        TIFFSetField(tiff, TIFFTAG_IMAGELENGTH, _height);
        TIFFSetField(tiff, TIFFTAG_BITSPERSAMPLE, 8);
        TIFFSetField(tiff, TIFFTAG_SAMPLESPERPIXEL, 1); //1: for grayscale
        
        TIFFSetField(tiff, TIFFTAG_COMPRESSION, 1); // no compression
      
        TIFFSetField(tiff, TIFFTAG_PLANARCONFIG, 1); //1 = Chunky format 
        TIFFSetField(tiff, TIFFTAG_PHOTOMETRIC, 1); //1 = BlackIsZero

        TIFFSetField(tiff, TIFFTAG_ROWSPERSTRIP, _height); //need to check this 
        TIFFSetField(tiff, TIFFTAG_FILLORDER, FILLORDER_MSB2LSB);
        //set the current imagenumber in the slice
        TIFFSetField(tiff, TIFFTAG_PAGENUMBER, actual_frame, _grab_numberFrames);   


        // update pointer positon to the actual frame 
        current_image = _ngrab_buffer + ( actual_frame * _grab_frameSize );
        //now save the image
//          char *tmpline = (char*)_TIFFmalloc(_width * sizeof(char*));
//          unsigned char* tmp = current_image;
       
//          for (unsigned int row = 0; row < _height; ++row)
//          {
//            tmp = current_image + row * _width;
//            _TIFFmemcpy(tmpline, tmp, _width);
//            TIFFWriteScanline(tiff,tmp,row);
//          }
        

        TIFFWriteEncodedStrip(tiff, 0, current_image, _width * _height);
        //not sure about the 0 NEED to fix
        
        TIFFWriteDirectory(tiff); //finished saving actual image to tiff
        
        QApplication::postEvent(_recv,
                              new SaveDoneEvent(actual_frame+1,_grab_numberFrames));
      } 
      TIFFClose(tiff);
    }
    else // (tiff == NULL)
    {
      std::cerr << __PRETTY_FUNCTION__ << ":tiff == NULL " << std::endl;   
    }
  }
  else //_ngrab_buffer == 0
  {
    std::cerr << __PRETTY_FUNCTION__ << ": _ngrab_buffer == 0" << std::endl;   
  }

  QApplication::postEvent(_recv,new SaveDoneEvent()); //finished with images
  delete[] _ngrab_buffer; // so delete the buffer
}

      
