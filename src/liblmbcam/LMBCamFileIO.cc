/**************************************************************************
**       Title: 
**    $RCSfile: LMBCamFileIO.cc,v $
**   $Revision: 1.2 $$Name:  $
**       $Date: 2003/12/18 15:33:19 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: LMBCamFileIO.cc,v $
**  Revision 1.2  2003/12/18 15:33:19  mechnich
**  sync
**
**  Revision 1.1  2003/11/05 16:22:07  mechnich
**  initial revision
**
**
**
**************************************************************************/

#include "LMBCamFileIO.hh"

std::vector<std::string> liblmbcam::LMBCamFileIO::_formats;

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
unsigned char*
liblmbcam::LMBCamFileIO::loadImageFile( std::istream& is,
                                        std::string format)
{
  if( format == "auto")
  {
  }
  else
  {
    std::vector<std::string>::iterator it = 
        std::find( _formats.begin(), _formats.end(), format);
    if( it != _formats.end())
    {
    }
  }
  
  return false;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
bool
liblmbcam::LMBCamFileIO::saveImageFile( std::ostream& os, unsigned char* data,
                                        unsigned int width,
                                        unsigned int height,
                                        std::string colorCoding,
                                        double bytePerPixel,
                                        std::string format)
{
  if( format == "auto")
  {
  }
  else
  {
    std::vector<std::string>::iterator it = 
        std::find( _formats.begin(), _formats.end(), format);
    if( it != _formats.end())
    {
    }
  }
  
  return false;
}


