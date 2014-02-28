/**************************************************************************
**       Title: 
**    $RCSfile: FileCam.cc,v $
**   $Revision: 1.2 $$Name:  $
**       $Date: 2003/12/18 15:34:33 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: FileCam.cc,v $
**  Revision 1.2  2003/12/18 15:34:33  mechnich
**  changed return value of bytePerPixel from float to double
**
**  Revision 1.1  2003/11/05 16:31:39  mechnich
**  initial revision
**
**
**
**************************************************************************/

#include "FileCam.hh"

#include "LMBCamFileIO.hh"

#include <fstream>

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::FileCam::FileCam()
        :p_filename(""), p_data(0), p_colorCoding("MONO"), p_framerate(15),
         p_left(0), p_top(0), p_width(0), p_height(0),
         p_maxWidth(0), p_maxHeight(0),
         p_readFrameCount(0), p_writtenFrameCount(0)
{}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::FileCam::setFile( const std::string& fileName)
{
  if( p_cameraIsRunning)
  {
    stopCamera();
  }
  std::ifstream file( fileName.c_str());
  if( !file.is_open())
  {
    p_filename = fileName;
    //liblmbcam::LMBCamFileIO::loadImageFile( file, p_data);
  }
  else
  {
    std::cout << "Unable to read from file " << fileName << std::endl;
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::FileCam::~FileCam()
{
  if( p_cameraIsRunning)
  {
    stopCamera();
  }
  if( p_data != 0)
  {
    delete[] p_data;
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::FileCam::startCamera()
{
  if( p_cameraIsRunning)
      stopCamera();
  
  p_cameraIsRunning = true;
}

