/**************************************************************************
**       Title: 
**    $RCSfile: FireCam.hh,v $
**   $Revision: 1.39 $$Name:  $
**       $Date: 2006/07/13 12:54:17 $
**   Copyright: GPL $Author: jhense $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: FireCam.hh,v $
**  Revision 1.39  2006/07/13 12:54:17  jhense
**  Added new interface getFrameStartMutex(). Other threads can be notified, when
**  frame transmission started. Raw-Mode support in SMP-kernels implemented. Internal
**  buffer-size is reduced a little bit to the size of frame (not the transmitted
**  packages).
**
**  Revision 1.38  2006/06/28 08:07:17  jhense
**  Defined NUM_DMA_BUFFERS as constant.
**
**  Revision 1.37  2006/06/16 15:19:16  jhense
**  Access to cameracapture, debug-message
**
**  Revision 1.36  2006/06/04 12:26:50  jhense
**  Throwing FireCamLostDMAPacketError when dma_single_capture fails.
**
**  Revision 1.35  2006/04/19 12:16:54  jhense
**  Made destructor virtual.
**
**  Revision 1.34  2006/04/05 14:56:30  jhense
**  FireCam is initialized with Timebase = 100us. Start and stop of sequence-feature
**  improved.
**
**  Revision 1.33  2006/04/01 11:15:13  jhense
**  Shutdown of sequence feature improved.
**
**  Revision 1.32  2006/03/09 14:55:32  jhense
**  New function measureRelativeStarttimes to get the timing of a
**  sequence.
**
**  Revision 1.31  2006/02/03 09:30:47  jhense
**  New function "getRelativeFrameStarttimes" added, where camera measures the
**  end of exposure = frame start. This cannot be retrieved when using dma.
**
**  Revision 1.30  2006/01/11 16:07:43  jhense
**  Added feature to allocate a ringbuffer in application and set it as camera-ringbuffer: setExternalRingbuffer
**  Start and end of transmission can be read with getFrameStarttime and getFrameFilltime
**
**  Revision 1.29  2005/12/23 10:53:17  jhense
**  New function dmaGetNextFreeFrame added. The grabbing-thread for dma is deactivated.
**
**  Revision 1.28  2005/12/14 16:11:25  jhense
**  Additional functions: applySettingsToSequenceRegister and selectSequenceRegister for better support of Sequence-Feature.
**
**  Revision 1.27  2005/11/06 18:56:06  mechnich
**  fixed warning caused by a missing comment line
**
**  Revision 1.26  2005/10/28 12:29:39  jhense
**  Added functions initSequenceFeature and applyParametersToSequence to support
**  the AVT-Sequence feature.
**
**  Revision 1.25  2004/12/03 09:06:43  ronneber
**  - added severals initializations to zero to make valgrind happy
**
**  Revision 1.24  2004/10/19 05:54:46  mechnich
**  added DMA stuff without testing, will probably need future fixes; added absolute control features
**
**  Revision 1.23  2004/05/28 12:10:13  mechnich
**  moved parameter handling to LMBCam class
**
**  Revision 1.22  2003/12/18 15:34:33  mechnich
**  changed return value of bytePerPixel from float to double
**
**  Revision 1.21  2003/12/05 14:14:55  mechnich
**  added missing comments and updated Doxyfile
**
**  Revision 1.20  2003/11/05 16:25:38  mechnich
**  sync
**
**  Revision 1.19  2003/10/01 13:29:07  pigorsch
**  - added parameter channel to constructor
**  - removed static member p_handlerObject
**  - added members p_using_ringbuffer, p_iso_handler_pos, p_iso_handler_start
**  - removed static flag of member p_grabberStatus
**
**  Revision 1.18  2003/08/05 14:39:39  mechnich
**  corrected errors in Format7 functionalities
**
**  Revision 1.17  2003/07/03 15:25:17  mechnich
**  - added comments
**  - improved Format7 capabilities
**  - added boolean return values to set...() functions
**
**  Revision 1.16  2003/06/12 16:13:19  mechnich
**  added comments
**
**  Revision 1.15  2003/05/22 08:29:46  mechnich
**  - minor bugfixes
**  - introduced parameterMap()
**
**  Revision 1.14  2003/05/21 14:59:09  mechnich
**  - introduced setColorCoding() function
**  - completed reading of already set camera features at startup
**  - completed framerate setting and reading
**
**  Revision 1.13  2003/05/21 09:00:47  mechnich
**  removed superfluent functions and added missing to LMBCam
**
**  Revision 1.12  2003/05/16 12:10:38  ronneber
**  - new method stopAtFrameIndex()
**
**  Revision 1.11  2003/05/12 15:42:38  mechnich
**  - camera settings are now loaded at startup
**  - framedropping set as default
**  - improved format7 compatibility
**  - added functions nComponents(), bytePerPixel() and isCompressed() to the interface
**
**  Revision 1.10  2003/05/07 00:06:29  mechnich
**  intorduced a wrapper mutex for critical libraw1394 functions,
**  liblmbcam should now be completely thread safe
**
**  Revision 1.9  2003/05/06 23:05:39  mechnich
**  - increased thread safety of library
**  - restructured parameter classes, introduced new subclasses to LMBCamParam:
**    LMBCamParamIntRange and LMBCamParamSwitch
**    TODO: add LMBCamParamDoubleRange for absolute features
**
**  Revision 1.8  2003/05/05 02:36:37  mechnich
**  temporarily removed locking and unlocking of the frame mutexes in the
**  handler thread to prevent applications from deadlocking
**
**  Revision 1.7  2003/04/24 17:02:26  mechnich
**  - added return value to setMode() and setFramerate()
**  - added full functionality to recently added functions
**
**  Revision 1.6  2003/04/23 13:28:52  mechnich
**  - fixed parameter queries for all tested cameras
**  - fixed bug in iso handler function
**  - moved p_cameraIsRunning with affiliated functions to LMBCam
**
**  Revision 1.5  2003/04/17 01:15:03  mechnich
**  - removed frame mutexes
**  - fixed bug in stopCamera()
**
**  Revision 1.4  2003/04/16 16:41:26  mechnich
**  added threaded framegrabber for non-DMA mode
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

#ifndef FIRECAM_HH
#define FIRECAM_HH

/*-------------------------------------------------------------------------
 *  Own includes
 *-------------------------------------------------------------------------*/
