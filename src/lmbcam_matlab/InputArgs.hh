/**************************************************************************
**       Title: 
**    $RCSfile: InputArgs.hh,v $
**   $Revision: 1.1 $$Name:  $
**       $Date: 2003/05/13 09:33:56 $
**   Copyright: GPL $Author: ronneber $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: InputArgs.hh,v $
**  Revision 1.1  2003/05/13 09:33:56  ronneber
**  initial revision
**
**
**
**************************************************************************/

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
