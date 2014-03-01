/**************************************************************************
**       Title: 
**    $RCSfile: LMBCam.hh,v $
**   $Revision: 1.24 $$Name:  $
**       $Date: 2006/07/13 12:51:57 $
**   Copyright: GPL $Author: jhense $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: LMBCam.hh,v $
**  Revision 1.24  2006/07/13 12:51:57  jhense
**  Added new interface getFrameStartMutex(). Other threads can be notified, when
**  frame transmission started.
**
**  Revision 1.23  2006/03/09 14:29:19  jhense
**  Removed unnecessary header array.h from file.
**
**  Revision 1.22  2006/02/03 09:30:47  jhense
**  New function "getRelativeFrameStarttimes" added, where camera measures the
**  end of exposure = frame start. This cannot be retrieved when using dma.
**
**  Revision 1.21  2006/01/11 16:07:43  jhense
**  Added feature to allocate a ringbuffer in application and set it as camera-ringbuffer: setExternalRingbuffer
**  Start and end of transmission can be read with getFrameStarttime and getFrameFilltime
**
**  Revision 1.20  2005/12/14 16:11:25  jhense
**  Additional functions: applySettingsToSequenceRegister and selectSequenceRegister for better support of Sequence-Feature.
**
**  Revision 1.19  2005/10/28 12:34:39  jhense
**   Added functions initSequenceFeature and applyParametersToSequence to support the AVT-Sequence-Feature.
**
**  Revision 1.18  2004/05/28 12:09:01  mechnich
**  added extended parameters
**
**  Revision 1.17  2004/05/09 16:26:11  mechnich
**  added saveParametersToMap() ( after adding loadParametersFromMap())
**
**  Revision 1.16  2004/05/09 16:02:04  mechnich
**  added loadParameterMap() function
**
**  Revision 1.15  2003/12/18 15:34:33  mechnich
**  changed return value of bytePerPixel from float to double
**
**  Revision 1.14  2003/11/05 16:25:38  mechnich
**  sync
**
**  Revision 1.13  2003/10/05 19:36:28  mechnich
**  added functions readState() and writeState()
**
**  Revision 1.12  2003/08/05 14:41:36  mechnich
**  added image position get and set functions
**
**  Revision 1.11  2003/07/03 15:25:41  mechnich
**  added boolean return values to set...() functions
**
**  Revision 1.10  2003/06/12 16:13:19  mechnich
**  added comments
**
**  Revision 1.9  2003/05/22 08:29:46  mechnich
**  - minor bugfixes
**  - introduced parameterMap()
**
**  Revision 1.8  2003/05/21 09:00:47  mechnich
**  removed superfluent functions and added missing to LMBCam
**
**  Revision 1.7  2003/04/24 17:01:30  mechnich
**  added return value to setFramerate() and setMode()
**
**  Revision 1.6  2003/04/23 13:23:53  mechnich
**  moved p_cameraIsRunning from FireCam class to here
**
**  Revision 1.5  2003/04/16 16:41:26  mechnich
**  added threaded framegrabber for non-DMA mode
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

#ifndef LMBCAM_HH
#define LMBCAM_HH

/*-------------------------------------------------------------------------
 *  Own includes
 *-------------------------------------------------------------------------*/
#include "LMBCamError.hh"
#include "LMBErrorHandler.hh"

/*-------------------------------------------------------------------------
 *  STL includes
 *-------------------------------------------------------------------------*/
#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <limits.h>

namespace liblmbcam
{
  
