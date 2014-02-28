/**************************************************************************
**       Title: 
**    $RCSfile: LMBCamFileIO.hh,v $
**   $Revision: 1.5 $$Name:  $
**       $Date: 2004/05/28 12:16:27 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: LMBCamFileIO.hh,v $
**  Revision 1.5  2004/05/28 12:16:27  mechnich
**  changed available image formats tp ppm/pgm
**
**  Revision 1.4  2003/12/18 15:33:19  mechnich
**  sync
**
**  Revision 1.3  2003/12/02 16:35:46  mechnich
**  just for maintenance
**
**  Revision 1.2  2003/11/05 16:57:49  mechnich
**  corrected dumb error
**
**  Revision 1.1  2003/11/05 16:22:07  mechnich
**  initial revision
**
**
**
**************************************************************************/

#ifndef LMBCAMFILEIO_HH
#define LMBCAMFILEIO_HH

#include <iostream>
#include <vector>

namespace liblmbcam
{
  class LMBCamFileIO
  {
  public:
    static unsigned char* loadImageFile( std::istream& is,
                                         std::string format="auto");

    static bool saveImageFile( std::ostream& os, unsigned char* data,
                               unsigned int width, unsigned int height,
                               std::string colorCoding, double bytePerPixel,
                               std::string format="auto");
  private:
    LMBCamFileIO()
          {
            _formats.push_back( "pgm");
            _formats.push_back( "ppm");
          }
    
    static std::vector<std::string> _formats;
    
    static LMBCamFileIO _singleton;
  };
}

#endif // LMBCAMFILEIO_HH
