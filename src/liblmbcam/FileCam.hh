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
    
    std::string vendor() const;
    std::string model() const;
    std::string guid() const;
    void reset();
    
    LMBCamParam* param( const std::string& paramStr);
    
    std::vector<std::string> availableParameters() const;  
    std::map<std::string,LMBCamParam*> parameterMap() const;
    
    bool setMode( const std::string& mode);
    std::string mode() const;
    std::vector<std::string> availableModes() const;
    bool setFramerate( double framerate, bool allowRounding=false);
    double framerate() const;
    std::vector<double> availableFramerates() const;
    std::vector<double> availableFrameratesInMode( const std::string& mode) const;
    bool setColorCoding( const std::string& colorCoding);
    std::string colorCoding() const;
    
    unsigned int left() const;
    bool setLeft( unsigned int left);
    unsigned int top() const;
    bool setTop( unsigned int top);
    unsigned int width() const;
    bool setWidth( unsigned int width);
    unsigned int maxWidth() const;
    unsigned int height() const;
    bool setHeight( unsigned int height);
    unsigned int maxHeight() const;
    
    void setRingBufferSize( unsigned int nFrames);
    void stopCamera();
    long mostRecentFrameIndex();
    long waitForFrameStarted( long frameIndex);
    long waitForFrameFinished( long frameIndex);
    bool tryLockFrame( long frameIndex);
    unsigned char* getFrame( long frameIndex);
    void unlockFrame();
    
    bool isCompressed() const;
    unsigned int nComponents() const;
    double bytePerPixel() const;
    
protected:
    FileCam( const FileCam& cam)
          {}
    void operator=( const FileCam& cam)
          {}
    
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
