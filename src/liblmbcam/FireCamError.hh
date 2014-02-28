/**************************************************************************
**       Title: 
**    $RCSfile: FireCamError.hh,v $
**   $Revision: 1.5 $$Name:  $
**       $Date: 2006/06/04 12:26:50 $
**   Copyright: GPL $Author: jhense $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: FireCamError.hh,v $
**  Revision 1.5  2006/06/04 12:26:50  jhense
**  Throwing FireCamLostDMAPacketError when dma_single_capture fails.
**
**  Revision 1.4  2004/10/19 05:53:35  mechnich
**  changed LMBError interface
**
**  Revision 1.3  2003/12/02 16:35:46  mechnich
**  just for maintenance
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

#ifndef FIRECAMERROR_HH
#define FIRECAMERROR_HH

/*-------------------------------------------------------------------------
 *  Own includes
 *-------------------------------------------------------------------------*/
#include "LMBCamError.hh"

/*-------------------------------------------------------------------------
 *  STL includes
 *-------------------------------------------------------------------------*/
#include <string>

namespace liblmbcam
{
  
/*======================================================================*/
/*!
 *  \class FireCamError FireCamError.hh
 *  \brief The FireCamError class is the standard class for FireCam errors
 */
/*======================================================================*/
class FireCamError : public LMBCamError
{
public:
/*======================================================================*/
/*! 
 *   Constructor
 *
 *   \param errmsg error message
 */
/*======================================================================*/
  FireCamError()
          :LMBCamError()
        {}

  FireCamError( const std::string& s)
          :LMBCamError( s)
        {}
};
 
/*======================================================================*/
/*!
 *  \class FireCamCriticalError FireCamError.hh
 *  \brief The FireCamCriticalError class is the standard class for critical
 *         FireCam errors.
 */
/*======================================================================*/
class FireCamCriticalError : public LMBCamCriticalError
{
public:
/*======================================================================*/
/*! 
 *   Constructor
 *
 *   \param errmsg error message
 */
/*======================================================================*/
  FireCamCriticalError()
          :LMBCamCriticalError()
        {}

  FireCamCriticalError( const std::string& s)
          :LMBCamCriticalError( s)
        {}
};
 
/*======================================================================*/
/*!
 *  \class FireCamBusError FireCamError.hh
 *  \brief The FireCamBusError class is the standard class for
 *         FireCamBus errors.
 */
/*======================================================================*/
class FireCamBusError : public LMBCamBusError
{
public:
/*======================================================================*/
/*! 
 *   Constructor
 *
 *   \param errmsg error message
 */
/*======================================================================*/
  FireCamBusError()
          :LMBCamBusError()
        {}

  FireCamBusError( const std::string& s)
          :LMBCamBusError( s)
        {}
};

/*======================================================================*/
/*!
 *  \class FireCamLostDMAPacketError
 *  \brief The FireCamBusError class is the standard class for
 *         FireCamBus errors.
 */
/*======================================================================*/
class FireCamLostDMAPacketError : FireCamError
{
public:
  
  FireCamLostDMAPacketError()
      { };
  
  
  FireCamLostDMAPacketError(const std::string& s) 
      : FireCamError(s)
      { }
};

 
}

#endif

