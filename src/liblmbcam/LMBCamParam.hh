/**************************************************************************
**       Title: 
**    $RCSfile: LMBCamParam.hh,v $
**   $Revision: 1.10 $$Name:  $
**       $Date: 2004/11/05 03:47:48 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: LMBCamParam.hh,v $
**  Revision 1.10  2004/11/05 03:47:48  mechnich
**  added resetValue()
**
**  Revision 1.9  2004/10/19 05:54:46  mechnich
**  added DMA stuff without testing, will probably need future fixes; added absolute control features
**
**  Revision 1.8  2003/10/05 19:37:34  mechnich
**  set default value of LMBCamParamSwitch to zero
**
**  Revision 1.7  2003/06/12 16:13:19  mechnich
**  added comments
**
**  Revision 1.6  2003/05/22 08:29:02  mechnich
**  added GUIHint() and setGUIHint() functions
**
**  Revision 1.5  2003/05/06 23:05:39  mechnich
**  - increased thread safety of library
**  - restructured parameter classes, introduced new subclasses to LMBCamParam:
**    LMBCamParamIntRange and LMBCamParamSwitch
**    TODO: add LMBCamParamDoubleRange for absolute features
**
**  Revision 1.4  2003/05/05 02:34:03  mechnich
**  added interface for querying and setting auto mode
**
**  Revision 1.3  2003/01/20 10:22:24  mechnich
**  *** empty log message ***
**
**  Revision 1.2  2002/12/11 04:21:21  mechnich
**  made non-dma mode the default
**  adapted parameter handling for dma accordingly
**
**  Revision 1.1  2002/12/04 13:17:46  mechnich
**  initial revision
**
**
**
**************************************************************************/

#ifndef LMBCAMPARAM_HH
#define LMBCAMPARAM_HH

/*-------------------------------------------------------------------------
 *  STL includes
 *-------------------------------------------------------------------------*/
#include <string>

namespace liblmbcam
{
  
/*======================================================================*/
/*!
 *  \class LMBCamParam LMBCamParam.hh
 *  \brief The LMBCamParam class defines the interface of all
 *         camera parameter classes.
 *
 *  This class contains all functions that camera parameter
 *  objects could possibly have.
 */
/*======================================================================*/
class LMBCamParam
{
public:
/*======================================================================*/
/*! 
 *   Constructor
 *   
 *   \param name the parameter's name
 */
/*======================================================================*/
  LMBCamParam( std::string name)
          :p_name( name)
        {}
  
/*======================================================================*/
/*! 
 *   Destructor
 */
/*======================================================================*/
  virtual ~LMBCamParam()
        {}
  
/*======================================================================*/
/*! 
 *   This function returns the parameter's name as a std::string.
 *
 *   \return parameter's name as a std::string
 */
/*======================================================================*/
  std::string name() const
        {
          return p_name;
        }
    
/*======================================================================*/
/*! 
 *   This function should set the parameter's value to <em>val</em>.
 *
 *   \param val parameter's new value
 */
/*======================================================================*/
  virtual void setValue( int val)=0;
  
/*======================================================================*/
/*! 
 *   This function should set the parameter's value to <em>val</em>.
 *
 *   \param val parameter's new value
 */
/*======================================================================*/
  virtual void setValue( float val)=0;