  class LMBCamParam;
  
/*======================================================================*/
/*!
 *  \class LMBCam LMBCam.hh
 *  \brief The LMBCam class specifies the interface for all camera classes
 *
 *  (description)
 */
/*======================================================================*/
class LMBCam
{
public:
/*======================================================================*/
/*! 
 *   This function should return the camera's vendor as a string
 *
 *   \return the camera's vendor
 */
/*======================================================================*/#
  virtual std::string vendor() const=0;

/*======================================================================*/
/*! 
 *   This function should return the camera model as a string
 *
 *   \return the camera model
 */
/*======================================================================*/
  virtual std::string model() const=0;
  
/*======================================================================*/
/*! 
 *   This function should return a unique id of the camera
 *
 *   \return unique id of the camera
 */
/*======================================================================*/
  virtual std::string guid() const=0;
  
/*======================================================================*/
/*! 
 *   This function reads camera settings from an opened istream.
 *
 *   \param is istream to read from
 */
/*======================================================================*/
  virtual void readState( std::istream& is);

/*======================================================================*/
/*! 
 *   This function writes camera settings to an opened ostream.
 *
 *   \param os the ostream to write to
 */
/*======================================================================*/
  virtual void writeState( std::ostream& os);
  
/*======================================================================*/
/*! 
 *   This function parses a string-string map containing parameter names and
 *   values and sets all known camera options.
 *
 *   \param paramMap the string-string map to be parsed
 *   \param prefix   string which precedes the parameter names of liblmbcam
 *                   (e.g. "cam_")
 */
/*======================================================================*/
  virtual void loadParametersFromMap(
      const std::map<std::string,std::string>& paramMap,
      const std::string& prefix=std::string(), bool clearExtended=true);
  
/*======================================================================*/
/*! 
 *   This function fills a string-string map with all known camera options.
 *
 *   \param paramMap the string-string map to be written to
 *   \param prefix   string which precedes the parameter names of liblmbcam
 *                   (e.g. "cam_")
 */
/*======================================================================*/
  virtual void saveParametersToMap(
      std::map<std::string,std::string>& paramMap,
      const std::string& prefix=std::string());
  
/*======================================================================*/
/*! 
 *   This function should reset the camera to its factory defaults
 */
/*======================================================================*/
  virtual void reset()=0;
  
/*======================================================================*/
/*! 
 *   This function should query the camera for a feature specified by paramStr
 *
 *   \param paramStr parameter name as string
 *   \return pointer to the requested camera parameter object
 */
/*======================================================================*/
  virtual LMBCamParam* param( const std::string& paramStr)=0;

/*======================================================================*/
/*! 
 *   This function returns a vector with the names of all available
 *   camera features
 *
 *   \return vector containing available camera feature names
 */
/*======================================================================*/
  virtual std::vector<std::string> availableParameters() const;  
  
/*======================================================================*/
/*! 
 *   This function returns a map containing all extended camera features
 *
 *   \return map containing extended camera features
 */
/*======================================================================*/
  virtual std::map<std::string,LMBCamParam*> parameterMap() const;

/*======================================================================*/
/*! 
 *   This function returns a vector with the names of all extended
 *   camera features
 *
 *   \return vector containing extended camera feature names
 */
/*======================================================================*/
  virtual std::vector<std::string> extendedParameters() const;  
  
/*======================================================================*/
/*! 
 *   This function returns a map containing all available camera features
 *
 *   \return map containing available camera features
 */
/*======================================================================*/
  virtual std::map<std::string,std::string> extendedParameterMap() const;

/*======================================================================*/
/*! 
 *   This function should set the camera mode to <em>mode</em>
 *
 *   \param mode a string containing the new mode for the camera
 *   \return true, if <em>mode</em> is supported by the camera
 *           and was also set, false otherwise
 */
/*======================================================================*/
  virtual bool setMode( const std::string& mode)=0;

/*======================================================================*/
/*! 
 *   This function should return the currently set mode of the camera
 *
 *   \return the currently set mode
 */
/*======================================================================*/
  virtual std::string mode() const=0;

/*======================================================================*/
/*! 
 *   This function should return a vector with the names of all
 *   available camera modes
 *
 *   \return vector containing available camera modes
 */
/*======================================================================*/
  virtual std::vector<std::string> availableModes() const=0;

/*======================================================================*/
/*! 
 *   This function should set the current framerate to <em>framerate</em>.
 *   If <em>allowRounding</em> is true, the framerate should be rounded if
 *   it is not valid for the camera in the current mode.
 *   Otherwise, an error should be thrown if <em>framerate</em> is invalid.
 *
 *   \param framerate The framerate the camera should be set to
 *   \param allowRounding Flag which controls rounding if <em>framerate</em>
 *                        is invalid
 *   \return true, if <em>framerate</em> is supported by the camera and
 *           was also set, false otherwise
 */
/*======================================================================*/
  virtual bool setFramerate( double framerate, bool allowRounding=false)=0;

/*======================================================================*/
/*! 
 *   This function should return the currently set framerate of the camera
 *
 *   \return the currently set framerate
 */
/*======================================================================*/
  virtual double framerate() const=0;
  
/*======================================================================*/
/*! 
 *   This function should return all valid framerates (not only for the
 *   current mode!)
 *
 *   \return vector containing available framerates
 */
/*======================================================================*/
  virtual std::vector<double> availableFramerates() const=0;

/*======================================================================*/
/*! 
 *   This function should return all available framerates in camera mode 
 *   <em>mode</em>
 *
 *   \param mode Camera mode which should be queried for framerates 
 *   \return vector containing available framerates
 */
/*======================================================================*/
  virtual std::vector<double> availableFrameratesInMode(
      const std::string& mode) const=0;

/*======================================================================*/
/*! 
 *   This function should change the color coding of the camera to
 *   <em>colorCoding</em>. Mode and framerate should stay unchanged.
 *
 *   \param colorCoding The color coding the camera should be set to
 *   \return true, if color coding could be changed,
 *           false otherwise
 */
/*======================================================================*/
  virtual bool setColorCoding( const std::string& colorCoding)=0;

/*======================================================================*/
/*! 
 *   This function should return the current color coding of the camera
 *
 *   \return the current color coding
 */
/*======================================================================*/
  virtual std::string colorCoding() const=0;
  
/*======================================================================*/
/*! 
 *   This function should return the offset of the left border
 *
 *   \return offset of the left border
 */
/*======================================================================*/
  virtual unsigned int left() const=0;

/*======================================================================*/
/*! 
 *   This function should set the offset of the left border
 *
 *   \param left new offset of the left border
 *   \return true if left border was set, false otherwise
 */
/*======================================================================*/
  virtual bool setLeft( unsigned int left)=0;

/*======================================================================*/
/*! 
 *   This function should return the offset of the top border
 *
 *   \return offset of the top border, 0 otherwise
 */
/*======================================================================*/
  virtual unsigned int top() const=0;

/*======================================================================*/
/*! 
 *   This function should set the offset of the top border
 *
 *   \param top new offset of the top border
 *   \return true if top border was set, false otherwise
 */
/*======================================================================*/
  virtual bool setTop( unsigned int top)=0;

/*======================================================================*/
/*! 
 *   This function should return the image width
 *
 *   \return width in pixels
 */
/*======================================================================*/
  virtual unsigned int width() const=0;

/*======================================================================*/
/*! 
 *   This function should set the width of the camera image
 *
 *   \param width new width of the camera image
 *   \return true if width was set, false otherwise
 */
/*======================================================================*/
  virtual bool setWidth( unsigned int width)=0;

/*======================================================================*/
/*! 
 *   This function should return the maximum image width
 *
 *   \return maximum width in pixels
 */
/*======================================================================*/
  virtual unsigned int maxWidth() const=0;

/*======================================================================*/
/*! 
 *   This function should return the image height
 *
 *   \return height in pixels
 */
/*======================================================================*/
  virtual unsigned int height() const=0;

/*======================================================================*/
/*! 
 *   This function should set the height of the camera image
 *
 *   \param height new height of the camera image
 *   \return true if height was set, false otherwise
 */
/*======================================================================*/
  virtual bool setHeight( unsigned int height)=0;

/*======================================================================*/
/*! 
 *   This function should return the maximum image height
 *
 *   \return maximum height in pixels
 */
/*======================================================================*/
  virtual unsigned int maxHeight() const=0;

/*======================================================================*/
/*! 
 *   This function should set the size of the internal ringbuffer to
 *   <em>nFrames</em>
 *
 *   \param nFrames number of frames to be buffered
 */
/*======================================================================*/
  virtual void setRingBufferSize( unsigned int nFrames)=0;

/*======================================================================*/
/*! 
 *   This function should stop the camera and free the buffer.
 */
/*======================================================================*/
  virtual void stopCamera()=0;

/*======================================================================*/
/*! 
 *   This function should return the index of the last recently grabbed frame.
 *
 *   \return index of the last recently grabbed frame
 */
/*======================================================================*/
  virtual long mostRecentFrameIndex()=0;

/*======================================================================*/
/*! 
 *   This function returns the ring buffer index of the last recently 
 *   grabbed frame.
 *
 *   \return ring buffer index of the last recently grabbed frame
 */
/*======================================================================*/
  virtual int mostRecentRingBufferIndex() = 0;


/*======================================================================*/
/*! 
 *   This function should wait for the grabber to start receiving the
 *   frame with index <em>frameIndex</em>.
 *
 *   \param frameIndex index of the frame to wait for
 *   \return index of the frame to wait for
 */
/*======================================================================*/
  virtual long waitForFrameStarted( long frameIndex)=0;

/*======================================================================*/
/*! 
 *   This function should wait for the grabber to finish receiving the
 *   frame with index <em>frameIndex</em>.
 *
 *   \param frameIndex index of the frame to wait for
 *   \return index of the frame to wait for
 */
/*======================================================================*/
  virtual long waitForFrameFinished( long frameIndex)=0;
  
/*======================================================================*/
/*! 
 *   This function should try to lock the frame with index <em>frameIndex</em>.
 *   The virtual function <em>ringBufferOverflow()</em> is called if
 *   the grabber tries to overwrite the locked frame.
 *
 *   \param frameIndex index of the frame to lock
 *   \return true if frame was locked, false otherwise
 */
/*======================================================================*/
  virtual bool tryLockFrame( long frameIndex)=0;

/*======================================================================*/
/*! 
 *   This function should return a pointer to the raw image data of the
 *   frame with index <em>frameIndex</em>.
 *
 *   \param frameIndex index of the frame
 *   \return pointer to the raw image data
 */
/*======================================================================*/
  virtual unsigned char* getFrame( long frameIndex)=0;

/*======================================================================*/
/*! 
 *   This function should unlock the frame previously locked by
 *   <em>tryLockFrame( long frameIndex)</em>
 */
/*======================================================================*/
  virtual void unlockFrame()=0;

/*======================================================================*/
/*! 
 *   This function should check if the currently used color coding consists of
 *   components( e.g. RGB) or represents a compressed image( e.g. YUV).
 *
 *   \return true if image is compressed, false otherwise
 */
/*======================================================================*/
  virtual bool isCompressed() const=0;

/*======================================================================*/
/*! 
 *   This function should return the number of components in a pixel.
 *   If the color coding is YUV, the function returns zero.
 *
 *   \return number of components in a pixel, 0 if image is compressed.
 */
/*======================================================================*/
  virtual unsigned int nComponents() const=0;

/*======================================================================*/
/*! 
 *   This function should return the physical size of a pixel in memory.
 *
 *   \return size of a pixel in byte
 */
/*======================================================================*/
  virtual double bytePerPixel() const=0;

/*======================================================================*/
/*! 
 *   This function starts the camera and keeps it running for an
 *   infinite amount of time
 */
/*======================================================================*/
  void startCameraRunContinuous();

/*======================================================================*/
/*! 
 *   This function starts the camera and stops it after <em>nFrames</em>
 *   frames have been captured.
 *
 *   \param nFrames number of frames to be captured
 */
/*======================================================================*/
  void startCameraRunNFrames( long nFrames);
  
/*======================================================================*/
/*! 
 *   This function returns the index of the frame at which the camera will
 *   be stopped
 *
 *   \return frame index at which the camera will be stopped
 */
/*======================================================================*/
  long stopAtFrameIndex() const;
  
/*======================================================================*/
/*! 
 *   This functions returns if the camera is actually running.
 *
 *   \return true if camera is running, false otherwise
 */
/*======================================================================*/
  bool isRunning() const;
  
/*======================================================================*/
/*! 
 *   This function restarts the camera
 */
/*======================================================================*/
  virtual void restartCamera();
  
/*======================================================================*/
/*! 
 *   This virtual function should be called when the grabber starts
 *   receiving a frame. The default implementation does nothing.
 *
 *   \param frameIndex index of the started frame
 */
/*======================================================================*/
  virtual void frameReceiveStarted( long frameIndex);

/*======================================================================*/
/*! 
 *   This virtual function should be called when the grabber finishes
 *   receiving a frame. The default implementation does nothing.
 *
 *   \param frameIndex index of the finished frame
 */
/*======================================================================*/
  virtual void frameReceiveFinished( long frameIndex);

/*======================================================================*/
/*! 
 *   This virtual function should be called when the grabber writes to a frame
 *   which lies three frames before the last read frame.
 *   The default implementation does nothing.
 */
/*======================================================================*/
  virtual void ringBufferCritical();

/*======================================================================*/
/*! 
 *   This virtual function should be called when the grabber tries to write to
 *   a locked frame. The default implementation does nothing.
 */
/*======================================================================*/
  void ringBufferOverflow();

/*======================================================================*/
/*! 
 *   initSequenceFeature activates the AVT-feature "Sequence", where you
 *   can set different camera parameters for a sequence of shots. 
 *   This function is to be set when camera is off, the sequence is
 *   executed from the next start of the camera until the end of the 
 *   sequence (autorewind off) or the next camera stop.
 *  
 *   \param count Number of images per sequence (Maximum is 16)
 *   \param autorewind  Repeats sequence until camera is stopped
 *   
 *   \return true is succeeds
 */
/*======================================================================*/
 