#include "LMBCam.hh"
#include "LMBCamParam.hh"
#include "LMBErrorHandler.hh"
#include "FireCamError.hh"
#include "AtomicCounter.hh"

/*-------------------------------------------------------------------------
 *  FireWire includes
 *-------------------------------------------------------------------------*/
#include <libraw1394/raw1394.h>
#include <libdc1394/dc1394_control.h>

/*-------------------------------------------------------------------------
 *  STL includes
 *-------------------------------------------------------------------------*/
#include <string>
#include <vector>
#include <iostream>
#include <map>
#include <sstream>
#include <algorithm>

#include <pthread.h>
#include <netinet/in.h>

#ifdef LIBLMBCAMDEBUG
#include <fstream>
#include <iomanip>
#endif
  

namespace liblmbcam
{
  class FireCamParam;

  
/*======================================================================*/
/*!
 *  \class FireCam FireCam.hh
 *  \brief The FireCam class implements an ieee1394 camera object
 *
 *  (description)
 */
/*======================================================================*/
class FireCam : public LMBCam
{
public:
/*======================================================================*/
/*! 
 *   This function returns the camera's vendor as a string
 *
 *   \return the camera's vendor
 */
/*======================================================================*/#
  std::string vendor() const;
  
/*======================================================================*/
/*! 
 *   This function returns the camera model as a string
 *
 *   \return the camera model
 */
/*======================================================================*/
  std::string model() const;
  
/*======================================================================*/
/*! 
 *   This function returns the unique id of the camera
 *
 *   \return unique id of the camera
 */
/*======================================================================*/
  std::string guid() const;
  
/*======================================================================*/
/*! 
 *   This function resets the camera to its factory defaults
 */
/*======================================================================*/
  void reset();
  
/*======================================================================*/
/*! 
 *   This function queries the camera for a feature specified by paramStr
 *
 *   \param paramStr parameter name as string
 *   \return pointer to the requested camera parameter object
 */
/*======================================================================*/
  LMBCamParam* param( const std::string& paramStr);

/*======================================================================*/
/*! 
 *   This function sets the camera mode to <em>mode</em>
 *
 *   \param mode a string containing the new mode for the camera
 *   \return true, if <em>mode</em> is supported by the camera
 *           and was also set, false otherwise
 */
/*======================================================================*/
  bool setMode( const std::string& mode);

/*======================================================================*/
/*! 
 *   This function returns the currently set mode of the camera
 *
 *   \return the currently set mode
 */
/*======================================================================*/
  std::string mode() const;

/*======================================================================*/
/*! 
 *   This function returns a vector with the names of all
 *   available camera modes
 *
 *   \return vector containing available camera modes
 */
/*======================================================================*/
  std::vector<std::string> availableModes() const;
    
/*======================================================================*/
/*! 
 *   This function sets the current framerate to <em>framerate</em>.
 *   If <em>allowRounding</em> is true, the framerate is rounded if it is
 *   not valid for the camera in the current mode. Otherwise, an error 
 *   thrown if <em>framerate</em> is invalid
 *
 *   \param framerate The framerate the camera should be set to
 *   \param allowRounding Flag which controls rounding if <em>framerate</em>
 *                        is invalid
 *   \return true, if <em>framerate</em> is supported by the camera and
 *           was also set, false otherwise
 */
/*======================================================================*/
  bool setFramerate( double framerate, bool allowRounding=false);

/*======================================================================*/
/*! 
 *   This function returns the currently set framerate of the camera
 *
 *   \return the currently set framerate
 */
/*======================================================================*/
  double framerate() const;
  
/*======================================================================*/
/*! 
 *   This function returns all fixed framerates that are valid accordding
 *   to the DCAM specification
 *
 *   \return vector containing available framerates
 */
/*======================================================================*/
  std::vector<double> availableFramerates() const;

/*======================================================================*/
/*! 
 *   This function returns all available framerates in camera mode 
 *   <em>mode</em>
 *
 *   \param mode Camera mode which should be queried for framerates 
 *   \return vector containing available framerates
 */
/*======================================================================*/
  std::vector<double> availableFrameratesInMode( const std::string& mode) const;

/*======================================================================*/
/*! 
 *   This function changes the color coding of the camera to
 *   <em>colorCoding</em>. Mode and framerate stay unchanged.
 *
 *   \param colorCoding The color coding the camera should be set to
 *   \return true, if color coding could be changed,
 *           false otherwise
 */
/*======================================================================*/
  bool setColorCoding( const std::string& colorCoding);

/*======================================================================*/
/*! 
 *   This function returns the current color coding of the camera
 *
 *   \return the current color coding
 */
/*======================================================================*/
  std::string colorCoding() const;
  
/*======================================================================*/
/*! 
 *   This function returns the offset of the left border in Format7 mode
 *
 *   \return offset of the left border in Format7 mode, 0 otherwise
 */
/*======================================================================*/
  unsigned int left() const;

/*======================================================================*/
/*! 
 *   This function sets the offset of the left border in Format7 mode
 *
 *   \param left new offset of the left border in Format7 mode
 *   \return true if left border was set, false otherwise
 */
/*======================================================================*/
  bool setLeft( unsigned int left);

/*======================================================================*/
/*! 
 *   This function returns the offset of the top border in Format7 mode
 *
 *   \return offset of the top border in Format7 mode, 0 otherwise
 */
/*======================================================================*/
  unsigned int top() const;

/*======================================================================*/
/*! 
 *   This function sets the offset of the top border in Format7 mode
 *
 *   \param top new offset of the top border in Format7 mode
 *   \return true if top border was set, false otherwise
 */
/*======================================================================*/
  bool setTop( unsigned int top);

/*======================================================================*/
/*! 
 *   This function returns the width of the camera image
 *
 *   \return width of the camera image
 */
/*======================================================================*/
  unsigned int width() const;

/*======================================================================*/
/*! 
 *   This function sets the width of the camera image
 *
 *   \param width new width of the camera image
 *   \return true if width was set, false otherwise
 */
/*======================================================================*/
  bool setWidth( unsigned int width);

/*======================================================================*/
/*! 
 *   This function returns the maximum Format7 width of the camera image 
 *
 *   \return maximum Format7 width of the camera image,
 *           0 if the camera doesn't support Format7
 */
/*======================================================================*/
  unsigned int maxWidth() const;

/*======================================================================*/
/*! 
 *   This function returns the height of the camera image
 *
 *   \return height of the camera image
 */
/*======================================================================*/
  unsigned int height() const;

/*======================================================================*/
/*! 
 *   This function sets the height of the camera image
 *
 *   \param height new height of the camera image
 *   \return true if height was set, false otherwise
 */
/*======================================================================*/
  bool setHeight( unsigned int height);

/*======================================================================*/
/*! 
 *   This function returns the maximum Format7 height of the camera image 
 *
 *   \return maximum Format7 height of the camera image,
 *           0 if the camera doesn't support Format7
 */
/*======================================================================*/
  unsigned int maxHeight() const;

/*======================================================================*/
/*! 
 *   This function sets the size of the internal ringbuffer to
 *   <em>nFrames</em>
 *
 *   \param nFrames number of frames to be buffered
 */
/*======================================================================*/
  void setRingBufferSize( unsigned int nFrames);

/*======================================================================*/
/*! 
 *   This function stops the camera and frees the buffer.
 */
/*======================================================================*/
  void stopCamera();

/*======================================================================*/
/*! 
 *   This function returns the index of the last recently grabbed frame.
 *
 *   \return index of the last recently grabbed frame
 */
/*======================================================================*/
  long mostRecentFrameIndex();

/*======================================================================*/
/*! 
 *   This function returns the ring buffer index of the last recently 
 *   grabbed frame.
 *
 *   \return ring buffer index of the last recently grabbed frame
 */
/*======================================================================*/
  int mostRecentRingBufferIndex();

/*======================================================================*/
/*! 
 *   This function waits for the grabber to start receiving the frame with
 *   index <em>frameIndex</em>.
 *
 *   \param frameIndex index of the frame to wait for
 *   \return index of the frame to wait for
 */
/*======================================================================*/
  long waitForFrameStarted( long frameIndex);

/*======================================================================*/
/*! 
 *   This function waits for the grabber to finish receiving the frame with
 *   index <em>frameIndex</em>.
 *
 *   \param frameIndex index of the frame to wait for
 *   \return index of the frame to wait for
 */
/*======================================================================*/
  long waitForFrameFinished( long frameIndex);
  
/*======================================================================*/
/*! 
 *   This function tries to lock the frame with index <em>frameIndex</em>.
 *   The virtual function <em>ringBufferOverflow()</em> is called if
 *   the grabber tries to overwrite the locked frame.
 *
 *   \param frameIndex index of the frame to lock
 *   \return true if frame was locked, false otherwise
 */
/*======================================================================*/
  bool tryLockFrame( long frameIndex);

/*======================================================================*/
/*! 
 *   This function returns a pointer to the raw image data of the frame with
 *   index <em>frameIndex</em>.
 *
 *   \param frameIndex index of the frame
 *   \return pointer to the raw image data
 */
/*======================================================================*/
  unsigned char* getFrame( long frameIndex);

/*======================================================================*/
/*! 
 *   This function unlocks the frame previously locked by
 *   <em>tryLockFrame( long frameIndex)</em>
 */
/*======================================================================*/
  void unlockFrame();

/*======================================================================*/
/*! 
 *   This function checks if the currently used color coding consists of
 *   components( e.g. RGB) or represents a compressed image( e.g. YUV).
 *
 *   \return true if image is compressed, false otherwise
 */
/*======================================================================*/
  bool isCompressed() const;

/*======================================================================*/
/*! 
 *   This function returns the number of components in a pixel.
 *   If the color coding is YUV, the function returns zero.
 *
 *   \return number of components in a pixel, 0 if image is compressed.
 */
/*======================================================================*/
  unsigned int nComponents() const;

/*======================================================================*/
/*! 
 *   This function returns the physical size of a pixel in memory.
 *
 *   \return size of a pixel in byte
 */
/*======================================================================*/
  double bytePerPixel() const;

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
 *  
 *   The following features are supported:
 *   -------------------------------------
 *   All modes:  Cur_V_Mode, Cur_V_Format, ISO_Channel, ISO_Speed, Brightness,
 *               White_Balance (color Kameras only), Shutter, Gain, Lookup-
 *               Table,TestImage
 *   Fixed modes only: Cur_V_Frm_Rate
 *   Format_7 only: Image_Position, Image_Size, Color_Coding_ID, 
 *                  Byte_Per_Packet, Binning
 */
/*======================================================================*/
 
