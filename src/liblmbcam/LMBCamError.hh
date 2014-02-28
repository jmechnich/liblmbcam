/**************************************************************************
**       Title: 
**    $RCSfile: LMBCamError.hh,v $
**   $Revision: 1.5 $$Name:  $
**       $Date: 2004/10/19 05:53:35 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: LMBCamError.hh,v $
**  Revision 1.5  2004/10/19 05:53:35  mechnich
**  changed LMBError interface
**
**  Revision 1.4  2003/12/05 14:14:55  mechnich
**  added missing comments and updated Doxyfile
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

#ifndef LMBCAMERROR_HH
#define LMBCAMERROR_HH

/*-------------------------------------------------------------------------
 *  Own includes
 *-------------------------------------------------------------------------*/
#include "LMBError.hh"

/*-------------------------------------------------------------------------
 *  STL includes
 *-------------------------------------------------------------------------*/
#include <string>

namespace liblmbcam
{
  
/*======================================================================*/
/*!
 *  \class LMBCamError LMBCamError.hh
 *  \brief The LMBCamError class is the base class of all camera error classes
 */
/*======================================================================*/
  class LMBCamError : public LMBError
  {
  public:
/*======================================================================*/
/*! 
 *   Constructor
 *
 *   \param msg the error message string
 */
/*======================================================================*/
    LMBCamError()
            :LMBError()
          {}

    LMBCamError( const std::string& s)
            :LMBError( s)
          {}
  };

/*======================================================================*/
/*!
 *  \class LMBCamCriticalError LMBCamError.hh
 *  \brief The LMBCamCriticalError class is the base class of all critical 
 *         camera error classes
 */
/*======================================================================*/
  class LMBCamCriticalError : public LMBCriticalError
  {
public:
/*======================================================================*/
/*! 
 *   Constructor
 *
 *   \param msg the error message string
 */
/*======================================================================*/
    LMBCamCriticalError()
            :LMBCriticalError()
          {}

    LMBCamCriticalError( const std::string& s)
            :LMBCriticalError( s)
          {}
  };

/*======================================================================*/
/*!
 *  \class LMBCamBusError LMBCamError.hh
 *  \brief The LMBCamBusError class is the base class for all bus error classes
 */
/*======================================================================*/
  class LMBCamBusError : public LMBCamCriticalError
  {
  public:
/*======================================================================*/
/*! 
 *   Constructor
 *
 *   \param errmsg error message
 */
/*======================================================================*/
    LMBCamBusError()
            :LMBCamCriticalError()
          {}

    LMBCamBusError( const std::string& s)
            :LMBCamCriticalError( s)
          {}
  };
  
/*======================================================================*/
/*!
 *  \class LMBCamFileIOError LMBCamError.hh
 *  \brief The LMBCamFileIOError class is the base class for all IO error
 *         classes
 */
/*======================================================================*/
  class LMBCamFileIOError : public LMBCamError
  {
  public:
/*======================================================================*/
/*! 
 *   Constructor
 *
 *   \param errmsg error message
 */
/*======================================================================*/
    LMBCamFileIOError()
            :LMBCamError()
          {}

    LMBCamFileIOError( const std::string& s)
            :LMBCamError( s)
          {}
  };
}

#endif
