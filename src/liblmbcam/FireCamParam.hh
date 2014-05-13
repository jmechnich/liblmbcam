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

#ifndef FIRECAMPARAM_HH
#define FIRECAMPARAM_HH

#include "LMBCamParam.hh"
#include "LMBErrorHandler.hh"
#include "FireCam.hh"
#include "FireCamError.hh"

#include <libdc1394/dc1394_control.h>

#include <map>
#include <string>
#include <sstream>

#include <pthread.h>

namespace liblmbcam
{
/*======================================================================*/
/*!
 *  \class FireCamParamIntRange FireCamParam.hh
 *  \brief The FireCamParamIntRange class represents an integer
 *         camera parameter with a minimum, a maximum and a value
 */
/*======================================================================*/
class FireCamParamIntRange : public LMBCamParamIntRange
{
public:
  FireCamParamIntRange( FireCam* camera, const std::string& name,
                        dc1394_feature_info feature,
                        pthread_mutex_t* libRawMutex);

  virtual void setValue( int val);
  virtual int asInt() const;
  virtual int minValueAsInt() const;
  virtual int maxValueAsInt() const;

protected:
  raw1394handle_t cameraHandle() const;
  nodeid_t cameraNode() const;
  
  FireCam* _camera;
  unsigned int _id;
  int _min, _max;
  pthread_mutex_t* _libRawMutex;
};

/*======================================================================*/
/*!
 *  \class FireCamParamWhiteBalanceU FireCamParam.hh
 *  \brief The FireCamParamWhiteBalanceU class is a specialization of the
 *         FireCamParamIntRange class
 */
/*======================================================================*/
class FireCamParamWhiteBalanceU : public FireCamParamIntRange
{
public:
  FireCamParamWhiteBalanceU( FireCam* camera, const std::string& name,
                             dc1394_feature_info feature,
                             pthread_mutex_t* libRawMutex);

  int  asInt() const;
  void setValue( int val);
};
  
/*======================================================================*/
/*!
 *  \class FireCamParamWhiteBalanceV FireCamParam.hh
 *  \brief The FireCamParamWhiteBalanceV class is a specialization of the
 *         FireCamParamIntRange class
 */
/*======================================================================*/
class FireCamParamWhiteBalanceV : public FireCamParamIntRange
{
public:
  FireCamParamWhiteBalanceV( FireCam* camera, const std::string& name,
                             dc1394_feature_info feature,
                             pthread_mutex_t* libRawMutex);
  int  asInt() const;
  void setValue( int val);
};

/*======================================================================*/
/*!
 *  \class FireCamParamAutoSwitch FireCamParam.hh
 *  \brief The FireCamParamAutoSwitch class is a specialization of the
 *         FireCamParamIntRange class
 */
/*======================================================================*/
class FireCamParamAutoSwitch : public FireCamParamIntRange
{
public:
  FireCamParamAutoSwitch( FireCam* camera, const std::string& name,
                          dc1394_feature_info feature,
                          pthread_mutex_t* libRawMutex);
    
  void setValue( int val);
  int  asInt() const;
  int  minValueAsInt() const;
  int  maxValueAsInt() const;

private:
  bool _autoCapable, _manualCapable, _onOffCapable;
  
};
 
/*======================================================================*/
/*!
 *  \class FireCamParamIntRange FireCamParam.hh
 *  \brief The FireCamParamIntRange class represents an integer
 *         camera parameter with a minimum, a maximum and a value
 */
/*======================================================================*/
class FireCamParamAbsRange : public LMBCamParamFloatRange
{
public:
  FireCamParamAbsRange( FireCam* camera, const std::string& name,
                        dc1394_feature_info feature,
                        pthread_mutex_t* libRawMutex);
  void  setValue( float val);
  void  resetValue();
  float asFloat() const;
  float minValueAsFloat() const;
  float maxValueAsFloat() const;

protected:
  raw1394handle_t cameraHandle() const;
  nodeid_t cameraNode() const;
  
  FireCam* _camera;
  unsigned int _id;
  float _val, _min, _max;
  pthread_mutex_t* _libRawMutex;
};

/*======================================================================*/
/*!
 *  \class FireCamParamAbsSwitch FireCamParam.hh
 *  \brief The FireCamParamAbsSwitch class is a specialization of the
 *         FireCamParamIntRange class
 */
/*======================================================================*/
class FireCamParamAbsSwitch : public FireCamParamIntRange
{
public:
  FireCamParamAbsSwitch( FireCam* camera, const std::string& name,
                         dc1394_feature_info feature,
                         pthread_mutex_t* libRawMutex);
  void setValue( int val);
  int asInt() const;
  int minValueAsInt() const;
  int maxValueAsInt() const;
};
 
}

#include "FireCamParam.icc"

#endif