  bool initSequenceFeature(char count = 16, bool autorewind = false);

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
  bool applyParametersToSequence(char position);

/*======================================================================*/
/*! 
 *   stopSequenceFeature sets the SEQUENCE_CTRL-Register to 0.
 *   \return true if succeeds
 */
/*======================================================================*/

  virtual bool stopSequenceFeature();

  ////Fast-edit:
  bool applySettingsToSequenceRegister();
  bool selectSequenceRegister(char position);
  

/*======================================================================*/
/*! 
 *   getFrameFilltime returns the time when the frame transfer was completed.
 *
 *   \param number Specifies the framenumber to query the filltime
 *
 *   \return completion time of frame transfer. 
 */
/*======================================================================*/ 

  struct timeval getFrameFilltime (long number);
  

/*======================================================================*/
/*! 
 *   getFrameStarttime returns the time when the frame transfer was started.
 *
 *   \param number Specifies the framenumber to query for starttime
 *
 *   \return starttime of frame transfer. 
 */
/*======================================================================*/ 

  struct timeval getFrameStarttime (long number);
  

/*-------------------------------------------------------------------------
 *  This functions are deprecated and should be replaced in a future version
 *  of liblmbcam
 *-------------------------------------------------------------------------*/
  void capture();
  unsigned char* rawImage();
  void doneWithBuffer();

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
  void setExternalRingBuffer(std::vector<unsigned char*>);
 
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
  virtual std::vector<timeval> measureRelativeFrameStarttimes(int no);
  
#ifndef LIBLMBCAMDEBUG  
protected:
#endif

/*-----------------------------------------------------------------------
 * Number of DMA-buffers for setup() 
 *-----------------------------------------------------------------------*/
#define NUM_DMA_BUFFERS 4

/*-------------------------------------------------------------------------
 *  Define bit masks
 *-------------------------------------------------------------------------*/
  static const unsigned long V_FORMAT_0=0x80000000UL; // VGA_NONCOMPRESSED
  static const unsigned long V_FORMAT_1=0x40000000UL; // SVGA_NONCOMPRESSED_1
  static const unsigned long V_FORMAT_2=0x20000000UL; // SVGA_NONCOMPRESSED_2
  static const unsigned long V_FORMAT_3=0x10000000UL; // reserved
  static const unsigned long V_FORMAT_4=0x08000000UL; // reserved
  static const unsigned long V_FORMAT_5=0x04000000UL; // reserved
  static const unsigned long V_FORMAT_6=0x02000000UL; // STILL_IMAGE
  static const unsigned long V_FORMAT_7=0x01000000UL; // SCALABLE_IMAGE_SIZE
  
