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

#include "LMBErrorHandler.hh"

liblmbcam::LMBErrorHandler*
liblmbcam::LMBErrorHandler::p_errorHandler=0;

pthread_mutex_t
liblmbcam::LMBErrorHandler::p_mutex=PTHREAD_MUTEX_INITIALIZER;

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

