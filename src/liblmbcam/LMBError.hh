/**************************************************************************
**       Title: 
**    $RCSfile: LMBError.hh,v $
**   $Revision: 1.5 $$Name:  $
**       $Date: 2004/10/19 05:53:35 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: LMBError.hh,v $
**  Revision 1.5  2004/10/19 05:53:35  mechnich
**  changed LMBError interface
**
**  Revision 1.4  2004/01/26 21:34:24  mechnich
**  added stream operator for easier error handling
**
**  Revision 1.3  2003/12/02 16:35:46  mechnich
**  just for maintenance
**
**  Revision 1.2  2003/06/12 16:13:19  mechnich
**  added comments
**
**  Revision 1.1  2002/12/04 13:17:46  mechnich
**  initial revision
**
**
**
**************************************************************************/

#ifndef LMBERROR_HH
#define LMBERROR_HH

/*-------------------------------------------------------------------------
 *  STL includes
 *-------------------------------------------------------------------------*/
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

/*======================================================================*/
/*! 
 *   Constructor
 *
 *   \param str the error message string
 */
/*======================================================================*/
    LMBError( const std::string& s, bool critical=false)
            :_str( s), _critical( critical)
          {}

/*======================================================================*/
/*! 
 *   This function returns the error message.
 *
 *   \return error message
 */
/*======================================================================*/
    std::string str() const
          {
            return _str;
          }
  
/*======================================================================*/
/*! 
 *   This function returns the error message.
 *
 *   \return error message
 */
/*======================================================================*/
    void setStr( const std::string& s )
          {
            _str = s;
          }
  
/*======================================================================*/
/*! 
 *   This function returns false everytime it is called.
 *
 *   \return false
 */
/*======================================================================*/
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
    
/*-------------------------------------------------------------------------
 *  Stream operator
 *-------------------------------------------------------------------------*/
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
/*======================================================================*/
/*! 
 *   Constructor
 *
 *   \param msg the error message string
 */
/*======================================================================*/
    LMBCriticalError()
            :LMBError( true)
          {}

    LMBCriticalError( const std::string& s)
            :LMBError( s, true)
          {}

  };
  
}

#endif
