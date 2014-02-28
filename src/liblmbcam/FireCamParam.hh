/**************************************************************************
**       Title: 
**    $RCSfile: FireCamParam.hh,v $
**   $Revision: 1.14 $$Name:  $
**       $Date: 2005/07/08 16:36:36 $
**   Copyright: GPL $Author: tschmidt $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: FireCamParam.hh,v $
**  Revision 1.14  2005/07/08 16:36:36  tschmidt
**  - If auto mode switch off request occurs, and no manual mode is available
**    an additional check is introduced, whether the feature is on_off_capable
**    and depending on this capability the auto_mode switch is turned off...
**    The sliders keep enabled, but due to their range (0..0) they are not
**    usable. In a later correction they should be suppressed at all
**
**  Revision 1.13  2004/11/05 03:46:07  mechnich
**  added resetValue() to FireCamAbsRange class, which resets the last given value for this feature
**
**  Revision 1.12  2004/10/19 05:54:46  mechnich
**  added DMA stuff without testing, will probably need future fixes; added absolute control features
**
**  Revision 1.11  2003/07/03 15:25:17  mechnich
**  - added comments
**  - improved Format7 capabilities
**  - added boolean return values to set...() functions
**
**  Revision 1.10  2003/06/12 16:13:19  mechnich
**  added comments
**
**  Revision 1.9  2003/05/22 08:29:02  mechnich
**  added GUIHint() and setGUIHint() functions
**
**  Revision 1.8  2003/05/07 00:06:29  mechnich
**  intorduced a wrapper mutex for critical libraw1394 functions,
**  liblmbcam should now be completely thread safe
**
**  Revision 1.7  2003/05/06 23:05:39  mechnich
**  - increased thread safety of library
**  - restructured parameter classes, introduced new subclasses to LMBCamParam:
**    LMBCamParamIntRange and LMBCamParamSwitch
**    TODO: add LMBCamParamDoubleRange for absolute features
**
**  Revision 1.6  2003/05/05 02:33:36  mechnich
**  added functions for querying and setting auto mode
**
**  Revision 1.5  2003/04/23 13:28:52  mechnich
**  - fixed parameter queries for all tested cameras
**  - fixed bug in iso handler function
**  - moved p_cameraIsRunning with affiliated functions to LMBCam
**
**  Revision 1.4  2003/01/20 10:22:24  mechnich
**  *** empty log message ***
**
**  Revision 1.3  2002/12/11 04:21:21  mechnich
**  made non-dma mode the default
**  adapted parameter handling for dma accordingly
**
**  Revision 1.2  2002/12/10 02:42:06  mechnich
**  added dma related functions in LMBCam
**  fixed bugs with PYRO WEBCAM API-200
**
**  Revision 1.1  2002/12/04 13:17:46  mechnich
**  initial revision
**
**
**
**************************************************************************/

#ifndef FIRECAMPARAM_HH
#define FIRECAMPARAM_HH

/*-------------------------------------------------------------------------
 *  Own includes
 *-------------------------------------------------------------------------*/
#include "LMBCamParam.hh"
#include "LMBErrorHandler.hh"
#include "FireCam.hh"
#include "FireCamError.hh"

/*-------------------------------------------------------------------------
 *  FireWire includes
 *-------------------------------------------------------------------------*/
#include <libdc1394/dc1394_control.h>

/*-------------------------------------------------------------------------
 *  STL includes
 *-------------------------------------------------------------------------*/
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
/*======================================================================*/
/*! 
 *   Constructor
 *
 *   \param camera pointer to the camera which owns this parameter
 *   \param name parameter name
 *   \param feature corresponding dc1394_feature_info
 *   \param libRawMutex pointer to the global mutex for libraw access
 */
/*======================================================================*/
  FireCamParamIntRange( FireCam* camera, const std::string& name,
                        dc1394_feature_info feature,
                        pthread_mutex_t* libRawMutex);

/*======================================================================*/
/*! 
 *   This function sets the parameter's value to <em>val</em>
 *
 *   \param val parameter's new value
 */
/*======================================================================*/
  virtual void setValue( int val);

/*======================================================================*/
/*! 
 *   This function returns the parameter's current value as an integer
 * 
 *   \return parameter's current value as an integer
 */
/*======================================================================*/
  virtual int asInt() const;

/*======================================================================*/
/*! 
 *   This function returns the parameter's minimal value as an integer
 *
 *   \return parameter's minimal value as an integer
 */
/*======================================================================*/
  virtual int minValueAsInt() const;

/*======================================================================*/
/*! 
 *   This function returns the parameter's maximal value as an integer
 *
 *   \return parameter's maximal value as an integer
 */
/*======================================================================*/
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
/*======================================================================*/
/*! 
 *   Constructor
 *
 *   \param camera pointer to the camera which owns this parameter
 *   \param name parameter name
 *   \param feature corresponding dc1394_feature_info
 *   \param libRawMutex pointer to the global mutex for libraw access
 */
/*======================================================================*/
  FireCamParamWhiteBalanceU( FireCam* camera, const std::string& name,
                             dc1394_feature_info feature,
                             pthread_mutex_t* libRawMutex);