  static const unsigned long V_MODE_0=0x80000000UL;
  static const unsigned long V_MODE_1=0x40000000UL;
  static const unsigned long V_MODE_2=0x20000000UL;
  static const unsigned long V_MODE_3=0x10000000UL;
  static const unsigned long V_MODE_4=0x08000000UL;
  static const unsigned long V_MODE_5=0x04000000UL;
  static const unsigned long V_MODE_6=0x02000000UL;
  static const unsigned long V_MODE_7=0x01000000UL;
  
  static const unsigned long V_COLOR_0=0x80000000UL; // MONO
  static const unsigned long V_COLOR_1=0x40000000UL; // YUV411
  static const unsigned long V_COLOR_2=0x20000000UL; // YUV422
  static const unsigned long V_COLOR_3=0x10000000UL; // YUV444
  static const unsigned long V_COLOR_4=0x08000000UL; // RGB
  static const unsigned long V_COLOR_5=0x04000000UL; // MONO16
  static const unsigned long V_COLOR_6=0x02000000UL; // RGB16
  
  static const unsigned long V_FRAMERATE_0=0x80000000UL; //  1.875 fps
  static const unsigned long V_FRAMERATE_1=0x40000000UL; //  3.75  fps
  static const unsigned long V_FRAMERATE_2=0x20000000UL; //  7.5   fps
  static const unsigned long V_FRAMERATE_3=0x10000000UL; // 15     fps
  static const unsigned long V_FRAMERATE_4=0x08000000UL; // 30     fps
  static const unsigned long V_FRAMERATE_5=0x04000000UL; // 60     fps

