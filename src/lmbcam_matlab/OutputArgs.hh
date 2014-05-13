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

#include <mex.h>

class OutputArgs
{
public:
  
  OutputArgs( int nlhs, mxArray** plhs)
          :_nlhs( nlhs),
           _plhs( plhs)
        {}
  
  
  void assertNArgs( int nRequiredArgs) const
        {
          if( _nlhs != nRequiredArgs)
          {
            char errmsg[256];
            sprintf( errmsg, "Method '%s' returns %d output "
                     "argument(s) but you requested %d instead",
                     _methodName.c_str(), nRequiredArgs, _nlhs);
            mexErrMsgTxt( errmsg);
          }
        }
  
  mxArray*& operator[]( unsigned int argIndex)
        {
          mxAssert( argIndex < _nlhs, "output argument index out of range");
          return _plhs[argIndex];
        }
  
  void setValueUint( unsigned int argIndex, unsigned int value)
        {
          mxAssert( argIndex < _nlhs, "output argument index out of range");
          mxAssert( _plhs[argIndex] == NULL, 
                    "output argument already specified");
          _plhs[argIndex] = mxCreateNumericMatrix(1,1,
                                                  mxUINT32_CLASS, 
                                                  mxREAL);
          unsigned int* p = reinterpret_cast<unsigned int*>(
              mxGetPr(_plhs[argIndex]));
          *p = value;
        }
  
  void setValueDouble( unsigned int argIndex, double value)
        {
          mxAssert( argIndex < _nlhs, "output argument index out of range");
          mxAssert( _plhs[argIndex] == NULL, 
                    "output argument already specified");
          _plhs[argIndex] = mxCreateNumericMatrix(1,1,
                                                  mxDOUBLE_CLASS, 
                                                  mxREAL);
          double* p = reinterpret_cast<double*>(
              mxGetPr(_plhs[argIndex]));
          *p = value;
        }
  
  void setValueString( unsigned int argIndex, std::string value)
        {
          mxAssert( argIndex < _nlhs, "output argument index out of range");
          mxAssert( _plhs[argIndex] == NULL, 
                    "output argument already specified");
          _plhs[argIndex] = mxCreateString( value.c_str());
        }
  

  // just needed for nicer error messages
  void setMethodName( const std::string& name)
        {
          _methodName = name;
        }
  

private:
  
  int            _nlhs;
  mxArray**      _plhs;
  std::string    _methodName;
  
};
