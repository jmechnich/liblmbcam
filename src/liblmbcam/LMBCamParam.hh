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

#ifndef LMBCAMPARAM_HH
#define LMBCAMPARAM_HH

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
  LMBCamParam( std::string name)
          :p_name( name)
        {}
  virtual ~LMBCamParam()
        {}
  
  std::string name() const
        {
          return p_name;
        }
    
  virtual void setValue( int val)=0;
  virtual void setValue( float val)=0;
  virtual int asInt() const=0;
  virtual float asFloat() const=0;
  virtual int minValueAsInt() const=0;
  virtual float minValueAsFloat() const=0;
  virtual int maxValueAsInt() const=0;
  virtual float maxValueAsFloat() const=0;

  virtual void resetValue() {}
  virtual std::string GUIHint() const
        {
          return p_guiHint;
        }
    
protected:
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
  LMBCamParamFloatRange( std::string name)
          :LMBCamParam( name)
        {
          setGUIHint( "lineedit");
        }

  virtual void setValue( int val)
        {
          setValue( float(val));
        }
  virtual int asInt() const
        {
          return int(asFloat());
        }
  virtual int minValueAsInt() const
        {
          return int( minValueAsFloat());
        }
  virtual int maxValueAsInt() const
        {
          return int( maxValueAsFloat());
        }
      
  virtual void  setValue( float val)=0;
  virtual float asFloat() const=0;
  virtual float minValueAsFloat() const=0;
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
  LMBCamParamIntRange( std::string name)
          :LMBCamParam( name)
        {
          setGUIHint( "slider");
        }

  virtual void setValue( int val)=0;
  virtual int  asInt() const=0;
  virtual int  minValueAsInt() const=0;
  virtual int  maxValueAsInt() const=0;
  
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
  float asFloat() const
        {
          return asInt();
        }
  float minValueAsFloat() const
        {
          return minValueAsInt();
        }
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
  LMBCamParamSwitch( std::string name)
          :LMBCamParamIntRange( name), p_value( 0)
        {
          setGUIHint( "checkbox");
        }
  virtual void setValue( int val)
        {
          p_value = 0 < val ? 1 : 0;
        }
  virtual int asInt() const
        {
          return p_value;
        }
  int minValueAsInt() const
        {
          return 0;
        }
  int maxValueAsInt() const
        {
          return 1;
        }
      
private:
  int p_value;
};
 
}

#endif
