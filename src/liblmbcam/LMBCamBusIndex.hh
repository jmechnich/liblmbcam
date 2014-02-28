/**************************************************************************
**       Title: 
**    $RCSfile: LMBCamBusIndex.hh,v $
**   $Revision: 1.3 $$Name:  $
**       $Date: 2004/06/29 16:27:53 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: LMBCamBusIndex.hh,v $
**  Revision 1.3  2004/06/29 16:27:53  mechnich
**  added getFirstCamera()
**
**  Revision 1.2  2004/06/06 17:03:21  mechnich
**  *** empty log message ***
**
**  Revision 1.1  2004/05/30 20:54:20  mechnich
**  initial revision
**
**
**
**************************************************************************/

#ifndef LMBCAMBUSINDEX_HH
#define LMBCAMBUSINDEX_HH

#include <string>
#include <map>

#include <LMBCam.hh>

namespace liblmbcam
{
  class LMBCamBus;
  
  class LMBCamBusIndex
  {
  public:
    LMBCamBusIndex();
    ~LMBCamBusIndex();
    LMBCam* findCamera( const std::string& guid) const;
    LMBCam* getFirstCamera() const;
    
  private:
    std::map<std::string,LMBCamBus*> _busses;
  };
}

#endif
