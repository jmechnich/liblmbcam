/**************************************************************************
**       Title: 
**    $RCSfile: FileCam.hh,v $
**   $Revision: 1.2 $$Name:  $
**       $Date: 2003/12/18 15:34:33 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: FileCam.hh,v $
**  Revision 1.2  2003/12/18 15:34:33  mechnich
**  changed return value of bytePerPixel from float to double
**
**  Revision 1.1  2003/11/05 16:31:39  mechnich
**  initial revision
**
**
**
**************************************************************************/

#ifndef FILECAM_HH
#define FILECAM_HH

#include "LMBCam.hh"
#include "AtomicCounter.hh"
#include "FileCamError.hh"

#include <cmath>
#include <string>
#include <vector>
#include <sstream>

namespace liblmbcam
{
  class LMBCamParam;
  
  class FileCam : public LMBCam
  {
  public:
    FileCam();
    ~FileCam();
    
    void setFile( const std::string& fileName);
    
/*======================================================================*/
/*! 
 *   This function should return the camera's vendor as a string
 *
 *   \return the camera's vendor
 */
/*======================================================================*/#
    std::string vendor() const;

/*======================================================================*/
/*! 
 *   This function should return the camera model as a string
 *
 *   \return the camera model
 */
/*======================================================================*/
    std::string model() const;
  
/*======================================================================*/
/*! 
 *   This function should return a unique id of the camera
 *
 *   \return unique id of the camera
 */
/*======================================================================*/
    std::string guid() const;
  
/*======================================================================*/
/*! 
 *   This function should reset the camera to its factory defaults
 */
/*======================================================================*/
    void reset();
  
/*======================================================================*/
/*! 
 *   This function should query the camera for a feature specified by paramStr
 *
 *   \param paramStr parameter name as string
 *   \return pointer to the requested camera parameter object
 */
/*======================================================================*/
    LMBCamParam* param( const std::string& paramStr);

/*======================================================================*/
/*! 
 *   This function should returns a vector with the names of all 
 *   available camera features
 *
 *   \return vector containing available camera feature names
 */
/*======================================================================*/
    std::vector<std::string> availableParameters() const;  
  
/*======================================================================*/
/*! 
 *   This function returns a map containing all available camera features
 *
 *   \return map containing available camera features
 */
/*======================================================================*/
    std::map<std::string,LMBCamParam*> parameterMap() const;

/*======================================================================*/
/*! 
 *   This function should set the camera mode to <em>mode</em>
 *
 *   \param mode a string containing the new mode for the camera
 *   \return true, if <em>mode</em> is supported by the camera
 *           and was also set, false otherwise
 */
/*======================================================================*/
    bool setMode( const std::string& mode);

/*======================================================================*/
/*! 
 *   This function should return the currently set mode of the camera
 *
 *   \return the currently set mode
 */
/*======================================================================*/
    std::string mode() const;

/*======================================================================*/
/*! 
 *   This function should return a vector with the names of all
 *   available camera modes
 *
 *   \return vector containing available camera modes
 */
/*======================================================================*/
    std::vector<std::string> availableModes() const;

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
  bool setFramerate( double framerate, bool allowRounding=false);

/*======================================================================*/
/*! 
 *   This function should return the currently set framerate of the camera
 *
 *   \return the currently set framerate
 */
/*======================================================================*/
  double framerate() const;
  
/*======================================================================*/
/*! 
 *   This function should return all valid framerates (not only for the
 *   current mode!)
 *
 *   \return vector containing available framerates
 */
/*======================================================================*/
  std::vector<double> availableFramerates() const;

/*======================================================================*/
/*! 
 *   This function should return all available framerates in camera mode 
 *   <em>mode</em>
 *
 *   \param mode Camera mode which should be queried for framerates 
 *   \return vector containing available framerates
 */
/*======================================================================*/
 std::vector<double> availableFrameratesInMode( const std::string& mode) const;

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
  bool setColorCoding( const std::string& colorCoding);

/*======================================================================*/
/*! 
 *   This function should return the current color coding of the camera
 *
 *   \return the current color coding
 */
/*======================================================================*/
  std::string colorCoding() const;
  
/*======================================================================*/
/*! 
 *   This function should return the offset of the left border
 *
 *   \return offset of the left border
 */
/*======================================================================*/
  unsigned int left() const;

/*======================================================================*/
/*! 
 *   This function should set the offset of the left border
 *
 *   \param left new offset of the left border
 *   \return true if left border was set, false otherwise
 */
/*======================================================================*/
  bool setLeft( unsigned int left);

/*======================================================================*/
/*! 
 *   This function should return the offset of the top border
 *
 *   \return offset of the top border, 0 otherwise
 */
/*======================================================================*/
  unsigned int top() const;

/*======================================================================*/
/*! 
 *   This function should set the offset of the top border
 *
 *   \param top new offset of the top border
 *   \return true if top border was set, false otherwise
 */
/*======================================================================*/
  bool setTop( unsigned int top);

/*======================================================================*/
/*! 
 *   This function should return the image width
 *
 *   \return width in pixels
 */
/*======================================================================*/
  unsigned int width() const;

/*======================================================================*/
/*! 
 *   This function should set the width of the camera image
 *
 *   \param width new width of the camera image
 *   \return true if width was set, false otherwise
 */
/*======================================================================*/
  bool setWidth( unsigned int width);

/*======================================================================*/
/*! 
 *   This function should return the maximum image width
 *
 *   \return maximum width in pixels
 */
/*======================================================================*/
  unsigned int maxWidth() const;

/*======================================================================*/
/*! 
 *   This function should return the image height
 *
 *   \return height in pixels
 */
/*======================================================================*/
  unsigned int height() const;

/*======================================================================*/
/*! 
 *   This function should set the height of the camera image
 *
 *   \param height new height of the camera image
 *   \return true if height was set, false otherwise
 */
/*======================================================================*/
  bool setHeight( unsigned int height);

/*======================================================================*/
/*! 
 *   This function should return the maximum image height
 *
 *   \return maximum height in pixels
 */
/*======================================================================*/
  unsigned int maxHeight() const;

/*======================================================================*/
/*! 
 *   This function should set the size of the internal ringbuffer to
 *   <em>nFrames</em>
 *
 *   \param nFrames number of frames to be buffered
 */
/*======================================================================*/
  void setRingBufferSize( unsigned int nFrames);

/*======================================================================*/
/*! 
 *   This function should stop the camera and free the buffer.
 */
/*======================================================================*/
  void stopCamera();

/*======================================================================*/
/*! 
 *   This function should return the index of the last recently grabbed frame.
 *
 *   \return index of the last recently grabbed frame
 */
/*======================================================================*/
  long mostRecentFrameIndex();

/*======================================================================*/
/*! 
 *   This function should wait for the grabber to start receiving the
 *   frame with index <em>frameIndex</em>.
 *
 *   \param frameIndex index of the frame to wait for
 *   \return index of the frame to wait for
 */
/*======================================================================*/
    long waitForFrameStarted( long frameIndex);

/*======================================================================*/
/*! 
 *   This function should wait for the grabber to finish receiving the
 *   frame with index <em>frameIndex</em>.
 *
 *   \param frameIndex index of the frame to wait for
 *   \return index of the frame to wait for
 */
/*======================================================================*/
    long waitForFrameFinished( long frameIndex);
  
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
  bool tryLockFrame( long frameIndex);

/*======================================================================*/
/*! 
 *   This function should return a pointer to the raw image data of the
 *   frame with index <em>frameIndex</em>.
 *
 *   \param frameIndex index of the frame
 *   \return pointer to the raw image data
 */
/*======================================================================*/
  unsigned char* getFrame( long frameIndex);

/*======================================================================*/
/*! 
 *   This function should unlock the frame previously locked by
 *   <em>tryLockFrame( long frameIndex)</em>
 */
/*======================================================================*/
  void unlockFrame();

/*======================================================================*/
/*! 
 *   This function should check if the currently used color coding consists of
 *   components( e.g. RGB) or represents a compressed image( e.g. YUV).
 *
 *   \return true if image is compressed, false otherwise
 */
/*======================================================================*/
  bool isCompressed() const;

/*======================================================================*/
/*! 
 *   This function should return the number of components in a pixel.
 *   If the color coding is YUV, the function returns zero.
 *
 *   \return number of components in a pixel, 0 if image is compressed.
 */
/*======================================================================*/
  unsigned int nComponents() const;

/*======================================================================*/
/*! 
 *   This function should return the physical size of a pixel in memory.
 *
 *   \return size of a pixel in byte
 */
/*======================================================================*/
  double bytePerPixel() const;

protected:
    FileCam( const FileCam& cam)
          {}
    
    void operator=( const FileCam& cam)
          {}
    
/*======================================================================*/
/*! 
 *   This function should start the camera
 */
/*======================================================================*/
    virtual void startCamera()=0;

    static void* grabberThreadStartFunction( void* camera);

    std::string p_filename;
    unsigned char* p_data;

    std::string p_colorCoding;
    float p_framerate;
    unsigned int p_left;
    unsigned int p_top;
    unsigned int p_width;
    unsigned int p_height;
    unsigned int p_maxWidth;
    unsigned int p_maxHeight;
    
    AtomicCounter<long> p_readFrameCount;
    AtomicCounter<long> p_writtenFrameCount;

    pthread_t p_grabberThread;

    pthread_cond_t p_frameStartCond;
    pthread_mutex_t p_frameStartMutex;
    
    pthread_cond_t p_frameFinishCond;
    pthread_mutex_t p_frameFinishMutex;
    
  };
}

#include "FileCam.icc"

#endif // FILECAM_HH
