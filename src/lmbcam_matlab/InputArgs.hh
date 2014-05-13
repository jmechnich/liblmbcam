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

#include <string>
#include <mex.h>

class InputArgs
{
public:
  
  InputArgs( int nrhs, const mxArray** prhs);
  
  void assertNArgs( int nRequiredArgs) const;
  double asDouble( int argIndex) const;
  unsigned int asUint( int argIndex) const;
  std::string asString( int argIndex) const;
  
  const mxArray* operator[]( unsigned int argIndex) const
        {
          mxAssert( argIndex < _nrhs, "input argument index out of range");
          return _prhs[argIndex];
        }
  
  const std::string& methodName() const
        {
          return _methodName;
        }
  
  unsigned int uniqueObjectID() const
        {
          return _uniqueObjectID;
        }
  

private:
  
  int            _nrhs;
  const mxArray** _prhs;
  std::string    _methodName;
  unsigned int   _uniqueObjectID;
  
};
