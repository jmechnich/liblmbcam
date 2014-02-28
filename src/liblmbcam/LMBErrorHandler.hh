/**************************************************************************
**       Title: 
**    $RCSfile: LMBErrorHandler.hh,v $
**   $Revision: 1.6 $$Name:  $
**       $Date: 2004/10/19 05:53:35 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: LMBErrorHandler.hh,v $
**  Revision 1.6  2004/10/19 05:53:35  mechnich
**  changed LMBError interface
**
**  Revision 1.5  2004/02/20 22:37:54  mechnich
**  added MutexLocker class
**
**  Revision 1.4  2003/12/02 16:35:46  mechnich
**  just for maintenance
**
**  Revision 1.3  2003/06/12 16:13:19  mechnich
**  added comments
**
**  Revision 1.2  2003/05/08 07:54:37  ronneber
**  - made handle() a member template, otherwise only objects of class
**    LMBError were thrown
**
**  Revision 1.1  2002/12/04 13:17:46  mechnich
**  initial revision
**
**
**
**************************************************************************/

#ifndef LMBERRORHANDLER_HH
#define LMBERRORHANDLER_HH

#include "LMBError.hh"

#include "MutexLocker.hh"

namespace liblmbcam
{
  
/*======================================================================*/
/*!
 *  \class LMBErrorHandler LMBErrorHandler.hh
 *  \brief The LMBErrorHandler class makes it possible to change the global
 *         behaviour of a program using the liblmbcam in case an error occurs.
 */
/*======================================================================*/
class LMBErrorHandler
{
public:
  enum
  {
      QUIET,
      WARNING,
      THROW
  };
  
/*======================================================================*/
/*! 
 *   This function sets the handler's mode to <em>mode</em> where valid modes
 *   are <em>QUIET</em>, <em>WARNING</em> and <em>THROW</em>.
 *
 *   \param mode one of the above handler modes
 */
/*======================================================================*/
  void setMode( int mode);

/*======================================================================*/
/*! 
 *   This function returns the current mode of the handler.
 *
 *   \return <em>QUIET</em>, <em>WARNING</em> or <em>THROW</em>
 */
/*======================================================================*/
  int mode() const;
  
/*======================================================================*/
/*! 
 *   This template function handles the error passed as argument according
 *   to the handlers mode.
 *   If the mode is <em>QUIET</em>, nothing is done.
 *   If the mode is <em>WARNING</em>, the error message is printed to stderr.
 *   If the mode is <em>THROW</em>, the error is thrown and can be caught
 *   in the main program.
 *
 *   \param error the error which should be handled
 */
/*======================================================================*/
  template<typename ERRORCLASS>
  void handle( ERRORCLASS& error)
        {
          MutexLocker locker( &p_mutex);
          if( error.isCritical())
          {
            throw error;
          }
          
          switch( p_mode)
          {
          case( QUIET):
            break;
          case( WARNING):
            std::cerr << error.str() << std::endl;
            break;
          case( THROW):
            throw error;
            break;
          default:
            break;
          }
        }
  

/*======================================================================*/
/*! 
 *   This static function ensures that only one error handler object exists.
 *
 *   \return pointer to the global error handler
 */
/*======================================================================*/
  static LMBErrorHandler* ErrorHandler();

private:
/*======================================================================*/
/*! 
 *   Default Constructor
 */
/*======================================================================*/
  LMBErrorHandler();

  int p_mode;

  static LMBErrorHandler* p_errorHandler;

  static pthread_mutex_t p_mutex;
};
 
}

#endif
