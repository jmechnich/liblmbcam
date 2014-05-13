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

