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

#ifndef LMBERROR_HH
#define LMBERROR_HH

#include <string>
#include <sstream>
#include <iostream>

namespace liblmbcam
{
  
/*======================================================================*/
/*!
 *  \class LMBError LMBError.hh
 *  \brief The LMBError class is the base class of all liblmbcam error classes
 */
/*======================================================================*/
  class LMBError
  {
  public:
    LMBError( bool critical=false)
            :_str( ""), _critical( critical)
          {}
    LMBError( const std::string& s, bool critical=false)
            :_str( s), _critical( critical)
          {}

    std::string str() const
          {
            return _str;
          }
  
    void setStr( const std::string& s )
          {
            _str = s;
          }
  
    bool isCritical() const
          {
            return _critical;
          }
  
    template<class T>
    LMBError& operator+=( const T& t)
          {
            std::ostringstream oss;
            oss << t;
            _str += oss.str();
            return *this;
          }
    
    LMBError& operator<<( const std::string& s)
          {
            _str += s;
            return *this;
          }
    
    template<class T>
    LMBError& operator<<( const T& t)
          {
            return operator+=( t);
          }
    
  protected:
    std::string _str;
    bool _critical;
  };

/*======================================================================*/
/*!
 *  \class LMBCriticalError LMBError.hh
 *  \brief The LMBCriticalError class is the base class of all critical 
 *         liblmbcam error classes.
 */
/*======================================================================*/
  class LMBCriticalError : public LMBError
  {
  public:
    LMBCriticalError()
            :LMBError( true)
          {}

    LMBCriticalError( const std::string& s)
            :LMBError( s, true)
          {}
  };
}

#endif