  virtual bool initSequenceFeature(char count = 16, bool autorewind = false)
  {
    return false;
  };


/*======================================================================*/
/*! 
 *   applyParametersToSequence sets the current camera parameters 
 *   to the next stack position of
 *   the sequence. When parameter position is given, the parameters of 
 *   a certain stack position can be set or changed. Stack starts with 
 *   position 0.
 *
 *   \param position at the sequence-stack
 *
 *   \return true if succeeds
 */
/*======================================================================*/
  virtual bool applyParametersToSequence(char position = -1)
  {
    return false;
  };

/*======================================================================*/
/*! 
 *   stopSequenceFeature sets the SEQUENCE_CTRL-Register to 0.
 *   \return true if succeeds
 */
/*======================================================================*/

  virtual bool stopSequenceFeature()
  {
    return false;
  };

/*======================================================================*/
/*! 
 *   getFrameFilltime returns the time when the frame transfer was completed.
 *
 *   \param number Specifies the framenumber to query the filltime
 *
 *   \return completion time of frame transfer. 
 */
/*======================================================================*/ 

  virtual struct timeval getFrameFilltime (long number)
  {
    return timeval();
  };

  

/*======================================================================*/
/*! 
 *   getFrameStarttime returns the time when the frame transfer was started.
 *
 *   \param number Specifies the framenumber to query for starttime
 *
 *   \return starttime of frame transfer. 
 */
/*======================================================================*/ 