  virtual void resetValue()
        {}
    
/*======================================================================*/
/*! 
 *   This function should return the parameter's current value as an integer.
 *
 *   \return parameter's current value as an integer
 */
/*======================================================================*/
  virtual int asInt() const=0;
  
/*======================================================================*/
/*! 
 *   This function should return the parameter's current value as a float.
 *
 *   \return parameter's current value as a float
 */
/*======================================================================*/
  virtual float asFloat() const=0;
  
/*======================================================================*/
/*! 
 *   This function should return the minimal value for the parameter
 *   as an integer.
 *
 *   \return parameter's maximal value as an integer
 */
/*======================================================================*/
  virtual int minValueAsInt() const=0;
  
/*======================================================================*/
/*! 
 *   This function should return the minimal value for the parameter
 *   as a float.
 *
 *   \return parameter's maximal value as a float
 */
/*======================================================================*/
  virtual float minValueAsFloat() const=0;
  
/*======================================================================*/
/*! 
 *   This function should return the maximal value for the parameter.
 *   as an integer.
 *
 *   \return parameter's maximal value as an integer
 */
/*======================================================================*/
  virtual int maxValueAsInt() const=0;
    
/*======================================================================*/
/*! 
 *   This function should return the maximal value for the parameter.
 *   as a float.
 *
 *   \return parameter's maximal value as a float
 */
/*======================================================================*/
  virtual float maxValueAsFloat() const=0;

/*======================================================================*/
/*! 
 *   This function returns a string which contains a hint on the type of
 *   an optimal representation in a GUI for this parameter.
 *
 *   \return a string containing the GUI hint ( e.g. "slider")
 */
/*======================================================================*/
  virtual std::string GUIHint() const
        {
          return p_guiHint;
        }
    
protected:
/*======================================================================*/
/*! 
 *   This function sets the string which should be returned by the function
 *   <em>std::string GUIHint() const</em>
 *
 *   \param guiHint a string which containing the GUI hint ( e.g. "slider")
 */
/*======================================================================*/
  virtual void setGUIHint( std::string guiHint)
        {
          p_guiHint = guiHint;
        }
  
