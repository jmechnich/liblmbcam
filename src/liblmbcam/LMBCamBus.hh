/**************************************************************************
**       Title: 
**    $RCSfile: LMBCamBus.hh,v $
**   $Revision: 1.4 $$Name:  $
**       $Date: 2003/12/05 14:14:55 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: LMBCamBus.hh,v $
**  Revision 1.4  2003/12/05 14:14:55  mechnich
**  added missing comments and updated Doxyfile
**
**  Revision 1.3  2003/07/03 15:25:41  mechnich
**  added boolean return values to set...() functions
**
**  Revision 1.2  2003/06/12 16:13:19  mechnich
**  added comments
**
**  Revision 1.1  2002/12/04 13:17:46  mechnich
**  initial revision
**
**
**
**************************************************************************/

#ifndef LMBCAMBUS_HH
#define LMBCAMBUS_HH

/*-------------------------------------------------------------------------
 *  Own includes
 *-------------------------------------------------------------------------*/
#include "LMBCam.hh"

/*-------------------------------------------------------------------------
 *  STL includes
 *-------------------------------------------------------------------------*/
#include <vector>

namespace liblmbcam
{

/*======================================================================*/
/*!
 *  \class LMBCamBus LMBCamBus.hh
 *  \brief The LMBCamBus class specifies the interface for all bus classes
 */
/*======================================================================*/
class LMBCamBus
{
public:
/*======================================================================*/
/*! 
 *   Default Constructor
 */
/*======================================================================*/
  LMBCamBus()
        {}
  
/*======================================================================*/
/*! 
 *   Destructor
 */
/*======================================================================*/
  virtual ~LMBCamBus()
        {}
  
/*======================================================================*/
/*! 
 *   This function should return the number of cameras attached to the bus
 *
 *   \return number of cameras
 */
/*======================================================================*/
  virtual unsigned int nCameras() const=0;

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
  virtual LMBCam* cameraByIndex( unsigned int index) const=0;

/*======================================================================*/
/*! 
 *   This function should return a pointer to the camera
 *   with GUID <em>guid</em>.
 *
 *   \param guid camera GUID
 *   \return pointer to camera with GUID <em>guid</em>
 */
/*======================================================================*/
  virtual LMBCam* cameraByGUID( const std::string& guid) const=0;
};
 
}

#endif