  virtual struct timeval getFrameStarttime (long number)
  {
    return timeval();
  };


/*======================================================================*/
/*! 
 *   setExternalRingBuffer allows to insert a ring buffer, that is controlled
 *   by the application. Unnecessary copying is avoided.
 * 
 *   \param ringBuffer Pointers to memory that holds the data of the 
 *                     buffered frames. 
 *
 *   \return nothing
 */
/*======================================================================*/ 
  void virtual setExternalRingBuffer(std::vector<unsigned char*>) = 0;

/*======================================================================*/
/*! 
 *   (description)
 *
 *   \param no Number of frames to measure during record
 *
 *   \exception 
 *
 *   \return vector of relative starttimes of the frames.
 */
/*======================================================================*/ 
  virtual std::vector<timeval> measureRelativeFrameStarttimes(int no) = 0;
  
 
/*======================================================================*/
/*! 
 *   This sets the capturing of the camera in the right mode: 
 *   dma/dma-format7/standard/standard-format7
 *   Geometry of image is sent to camera.
 *   Uses the dc1394-functions.
 *
 *   \exception FireCamError
 *
 *   \return
 */
/*======================================================================*/
  virtual void setupCapture(){};
  
   
/*======================================================================*/
/*! 
 *   Returns mutex and condition to signal the start
 *   of the frame has happened.
 *
 *   \param 
 *
 *   \exception 
 *
 *   \return
 */
/*======================================================================*/
  virtual  void getFrameStartMutex(pthread_mutex_t**, pthread_cond_t**);
  

protected:
/*======================================================================*/
/*! 
 *   Default Constructor
 */
/*======================================================================*/
  LMBCam()
          :p_cameraIsRunning( false), p_maxReadFrames( LONG_MAX)
        {}

/*======================================================================*/
/*! 
 *   Destructor
 */
/*======================================================================*/
  virtual ~LMBCam()
        {}

/*======================================================================*/
/*! 
 *   This function should start the camera
 */
/*======================================================================*/
  virtual void startCamera()=0;

  bool p_cameraIsRunning;
  long p_maxReadFrames;

  std::map<std::string,LMBCamParam*> p_availableParameters;
  std::map<std::string,std::string> p_extendedParameters;

};

#include "LMBCam.icc"

}

#endif
