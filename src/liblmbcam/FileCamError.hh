/**************************************************************************
**       Title: 
**    $RCSfile: FileCamError.hh,v $
**   $Revision: 1.1 $$Name:  $
**       $Date: 2003/11/05 16:31:39 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: FileCamError.hh,v $
**  Revision 1.1  2003/11/05 16:31:39  mechnich
**  initial revision
**
**
**
**************************************************************************/

#ifndef FILECAMERROR_HH
#define FILECAMERROR_HH

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
 *  \class FileCamError FileCamError.hh
 *  \brief The FileCamError class is the standard class for FileCam errors
 */
/*======================================================================*/
  class FileCamError : public LMBCamError
  {
  public:
/*======================================================================*/
/*! 
 *   Constructor
 *
 *   \param errmsg error message
 */
/*======================================================================*/
    FileCamError( const std::string& errmsg)
            :LMBCamError( errmsg)
          {}
  };
}

#endif // FILECAMERROR_HH
