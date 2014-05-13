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

#include "V4LCamParam.hh"

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:V4LCamParamIntRange::V4LCamParamIntRange(
 *             V4LCam* camera, const std::string& name,
 *             dc1394_feature_info feature, pthread_mutex_t* libRawMutex)
 *  ==> see headerfile
 *=======================================================================*/
liblmbcam::V4LCamParamIntRange::V4LCamParamIntRange(
    V4LCam* camera, const std::string& name, const v4l2_queryctrl& qc)
        :LMBCamParamIntRange( name),
         _camera( camera), _qc( qc)
{}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:V4LCamParamIntRange::asInt() const
 *  ==> see headerfile
 *=======================================================================*/
int
liblmbcam::V4LCamParamIntRange::asInt() const
{
  struct v4l2_control control={0};
  control.id = _qc.id;
  if( -1 == _camera->xioctl( VIDIOC_G_CTRL, &control))
  {
    std::stringstream errString;
    errString << "Query for value of feature " << name() << " failed";
    V4LCamError error( errString.str());
    LMBErrorHandler::ErrorHandler()->handle( error);
    return -1;
  }
  
  return int( control.value);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:V4LCamParamIntRange::setValue( int val)
 *  ==> see headerfile
 *=======================================================================*/
void
liblmbcam::V4LCamParamIntRange::setValue( int val)
{
  if( val < minValueAsInt() || maxValueAsInt() < val)
  {
    V4LCamError error( name() + ": Value out of range");
    LMBErrorHandler::ErrorHandler()->handle( error);
    return;
  }
  else
  {
    if( -1 == _camera->xioctl( VIDIOC_QUERYCTRL, &_qc))
    {
      std::stringstream errString;
      errString << "Setting of feature " << name() << " to "
                << val << " failed";
      V4LCamError error( errString.str());
      LMBErrorHandler::ErrorHandler()->handle( error);
      return;
    }
    
    if( _qc.flags & V4L2_CTRL_FLAG_GRABBED ||
        _qc.flags & V4L2_CTRL_FLAG_READ_ONLY ||
        _qc.flags & V4L2_CTRL_FLAG_INACTIVE)
    {
      return;
    }
    
    struct v4l2_control control={0};
    control.id = _qc.id;
    control.value = val;
    if( -1 == _camera->xioctl( VIDIOC_S_CTRL, &control))
    {
      std::stringstream errString;
      errString << "Setting of feature " << name() << " to "
                << val << " failed";
      V4LCamError error( errString.str());
      LMBErrorHandler::ErrorHandler()->handle( error);
      return;
    }
  }
}