/*======================================================================*/
/*! 
 *   This function returns the parameter's current value as an integer
 * 
 *   \return parameter's current value as an integer
 */
/*======================================================================*/
  int asInt() const;

/*======================================================================*/
/*! 
 *   This function sets the parameter's value to <em>val</em>
 *
 *   \param val parameter's new value
 */
/*======================================================================*/
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
/*======================================================================*/
/*! 
 *   Constructor
 *
 *   \param camera pointer to the camera which owns this parameter
 *   \param name parameter name
 *   \param feature corresponding dc1394_feature_info
 *   \param libRawMutex pointer to the global mutex for libraw access
 */
/*======================================================================*/
  FireCamParamWhiteBalanceV( FireCam* camera, const std::string& name,
                             dc1394_feature_info feature,
                             pthread_mutex_t* libRawMutex);

/*======================================================================*/
/*! 
 *   This function returns the parameter's current value as an integer
 * 
 *   \return parameter's current value as an integer
 */
/*======================================================================*/
  int asInt() const;

/*======================================================================*/
/*! 
 *   This function sets the parameter's value to <em>val</em>
 *
 *   \param val parameter's new value
 */
/*======================================================================*/
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
/*======================================================================*/
/*! 
 *   Constructor
 *
 *   \param camera pointer to the camera which owns this parameter
 *   \param name parameter name
 *   \param feature corresponding dc1394_feature_info
 *   \param libRawMutex pointer to the global mutex for libraw access
 */
/*======================================================================*/
  FireCamParamAutoSwitch( FireCam* camera, const std::string& name,
                          dc1394_feature_info feature,
                          pthread_mutex_t* libRawMutex);
    
/*======================================================================*/
/*! 
 *   This function sets the parameter's value to <em>val</em>
 *
 *   \param val parameter's new value
 */
/*======================================================================*/
  void setValue( int val);

/*======================================================================*/
/*! 
 *   This function returns the parameter's current value as an integer
 * 
 *   \return parameter's current value as an integer
 */
/*======================================================================*/
  int asInt() const;

/*======================================================================*/
/*! 
 *   This function returns the parameter's minimal value as an integer
 *
 *   \return parameter's minimal value as an integer
 */
/*======================================================================*/
  int minValueAsInt() const;

/*======================================================================*/
/*! 
 *   This function returns the parameter's maximal value as an integer
 *
 *   \return parameter's maximal value as an integer
 */
/*======================================================================*/
  int maxValueAsInt() const;

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
/*======================================================================*/
/*! 
 *   Constructor
 *
 *   \param camera pointer to the camera which owns this parameter
 *   \param name parameter name
 *   \param feature corresponding dc1394_feature_info
 *   \param libRawMutex pointer to the global mutex for libraw access
 */
/*======================================================================*/
  FireCamParamAbsRange( FireCam* camera, const std::string& name,
                        dc1394_feature_info feature,
                        pthread_mutex_t* libRawMutex);

/*======================================================================*/
/*! 
 *   This function sets the parameter's value to <em>val</em>
 *
 *   \param val parameter's new value
 */
/*======================================================================*/
  void setValue( float val);

  void resetValue();
  
/*======================================================================*/
/*! 
 *   This function returns the parameter's current value as an integer
 * 
 *   \return parameter's current value as an integer
 */
/*======================================================================*/
  float asFloat() const;

/*======================================================================*/
/*! 
 *   This function returns the parameter's minimal value as an integer
 *
 *   \return parameter's minimal value as an integer
 */
/*======================================================================*/
  float minValueAsFloat() const;

/*======================================================================*/
/*! 
 *   This function returns the parameter's maximal value as an integer
 *
 *   \return parameter's maximal value as an integer
 */
/*======================================================================*/
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
/*======================================================================*/
/*! 
 *   Constructor
 *
 *   \param camera pointer to the camera which owns this parameter
 *   \param name parameter name
 *   \param feature corresponding dc1394_feature_info
 *   \param libRawMutex pointer to the global mutex for libraw access
 */
/*======================================================================*/
  FireCamParamAbsSwitch( FireCam* camera, const std::string& name,
                         dc1394_feature_info feature,
                         pthread_mutex_t* libRawMutex);
    
/*======================================================================*/
/*! 
 *   This function sets the parameter's value to <em>val</em>
 *
 *   \param val parameter's new value
 */
/*======================================================================*/
  void setValue( int val);

/*======================================================================*/
/*! 
 *   This function returns the parameter's current value as an integer
 * 
 *   \return parameter's current value as an integer
 */
/*======================================================================*/
  int asInt() const;

/*======================================================================*/
/*! 
 *   This function returns the parameter's minimal value as an integer
 *
 *   \return parameter's minimal value as an integer
 */
/*======================================================================*/
  int minValueAsInt() const;

/*======================================================================*/
/*! 
 *   This function returns the parameter's maximal value as an integer
 *
 *   \return parameter's maximal value as an integer
 */
/*======================================================================*/
  int maxValueAsInt() const;
};
 
}

#include "FireCamParam.icc"

#endif
