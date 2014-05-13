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

#ifndef V4LCAMPARAM_HH
#define V4LCAMPARAM_HH

/*-------------------------------------------------------------------------
 *  Own includes
 *-------------------------------------------------------------------------*/
#include "LMBCamParam.hh"
#include "LMBErrorHandler.hh"
#include "V4LCam.hh"
#include "V4LCamError.hh"

/*-------------------------------------------------------------------------
 *  STL includes
 *-------------------------------------------------------------------------*/
#include <map>
#include <string>
#include <sstream>

#include <pthread.h>
#include <linux/videodev2.h>

namespace liblmbcam
{
/*======================================================================*/
/*!
 *  \class V4LCamParamIntRange V4LCamParam.hh
 *  \brief The V4LCamParamIntRange class represents an integer
 *         camera parameter with a minimum, a maximum and a value
 */
/*======================================================================*/
class V4LCamParamIntRange : public LMBCamParamIntRange
{
public:
  V4LCamParamIntRange( V4LCam* camera, const std::string& name,
                       const v4l2_queryctrl& qc);

  void setValue( int val);
  int  asInt() const;
  int  minValueAsInt() const
        {
          return _qc.minimum;
        }
  int  maxValueAsInt() const
        {
          return _qc.maximum;
        }
  void resetValue()
        {
          setValue( _qc.default_value);
        }
  
protected:
  V4LCam* _camera;
  v4l2_queryctrl _qc;
};

/*======================================================================*/
/*!
 *  \class V4LCamParamAutoSwitch V4LCamParam.hh
 *  \brief The V4LCamParamAutoSwitch class is a specialization of the
 *         V4LCamParamIntRange class
 */
/*======================================================================*/
class V4LCamParamSwitch : public V4LCamParamIntRange
{
public:
  V4LCamParamSwitch( V4LCam* camera, const std::string& name,
                     const v4l2_queryctrl& qc)
          : V4LCamParamIntRange( camera, name, qc)
        {
          setGUIHint( "checkbox");
        }
};
 
}

#endif
