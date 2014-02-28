/**************************************************************************
**       Title: 
**    $RCSfile: FireCamBus.hh,v $
**   $Revision: 1.7 $$Name:  $
**       $Date: 2004/01/26 21:33:41 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: FireCamBus.hh,v $
**  Revision 1.7  2004/01/26 21:33:41  mechnich
**  added FireCamBus::available() function
**
**  Revision 1.6  2003/08/05 14:41:01  mechnich
**  removed inclusion of libraw header
**
**  Revision 1.5  2003/07/03 15:25:17  mechnich
**  - added comments
**  - improved Format7 capabilities
**  - added boolean return values to set...() functions
**
**  Revision 1.4  2003/06/12 16:13:19  mechnich
**  added comments
**
**  Revision 1.3  2003/05/09 14:22:41  ronneber
**  - releases bus correctly when exception in constructor occurs
**
**  Revision 1.2  2003/05/07 00:06:29  mechnich
**  intorduced a wrapper mutex for critical libraw1394 functions,
**  liblmbcam should now be completely thread safe
**
**  Revision 1.1  2002/12/04 13:17:46  mechnich
**  initial revision
**
**
**
**************************************************************************/

#ifndef FIRECAMBUS_HH
#define FIRECAMBUS_HH

/*-------------------------------------------------------------------------
 *  Own includes
 *-------------------------------------------------------------------------*/
#include "LMBCamBus.hh"
#include "LMBErrorHandler.hh"
#include "FireCam.hh"
#include "FireCamError.hh"

/*-------------------------------------------------------------------------
 *  FireWire includes
 *-------------------------------------------------------------------------*/
#include <libdc1394/dc1394_control.h>

/*-------------------------------------------------------------------------
 *  STL includes
 *-------------------------------------------------------------------------*/
#include <vector>
#include <sstream>

#include <pthread.h>

namespace liblmbcam
{
/*======================================================================*/
/*!
 *  \class FireCamBus FireCamBus.hh
 *  \brief The FireCamBus class represents a ieee1394 bus which allows
 *         access to connected ieee1394 cameras.
 *
 *  
 */
/*======================================================================*/
class FireCamBus : public LMBCamBus
{
public:
/*======================================================================*/
/*! 
 *   Default Constructor
 */
/*======================================================================*/
  FireCamBus( unsigned int busIndex=0);

/*======================================================================*/
/*! 
 *   Destructor
 */
/*======================================================================*/
  ~FireCamBus();
  
/*======================================================================*/
/*! 
 *   This function scans the bus for connected cameras. If
 *   <em>killHandleIfHighestNode</em> is true, the hardware handle of the
 *   bus is deleted if a camera is the highest node on the bus.
 *
 *   \param killHandleIfHighestNode If set to true the hardware handle of the
 *          bus is deleted if a camera is the highest node on the bus.
 */
/*======================================================================*/
  void rescan( bool killHandleIfHighestNode=false);

/*======================================================================*/
/*! 
 *   This function returns the number of cameras attached to the bus.
 *
 *   \return number of cameras
 */
/*======================================================================*/
  unsigned int nCameras() const;

/*======================================================================*/
/*! 
 *   This function returns a pointer to the camera at index <em>index</em>.
 *
 *   \param index camera index 
 *   \return pointer to camera <em>index</em>
 */
/*======================================================================*/
  LMBCam* cameraByIndex( unsigned int index) const;

/*======================================================================*/
/*! 
 *   This function returns a pointer to the camera with GUID <em>guid</em>.
 *
 *   \param guid camera GUID
 *   \return pointer to camera with GUID <em>guid</em>
 */
/*======================================================================*/
  LMBCam* cameraByGUID( const std::string& guid) const;

  static std::vector<unsigned int> available();
  
private:
/*-------------------------------------------------------------------------
 *  Copy constructor and assignment operator are private, so we don't have
 *  to take care of pointers
 *-------------------------------------------------------------------------*/
  FireCamBus( const FireCamBus&);
  void operator=( const FireCamBus&);

  unsigned int p_busIndex;

  std::vector<FireCam*> p_cameras;
  
  raw1394handle_t p_handle;

  pthread_mutex_t p_libRawMutex;
};

}

#include "FireCamBus.icc"

#endif
