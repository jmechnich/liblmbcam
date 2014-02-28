/**************************************************************************
**       Title: 
**    $RCSfile: LMBErrorHandler.cc,v $
**   $Revision: 1.5 $$Name:  $
**       $Date: 2004/02/20 22:37:54 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: LMBErrorHandler.cc,v $
**  Revision 1.5  2004/02/20 22:37:54  mechnich
**  added MutexLocker class
**
**  Revision 1.4  2003/12/02 16:35:46  mechnich
**  just for maintenance
**
**  Revision 1.3  2003/06/12 16:13:19  mechnich
**  added comments
**
**  Revision 1.2  2003/05/08 07:54:24  ronneber
**  - made handle() a member template, otherwise only objects of class
**    LMBError were thrown
**
**  Revision 1.1  2002/12/04 13:17:46  mechnich
**  initial revision
**
**
**
**************************************************************************/

#include "LMBErrorHandler.hh"

liblmbcam::LMBErrorHandler* liblmbcam::LMBErrorHandler::p_errorHandler=0;

pthread_mutex_t liblmbcam::LMBErrorHandler::p_mutex=PTHREAD_MUTEX_INITIALIZER;

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:LMBErrorHandler::LMBErrorHandler()
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::LMBErrorHandler::LMBErrorHandler()
        :p_mode( WARNING)
{}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:LMBErrorHandler::setMode( int mode)
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::LMBErrorHandler::setMode( int mode)
{
  MutexLocker locker( &p_mutex);
  p_mode = mode;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:LMBErrorHandler::mode() const
 *  ==> see headerfile
 *=======================================================================*/
int
liblmbcam::LMBErrorHandler::mode() const
{
  MutexLocker locker( &p_mutex);
  return p_mode;
}


/*=========================================================================
 *  DESCRIPTION OF FUNCTION:LMBErrorHandler::ErrorHandler()
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::LMBErrorHandler*
liblmbcam::LMBErrorHandler::ErrorHandler()
{
  MutexLocker locker( &p_mutex);
  if( p_errorHandler == 0)
  {
    p_errorHandler = new LMBErrorHandler();
  }
  LMBErrorHandler* ret = p_errorHandler;
  
  return ret;
}

