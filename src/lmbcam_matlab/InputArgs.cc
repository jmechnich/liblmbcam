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

#include "InputArgs.hh"

InputArgs::InputArgs( int nrhs, const mxArray** prhs)
          : _nrhs( nrhs),
            _prhs( prhs)
{
  /*-----------------------------------------------------------------
   *   extract method name (first right hand side argument)
   *-----------------------------------------------------------------*/
  if( _nrhs < 2 || !mxIsChar( _prhs[0]))
  {
    mexErrMsgTxt( "lmbcam_matlab requires \n"
                  " argument 0: method name as string\n"
                  " argument 1: unique object ID or 0 for global/static methods");
  }
  char tmp[256];
  
  if( mxGetString( _prhs[0], tmp, 255) != 0)
  {
    mexErrMsgTxt( "method name should not exceed 255 characters, "
                  "maybe your first argument was not the name?");
  }
  _methodName = tmp;
  
  /*-----------------------------------------------------------------
   *   extract unique object ID (second right hand side argument)
   *-----------------------------------------------------------------*/
  if( mxIsDouble( _prhs[1]))
  {
    if( asDouble( 1) != 0)
    {
      mexErrMsgTxt( "second argument must be unique Object ID or 0");
    }
    _uniqueObjectID = 0;
  }
  else
  {
    _uniqueObjectID = asUint( 1);
  }
}




void InputArgs::assertNArgs( int nRequiredArgs) const
{
  if( _nrhs != nRequiredArgs)
  {
    char errmsg[256];
    sprintf( errmsg, "Method '%s' expects unique object ID and "
             "%d further argument(s), but you passed %d instead",
                     _methodName.c_str(), nRequiredArgs-2, _nrhs-2);
    mexErrMsgTxt( errmsg);
  }
}

double InputArgs::asDouble( int argIndex) const
{
  mxAssert( argIndex < _nrhs, "input argument index out of range");
  if( !mxIsDouble( _prhs[argIndex])
      || mxIsComplex( _prhs[argIndex])
      || mxGetN( _prhs[argIndex]) * mxGetM( _prhs[argIndex]) != 1)
  {
    char errmsg[256];
    sprintf( errmsg, 
             "argument %d of method %s must be a double value",
             argIndex, _methodName.c_str());
    mexErrMsgTxt( errmsg);
  }
  return mxGetScalar( _prhs[argIndex]);
}

unsigned int InputArgs::asUint( int argIndex) const
{
  mxAssert( argIndex < _nrhs, "input argument index out of range");
  if( !mxIsUint32( _prhs[argIndex])
      || mxGetN( _prhs[argIndex]) * mxGetM( _prhs[argIndex]) != 1)
  {
    char errmsg[256];
    sprintf( errmsg, 
             "argument %d of method %s must be an unit32 value",
             argIndex, _methodName.c_str());
    mexErrMsgTxt( errmsg);
  }
  
  unsigned int* value =
      reinterpret_cast<unsigned int*>(mxGetData(_prhs[argIndex]));
  mxAssert( value != NULL, "error accessing uint32 arguments data");
  
  return *value;
}
  
 
std::string InputArgs::asString( int argIndex) const
{
  mxAssert( argIndex < _nrhs, "input argument index out of range");
  if( !mxIsChar( _prhs[argIndex])
      || mxIsComplex( _prhs[argIndex])
      || mxGetM( _prhs[argIndex]) != 1)
  {
    char errmsg[256];
    sprintf( errmsg, 
             "argument %d of method %s must be a string value",
             argIndex, _methodName.c_str());
    mexErrMsgTxt( errmsg);
  }
  
  int size = mxGetN( _prhs[argIndex]) + 1;
  char data[size];
  
  if( mxGetString( _prhs[argIndex], data, size) != 0)
  {
    char errmsg[256];
    sprintf( errmsg, 
             "argument %d of method %s must be a string value",
             argIndex, _methodName.c_str());
    mexErrMsgTxt( errmsg);
  }

  return std::string( data);
  
}
  
 
