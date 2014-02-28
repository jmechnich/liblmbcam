/**************************************************************************
**       Title: 
**    $RCSfile: OutputArgs.hh,v $
**   $Revision: 1.1 $$Name:  $
**       $Date: 2003/05/13 09:33:56 $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: OutputArgs.hh,v $
**  Revision 1.1  2003/05/13 09:33:56  ronneber
**  initial revision
**
**
**
**************************************************************************/

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