  static const unsigned long long AVTReg_Sequence_Ctrl =  0xFFFFF1000220ULL;
  static const unsigned long long AVTReg_Sequence_Param = 0xFFFFF1000224ULL;
  static const unsigned long long AVTReg_Adv_Inq_1 =      0xFFFFF1000040ULL;
  static const unsigned long long AVTReg_Timebase =       0xFFFFF1000208ULL;
  
  
/*======================================================================*/
/*! 
 *   Constructor
 *
 *   \param handle       raw hardware handle for the ieee1394 bus
 *   \param node         camera node 
 *   \param portNumber   camera port number
 *   \param channel      iso channel index
 *   \param libRawMutex  pointer to global raw1394 mutex
 */
/*======================================================================*/
  FireCam( raw1394handle_t handle, nodeid_t node, unsigned int portNumber,
           unsigned int channel, pthread_mutex_t* libRawMutex);

/*======================================================================*/
/*! 
 *   Destructor
 */
/*======================================================================*/
  virtual ~FireCam();

/*======================================================================*/
/*! 
 *   Start the camera to make pictures.
 *
 *   \param ringBuffer If set, the camera uses the given memory for 
 *                     buffering, otherwise an internal buffer is used.
 *
 *   \return Nothing.
 */
/*======================================================================*/ 
  void startCamera();

