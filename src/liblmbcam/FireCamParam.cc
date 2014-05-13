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

#include "FireCamParam.hh"

#include "MutexLocker.hh"

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCamParamIntRange::FireCamParamIntRange(
 *             FireCam* camera, const std::string& name,
 *             dc1394_feature_info feature, pthread_mutex_t* libRawMutex)
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::FireCamParamIntRange::FireCamParamIntRange(
    FireCam* camera, const std::string& name, dc1394_feature_info feature,
    pthread_mutex_t* libRawMutex)
        :LMBCamParamIntRange( name),
         _camera( camera),
         _id( feature.feature_id),
         _min( feature.min),
         _max( feature.max),
         _libRawMutex( libRawMutex)
{}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCamParamIntRange::asInt() const
 *  ==> see headerfile
 *=======================================================================*/
int
liblmbcam::FireCamParamIntRange::asInt() const
{
  MutexLocker locker( _libRawMutex);

  unsigned int ret;
  
  if( dc1394_get_feature_value( _camera->handle(), _camera->node(),
                                _id, &ret) != DC1394_SUCCESS)
  {
    std::stringstream errString;
    errString << "Query for value of feature " << name() << " failed";
    FireCamError error( errString.str());
    LMBErrorHandler::ErrorHandler()->handle( error);
    return -1;
  }
  
  return int( ret);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCamParamIntRange::setValue( int val)
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::FireCamParamIntRange::setValue( int val)
{
  MutexLocker locker( _libRawMutex);
  
  if( val < minValueAsInt() || maxValueAsInt() < val)
  {
    FireCamError error( p_name + ": Value out of range");
    LMBErrorHandler::ErrorHandler()->handle( error);
    return;
  }
  else
  {
    if( dc1394_set_feature_value( _camera->handle(), _camera->node(),
                                  _id, (unsigned int) val) != DC1394_SUCCESS)
    {
      std::stringstream errString;
      errString << "Setting of feature " << name() << " to "
                << val << " failed";
      FireCamError error( errString.str());
      LMBErrorHandler::ErrorHandler()->handle( error);
      return;
    }
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCamParamAbsRange::FireCamParamAbsRange(
 *             FireCam* camera, const std::string& name,
 *             dc1394_feature_info feature, pthread_mutex_t* libRawMutex)
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::FireCamParamAbsRange::FireCamParamAbsRange(
    FireCam* camera, const std::string& name, dc1394_feature_info feature,
    pthread_mutex_t* libRawMutex)
        :LMBCamParamFloatRange( name),
         _camera( camera),
         _id( feature.feature_id),
         _val( feature.abs_value),
         _min( feature.abs_min),
         _max( feature.abs_max),
         _libRawMutex( libRawMutex)
{
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCamParamAbsRange::asFloat() const
 *  ==> see headerfile
 *=======================================================================*/
float
liblmbcam::FireCamParamAbsRange::asFloat() const
{
  MutexLocker locker( _libRawMutex);
  
  float ret=0;
  
  if( dc1394_query_absolute_feature_value( _camera->handle(), _camera->node(),
                                           _id, &ret) != DC1394_SUCCESS)
  {
    std::stringstream errString;
    errString << "Query for value of absolute feature " << name() << " failed";
    FireCamError error( errString.str());
    LMBErrorHandler::ErrorHandler()->handle( error);
    return -1;
  }

  return ret;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCamParamAbsRange::setValue( float val)
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::FireCamParamAbsRange::setValue( float val)
{
  MutexLocker locker( _libRawMutex);
  
  if( val < minValueAsFloat() || maxValueAsFloat() < val)
  {
    FireCamError error( name() + "_abs: Value out of range ");
    LMBErrorHandler::ErrorHandler()->handle( error);
    return;
  }
  else
  {
    if( dc1394_set_absolute_feature_value(
            _camera->handle(), _camera->node(), _id, val) != DC1394_SUCCESS)
    {
      std::stringstream errString;
      errString << "Setting of absolute feature " << name() << " to "
                << val << " failed";
      std::cerr << errString.str() << std::endl;
      FireCamError error( errString.str());
      LMBErrorHandler::ErrorHandler()->handle( error);
      return;
    }
    _val = val;
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCamParamWhiteBalanceU::
 *                          FireCamParamWhiteBalanceU( FireCam* camera,
 *                                                     const std::string& name,
 *                                               dc1394_feature_info feature,
 *                                               pthread_mutex_t* libRawMutex)
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::FireCamParamWhiteBalanceU::FireCamParamWhiteBalanceU(
    FireCam* camera,
    const std::string &name,
    dc1394_feature_info feature,
    pthread_mutex_t* libRawMutex)
        : FireCamParamIntRange( camera, name, feature, libRawMutex)
{}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCamParamWhiteBalanceV::
 *                          FireCamParamWhiteBalanceV( FireCam* camera,
 *                                                     const std::string& name,
 *                                               dc1394_feature_info feature,
 *                                               pthread_mutex_t* libRawMutex)

 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::FireCamParamWhiteBalanceV::FireCamParamWhiteBalanceV(
    FireCam* camera,
    const std::string& name,
    dc1394_feature_info feature,
    pthread_mutex_t* libRawMutex)
        : FireCamParamIntRange( camera, name, feature, libRawMutex)
{}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCamParamWhiteBalanceU::setValue( int val)
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::FireCamParamWhiteBalanceU::setValue( int val)
{
  MutexLocker locker( _libRawMutex);
  
  if( val < minValueAsInt() || maxValueAsInt() < val)
  {
    FireCamError error(  name() + ": Value out of range");
    LMBErrorHandler::ErrorHandler()->handle( error);
    return;
  }
  else
  {
    dc1394_feature_info feature = { 0 };
    feature.feature_id = _id;
  
    if( dc1394_get_camera_feature( cameraHandle(), cameraNode(),
                                   &feature) != DC1394_SUCCESS)
    {
      std::stringstream errString;
      errString << "Query for feature " << name() << " failed";
      FireCamError error( errString.str());
      LMBErrorHandler::ErrorHandler()->handle( error);
      return;
    }
    
    if( dc1394_set_white_balance( cameraHandle(), cameraNode(),
                                  (unsigned int) val, feature.RV_value)
        != DC1394_SUCCESS)
    {
      std::stringstream errString;
      errString << "Setting of white balance uvalue failed";
      FireCamError error( errString.str());
      LMBErrorHandler::ErrorHandler()->handle( error);
      return;
    }
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCamParamWhiteBalanceV::setValue( int val)
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::FireCamParamWhiteBalanceV::setValue( int val)
{
  MutexLocker locker( _libRawMutex);
  if( val < minValueAsInt() || maxValueAsInt() < val)
  {
     FireCamError error(  name() + ": Value out of range");
     LMBErrorHandler::ErrorHandler()->handle( error);
     return;
  }
  else
  {
    dc1394_feature_info feature = { 0 };
    feature.feature_id = _id;
  
    if( dc1394_get_camera_feature( cameraHandle(), cameraNode(),
                                   &feature) != DC1394_SUCCESS)
    {
      std::stringstream errString;
      errString << "Query for feature " << name() << " failed";
      FireCamError error( errString.str());
      LMBErrorHandler::ErrorHandler()->handle( error);
     return;
    }
    if( dc1394_set_white_balance( cameraHandle(), cameraNode(),
                                  feature.BU_value, (unsigned int) val)
        != DC1394_SUCCESS)
    {
      std::stringstream errString;
      errString << "Setting of white balance vvalue failed";
      FireCamError error( errString.str());
      LMBErrorHandler::ErrorHandler()->handle( error);
     return;
    }
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCamParamWhiteBalanceU::asInt() const
 *  ==> see headerfile
 *=======================================================================*/
int
liblmbcam::FireCamParamWhiteBalanceU::asInt() const
{
  MutexLocker locker( _libRawMutex);
  
  dc1394_feature_info feature = { 0 };
  feature.feature_id = _id;
  
  if( dc1394_get_camera_feature( cameraHandle(), cameraNode(),
                                 &feature) != DC1394_SUCCESS)
  {
    std::stringstream errString;
    errString << "Query for feature " << name() << " failed";
    FireCamError error( errString.str());
    LMBErrorHandler::ErrorHandler()->handle( error);
    return -1;
  }
  
  return feature.BU_value;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCamParamWhiteBalanceV::asInt() const
 *  ==> see headerfile
 *=======================================================================*/
int
liblmbcam::FireCamParamWhiteBalanceV::asInt() const
{
  MutexLocker locker( _libRawMutex);
  
  dc1394_feature_info feature = { 0 };
  feature.feature_id = _id;
  
  if( dc1394_get_camera_feature( cameraHandle(), cameraNode(),
                                 &feature) != DC1394_SUCCESS)
  {
    std::stringstream errString;
    errString << "Query for feature " << name() << " failed";
    FireCamError error( errString.str());
    LMBErrorHandler::ErrorHandler()->handle( error);
    return -1;
  }
  
  return feature.RV_value;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCamParamAutoSwitch::FireCamParamAutoSwitch(
 *                    FireCam* camera, const std::string& name,
 *                    dc1394_feature_info feature,
 *                    pthread_mutex_t* libRawMutex)
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::FireCamParamAutoSwitch::FireCamParamAutoSwitch(
    FireCam* camera, const std::string& name,
    dc1394_feature_info feature,
    pthread_mutex_t* libRawMutex)
        :FireCamParamIntRange( camera, name, feature, libRawMutex)
{
  setGUIHint( "checkbox");
  _autoCapable   = feature.auto_capable   == DC1394_TRUE ?
      true : false;
  
  _manualCapable = feature.manual_capable == DC1394_TRUE ?
      true : false;

  _onOffCapable = feature.on_off_capable == DC1394_TRUE ?
      true : false;
}
    
/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCamParamAutoSwitch::setValue( int value)
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::FireCamParamAutoSwitch::setValue( int value)
{
  MutexLocker locker( _libRawMutex);
  
  if( value == 1)
  {
    if( _autoCapable)
    {
      if( dc1394_auto_on_off( cameraHandle(), cameraNode(),
                              _id, 1) != DC1394_SUCCESS)
      {
        std::stringstream errString;
        errString << "Setting of feature " << name() << " to auto failed";
        FireCamError error( errString.str());
        LMBErrorHandler::ErrorHandler()->handle( error);
        return;
      }
    }
    else
    {
        std::stringstream errString;
        errString << "Feature " << name() << " is not auto capable";
        FireCamError error( errString.str());
        LMBErrorHandler::ErrorHandler()->handle( error);
        return;
    }
  }
  else
  {
    if( _manualCapable)
    {
      if( dc1394_auto_on_off( cameraHandle(), cameraNode(),
                              _id, 0) != DC1394_SUCCESS)
      {
        std::stringstream errString;
        errString << "Setting of feature " << name() << " to manual failed";
        FireCamError error( errString.str());
        LMBErrorHandler::ErrorHandler()->handle( error);
        return;
      }
    }
    else
    {
      if (_onOffCapable)
      {
        if (dc1394_auto_on_off( cameraHandle(), cameraNode(),
                                   _id, 0) != DC1394_SUCCESS)
        {
          std::stringstream errString;
          errString << "Feature " << name() << " can not be switched off";
          FireCamError error( errString.str());
          LMBErrorHandler::ErrorHandler()->handle( error);
          return;
        }
      }
      else 
      {
        std::stringstream errString;
        errString << "Feature " << name() << " is not manual capable and "
        "could not be switched off";
        FireCamError error( errString.str());
        LMBErrorHandler::ErrorHandler()->handle( error);
        return;
      }
    }
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCamParamAutoSwitch::asInt() const
 *  ==> see headerfile
 *=======================================================================*/
int
liblmbcam::FireCamParamAutoSwitch::asInt() const
{
  MutexLocker locker( _libRawMutex);
  
  dc1394bool_t tmp;
  
  if( dc1394_is_feature_auto( cameraHandle(), cameraNode(),
                              _id, &tmp) != DC1394_SUCCESS)
  {
    std::stringstream errString;
    errString << "Query for auto mode of feature " << name() << " failed";
    FireCamError error( errString.str());
    LMBErrorHandler::ErrorHandler()->handle( error);
    return -1;
  }
  return tmp == DC1394_TRUE ? 1 : 0;
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCamParamAbsSwitch::FireCamParamAbsSwitch(
 *                    FireCam* camera, const std::string& name,
 *                    dc1394_feature_info feature,
 *                    pthread_mutex_t* libRawMutex)
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::FireCamParamAbsSwitch::FireCamParamAbsSwitch(
    FireCam* camera, const std::string& name,
    dc1394_feature_info feature,
    pthread_mutex_t* libRawMutex)
        :FireCamParamIntRange( camera, name, feature, libRawMutex)
{
  setGUIHint( "checkbox");
}
    
/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCamParamAbsSwitch::setValue( int value)
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::FireCamParamAbsSwitch::setValue( int value)
{
  MutexLocker locker( _libRawMutex);
  
  if( dc1394_absolute_setting_on_off( cameraHandle(), cameraNode(), _id, value)
      != DC1394_SUCCESS)
  {
    std::stringstream errString;
    errString << "Activation of absolute control of feature " << name()
                << " failed";
    FireCamError error( errString.str());
    LMBErrorHandler::ErrorHandler()->handle( error);
    return;
  }
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:FireCamParamAbsSwitch::asInt() const
 *  ==> see headerfile
 *=======================================================================*/
int
liblmbcam::FireCamParamAbsSwitch::asInt() const
{
  MutexLocker locker( _libRawMutex);
  
  dc1394bool_t tmp;
  
  if( dc1394_query_absolute_control( cameraHandle(), cameraNode(),
                                     _id, &tmp) != DC1394_SUCCESS)
  {
    std::stringstream errString;
    errString << "Query for absolute control of feature " << name()
              << " failed";
    FireCamError error( errString.str());
    LMBErrorHandler::ErrorHandler()->handle( error);
    return -1;
  }

  return tmp == DC1394_TRUE ? 1 : 0;
}
