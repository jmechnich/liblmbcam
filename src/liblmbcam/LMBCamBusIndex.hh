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

#include <LMBCamBus.hh>

namespace liblmbcam
{
  class LMBCamBusIndex : public LMBCamBus
  {
  public:
    enum BusType
    {
        None=0x0,
        FireCam=0x1,
        V4LCam=0x2,
        All=0x3
    };
    
    LMBCamBusIndex( BusType type=All);
    ~LMBCamBusIndex();

/*======================================================================*/
/*! 
 *   This function should return the number of cameras attached to the bus
 *
 *   \return number of cameras
 */
/*======================================================================*/
    unsigned int nCameras() const;

/*======================================================================*/
/*! 
 *   This function should return a pointer to camera <em>index</em>
 *
 *   \param index  camera index
 *   \exception throws LMBCamBusError_OutOfBounds camera <em>index</em>
 *              does not exist
 *   \return pointer to camera <em>index</em>
 */
/*======================================================================*/
    LMBCam* cameraByIndex( unsigned int index) const;

/*======================================================================*/
/*! 
 *   This function should return a pointer to the camera
 *   with GUID <em>guid</em>.
 *
 *   \param guid camera GUID
 *   \return pointer to camera with GUID <em>guid</em>
 */
/*======================================================================*/
    LMBCam* cameraByGUID( const std::string& guid) const;

  private:
    std::map<std::string,LMBCamBus*> _busses;
    BusType _type;
  };
}

#endif