  void freeBuffer();
  void cleanup();

  void readSettingsFromCamera();
  virtual bool testSettings();
  bool testFormat7PacketSize();
  
/*======================================================================*/
/*! 
 *   This function returns the camera's raw hardware handle
 *
 *   \return copy of the camera's raw hardware handle
 */
/*======================================================================*/
  raw1394handle_t handle();

/*======================================================================*/
/*! 
 *   This function returns the camera's dc1394 handle
 *
 *   \return copy of the camera's dc1394 handle
 */
/*======================================================================*/
  dc1394_cameracapture getCameraCapture();

/*======================================================================*/
/*! 
 *   This function returns the camera's node number
 *
 *   \return copy of the camera's node number
 */
/*======================================================================*/
  nodeid_t node();

/*======================================================================*/
/*! 
 *   This sets the capturing of the camera in the right mode: 
 *   dma/dma-format7/standard/standard-format7
 *   Geometry of image is sent to camera.
 *   Uses the dc1394-functions.
 *
 *   \throws FireCamError
 */
/*======================================================================*/
  void setupCapture();

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
  
  

  raw1394handle_t p_handle;
  nodeid_t p_node;
  unsigned int p_channel;
  std::string p_deviceName;
  
  dc1394_camerainfo p_info;
  dc1394_cameracapture p_camera;

  void initAvailableParameters();
  void initAvailableModes();
  
  std::map<std::string, quadlet_t> p_availableModes;

