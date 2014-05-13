// This file is part of liblmbcam.
//
// liblmbcam is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// liblmbcam is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with liblmbcam.  If not, see <http://www.gnu.org/licenses/>.

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
  
  void setMode( int mode);
  int mode() const;
  
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
  
  static LMBErrorHandler* ErrorHandler();

private:
  LMBErrorHandler();

  int p_mode;
  static LMBErrorHandler* p_errorHandler;
  static pthread_mutex_t p_mutex;
};
 
}

#endif