  std::string p_name;
  std::string p_guiHint;
};

/*======================================================================*/
/*!
 *  \class LMBCamParamFloatRange LMBCamParam.hh
 *  \brief The LMBCamParamFloatRange class represents a float camera parameter
 *         with a minimum, a maximum and a value
 */
/*======================================================================*/
class LMBCamParamFloatRange : public LMBCamParam
{
public:
/*======================================================================*/
/*! 
 *   Constructor
 *   
 *   \param name the parameter's name
 */
/*======================================================================*/
  LMBCamParamFloatRange( std::string name)
          :LMBCamParam( name)
        {
          setGUIHint( "lineedit");
        }

/*======================================================================*/
/*! 
 *   This function should set the parameter's value to <em>val</em>.
 *
 *   \param val parameter's new value
 */
/*======================================================================*/
  void setValue( int val)
      {
        setValue( float(val));
      }
  
/*======================================================================*/
/*! 
 *   This function rounds its parameter <em>val</em> to the closest integer
 *   and calls <em>setValue( int val)</em>
 *
 *   \param val parameter's new value
 */
/*======================================================================*/
  virtual void setValue( float val)=0;
  
/*======================================================================*/
/*! 
 *   This function should return the parameter's current value.
 *
 *   \return parameter's current value
 */
/*======================================================================*/
  int asInt() const
        {
          return int(asFloat());
        }
  
/*======================================================================*/
/*! 
 *   This function returns the return value of <em>asInt()</em>
 *
 *   \return parameter's current value
 */
/*======================================================================*/
  virtual float asFloat() const=0;
  
/*======================================================================*/
/*! 
 *   This function should return the minimal value for the parameter.
 *
 *   \return parameter's minimal value
 */
/*======================================================================*/
  virtual int minValueAsInt() const
        {
          return int( minValueAsFloat());
        }
  
/*======================================================================*/
/*! 
 *   This function returns the return value of <em>minValueAsInt()</em>.
 *
 *   \return parameter's minimal value
 */
/*======================================================================*/
  virtual float minValueAsFloat() const=0;
    
/*======================================================================*/
/*! 
 *   This function should return the maximal value for the parameter.
 *
 *   \return parameter's maximal value
 */
/*======================================================================*/
  virtual int maxValueAsInt() const
        {
          return maxValueAsInt();
        }
      
/*======================================================================*/
/*! 
 *   This function returns the return value of <em>minValueAsInt()</em>.
 *
 *   \return parameter's maximal value
 */
/*======================================================================*/
  virtual float maxValueAsFloat() const=0;
  
};

/*======================================================================*/
/*!
 *  \class LMBCamParamIntRange LMBCamParam.hh
 *  \brief The LMBCamParamIntRange class represents an integer camera parameter
 *         with a minimum, a maximum and a value
 */
/*======================================================================*/
class LMBCamParamIntRange : public LMBCamParam
{
public:
/*======================================================================*/
/*! 
 *   Constructor
 *   
 *   \param name the parameter's name
 */
/*======================================================================*/
  LMBCamParamIntRange( std::string name)
          :LMBCamParam( name)
        {
          setGUIHint( "slider");
        }

/*======================================================================*/
/*! 
 *   This function should set the parameter's value to <em>val</em>.
 *
 *   \param val parameter's new value
 */
/*======================================================================*/
  virtual void setValue( int val)=0;
  
/*======================================================================*/
/*! 
 *   This function rounds its parameter <em>val</em> to the closest integer
 *   and calls <em>setValue( int val)</em>
 *
 *   \param val parameter's new value
 */
/*======================================================================*/
  void setValue( float val)
        {
          if( minValueAsFloat() <= val && val <= maxValueAsFloat())
          {
            int tmp =
                ( (int)(val) == (int)(val + 0.5) ?
                  (int)(val) : (int)(val + 0.5));
              setValue( tmp);
            }
        }
  
/*======================================================================*/
/*! 
 *   This function should return the parameter's current value.
 *
 *   \return parameter's current value
 */
/*======================================================================*/
  virtual int asInt() const=0;
  
/*======================================================================*/
/*! 
 *   This function returns the return value of <em>asInt()</em>
 *
 *   \return parameter's current value
 */
/*======================================================================*/
  float asFloat() const
        {
          return asInt();
        }
    
/*======================================================================*/
/*! 
 *   This function should return the minimal value for the parameter.
 *
 *   \return parameter's minimal value
 */
/*======================================================================*/
  virtual int minValueAsInt() const=0;
  
/*======================================================================*/
/*! 
 *   This function returns the return value of <em>minValueAsInt()</em>.
 *
 *   \return parameter's minimal value
 */
/*======================================================================*/
  float minValueAsFloat() const
        {
          return minValueAsInt();
        }
    
/*======================================================================*/
/*! 
 *   This function should return the maximal value for the parameter.
 *
 *   \return parameter's maximal value
 */
/*======================================================================*/
  virtual int maxValueAsInt() const=0;
      
/*======================================================================*/
/*! 
 *   This function returns the return value of <em>minValueAsInt()</em>.
 *
 *   \return parameter's maximal value
 */
/*======================================================================*/
  float maxValueAsFloat() const
        {
          return maxValueAsInt();
        }
  
};

/*======================================================================*/
/*!
 *  \class LMBCamParamSwitch LMBCamParam.hh
 *  \brief The LMBCamParamSwitch class represents a parameter
 *         with "on/off"-capability.
 */
/*======================================================================*/
class LMBCamParamSwitch : public LMBCamParamIntRange
{
public:
/*======================================================================*/
/*! 
 *   Constructor
 *
 *   \param name the parameter's name
 */
/*======================================================================*/
  LMBCamParamSwitch( std::string name)
          :LMBCamParamIntRange( name), p_value( 0)
        {
          setGUIHint( "checkbox");
        }
    
/*======================================================================*/
/*! 
 *   This function activates the switch for <em>val</em> != 0.
 *
 *   \param val parameter's new value
 */
/*======================================================================*/
  virtual void setValue( int val)
        {
          p_value = 0 < val ? 1 : 0;
        }
    
/*======================================================================*/
/*! 
 *   This function returns the switch's current state.
 *
 *   \return 1 if the switch is activated,
 *           0 otherwise
 */
/*======================================================================*/
  virtual int asInt() const
        {
          return p_value;
        }
    
/*======================================================================*/
/*! 
 *   This function returns the integer <em>0</em>.
 *
 *   \return 0
 */
/*======================================================================*/
  int minValueAsInt() const
        {
          return 0;
        }
  
/*======================================================================*/
/*! 
 *   This function returns the integer <em>1</em>.
 *
 *   \return 1
 */
/*======================================================================*/
  int maxValueAsInt() const
        {
          return 1;
        }
      
private:
  
  int p_value;
};
 
}

#endif