  struct Mode
  {
    Mode()
            :p_id( -1), p_colorCodingID( -1), 
             p_nComponents( 0), p_bytePerPixel( -1),
             p_width( 0), p_height( 0),
             p_formatID( -1), p_modeBit( 0),
             p_colorBit( 0)
          {}
    Mode( int id, int colorCodingID,
          unsigned int nComponents, double bytePerPixel,
          unsigned int width, unsigned int height,
          int formatID, quadlet_t modeBit, quadlet_t colorBit=0)
            :p_id( id), p_colorCodingID( colorCodingID), 
             p_nComponents( nComponents), p_bytePerPixel( bytePerPixel),
             p_width( width), p_height( height),
             p_formatID( formatID), p_modeBit( modeBit),
             p_colorBit( colorBit)
           {}
    
    int p_id, p_colorCodingID;
    unsigned int p_nComponents;
    double p_bytePerPixel;
    unsigned int p_width, p_height;
    int p_formatID;
    quadlet_t p_modeBit, p_colorBit;
  };
      
  struct Framerate
  {
    Framerate()
        :p_id( -1), p_framerateBit( 0)
          {}
    Framerate( int id, quadlet_t framerateBit)
        :p_id( id), p_framerateBit( framerateBit)
          {}
    
    int p_id;
    quadlet_t p_framerateBit;
  };
  
  void setMode( const Mode& mode);
  void setFramerate( const Framerate& framerate);
    
  Mode p_currentMode;
  Framerate p_currentFramerate;

  unsigned int p_format7Left;
  unsigned int p_format7Top;
  unsigned int p_format7Width;
  unsigned int p_format7MaxWidth;
  unsigned int p_format7Height;
  unsigned int p_format7MaxHeight;
  unsigned int p_format7PacketSize;
  
/*-------------------------------------------------------------------------
 *  The following functions initialize the static lookup tables
 *-------------------------------------------------------------------------*/
  static void initFeatureMap();
  static void initColorCodingMap();
  static void initFormatMap();
  static void initSpeedMap();
  static void initFramerateMap();
  static void initModeMap();
  static void initTriggermodeMap();
    
/*-------------------------------------------------------------------------
 *  Lookup tables
 *-------------------------------------------------------------------------*/
  static std::map<std::string,int> p_features;
  static std::map<std::string,int> p_colorCodings;
  static std::map<quadlet_t,int> p_formats;
  static std::map<int,int> p_speeds;
  static std::map<double,Framerate> p_framerates;
  static std::map<std::string,Mode> p_modes;
  static std::map<int,int> p_triggerModes;

  static void* dmaThreadStartFunction( void* camera);
  static void* grabberThreadStartFunction( void* camera);
  static int IsoHandler( raw1394handle_t handle, int channel,
                         size_t length, quadlet_t* data);
  
  unsigned char* p_iso_handler_pos;
  unsigned char* p_iso_handler_start;
  
  unsigned int p_numBufferedFrames;
  bool p_dropFrames;
  pthread_t p_grabberThread;

  unsigned char* p_ringBuffer;
  std::vector<unsigned char*> _vBufFrameStart;
  pthread_mutex_t* p_libRawMutex;

  std::vector<struct timeval> p_Filltimes;
  std::vector<struct timeval> p_Starttimes;
  
  AtomicCounter<long> p_readFrameCount;
  AtomicCounter<long> p_writtenFrameCount;
  AtomicCounter<long> p_lockedFrame;

  pthread_cond_t p_frameStartCond;
  pthread_mutex_t p_frameStartMutex;
  
  pthread_cond_t p_frameFinishCond;
  pthread_mutex_t p_frameFinishMutex;

  bool _packetLostInDMA;
  
  enum GrabberStatus {INIT, READING, COMPLETE};
  //static GrabberStatus p_grabberStatus;
  GrabberStatus p_grabberStatus;

  bool supports_sequence_feature;
  bool sequence_feature_running;
  
  std::vector<unsigned char*> _vExternalRingBuffer;
  
  friend class FireCamBus;
  friend class FireCamParamIntRange;
  friend class FireCamParamAbsRange;

// #ifdef LIBLMBCAMDEBUG
//   std::ofstream filltimeFile;
// #endif
  

};

}

#include "FireCam.icc"

#endif
