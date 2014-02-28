/**************************************************************************
**       Title: 
**    $RCSfile: LMBCamML.hh,v $
**   $Revision: 1.6 $$Name:  $
**       $Date: 2004/10/19 06:17:00 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: LMBCamML.hh,v $
**  Revision 1.6  2004/10/19 06:17:00  mechnich
**  *** empty log message ***
**
**  Revision 1.5  2004/03/22 15:46:04  ronneber
**  - just some debugging output
**
**  Revision 1.4  2003/11/26 16:38:14  ronneber
**  - extended to multi component images (not ready yet)
**
**  Revision 1.3  2003/05/22 16:29:39  ronneber
**  - improved  GUIThread to use less process time
**
**  Revision 1.2  2003/05/16 12:22:17  ronneber
**  - guithread does not call updateImage() twice for the same image
**  - extended printActualParametersML()
**
**  Revision 1.1  2003/05/13 09:33:56  ronneber
**  initial revision
**
**
**
**************************************************************************/

// remove matlab's stupid printf definition
#ifdef printf
#undef printf
#endif

#include <unistd.h> // usleep

#include <qapplication.h>
#include <pthread.h>
#include <qpushbutton.h>

#include <QlmbCamMainWidget.hh>

pthread_t     _threadid = 0;
QApplication* _app = NULL;
QlmbCamMainWidget*  _camWid = NULL;
liblmbcam::FireCam* _actualCameraForCamWid = NULL;

int           _i = 0;
bool          _applicationIsLocked = false;

/*======================================================================*/
/*! 
 *   QApplication GUI Thread
 *
 *   \param 
 *
 *   \exception 
 *
 *   \return
 */
/*======================================================================*/
void* _showLiveWindowGUIThread( void* dummy)
{
  if( _app == NULL)
  {
    char* argv[] = {"lmbcam_matlab"};
    int argc = 1;
    _app = new QApplication( argc, argv );
    
    _camWid = new QlmbCamMainWidget( _actualCameraForCamWid);
    
    _app->setMainWidget( _camWid );
    _camWid->show();



    long lastFrameIndex = -2;

    try
    {
      while( !_camWid->wasClosed())
      {
        lastFrameIndex = _actualCameraForCamWid->mostRecentFrameIndex();
        _camWid->updateImage();
        do
        {
          if( _app->hasPendingEvents())
          {
            _app->processEvents();
          }
          else
          {
            usleep( 1000);
          }
          
        } while( lastFrameIndex == 
                 _actualCameraForCamWid->mostRecentFrameIndex()) ;
        
        
      }
    }
    catch( ...)
    {
      mexPrintf( "cought exception in showLiveWindowGUIThread!\n");
    }
    
     delete _app;
    _app = NULL;
    _camWid = NULL;
    _threadid = 0;
    pthread_exit((void *) 0);
  }
  else
  {
    printf( "Qt already running");
  }
  
    
  return NULL;
}
  






/*-------------------------------------------------------------------------
 *  provide LMBCam methods with matlab-style parameters
 *  The creator remains the owner of the LMBCam object!
 *-------------------------------------------------------------------------*/
class LMBCamML
{	
public:
  LMBCamML( liblmbcam::FireCam* cam)
          :_cam( cam)
        {};

  void callMethodByName( std::string name,
                         const InputArgs& argsIn, OutputArgs& argsOut)
        {
          if( name == "LMBCam_reset")    
              resetML(argsIn, argsOut);

          else if( name == "LMBCam_availableModes") 
              availableModesML( argsIn, argsOut);

          else if( name == "LMBCam_availableParameters") 
              availableParametersML( argsIn, argsOut);

          else if( name == "LMBCam_printActualParameters") 
              printActualParametersML( argsIn, argsOut);

          else if( name == "LMBCam_mode") 
              modeML( argsIn, argsOut);

          else if( name == "LMBCam_setMode") 
              setModeML( argsIn, argsOut);

          else if( name == "LMBCam_framerate") 
              framerateML( argsIn, argsOut);

          else if( name == "LMBCam_setFramerate") 
              setFramerateML( argsIn, argsOut);

          else if( name == "LMBCam_setRingBufferSize") 
              setRingBufferSizeML( argsIn, argsOut);

          else if( name == "LMBCam_paramSetValue") 
              paramSetValueML( argsIn, argsOut);

          else if( name == "LMBCam_startCameraRunContinuous") 
              startCameraRunContinuousML( argsIn, argsOut);

           else if( name == "LMBCam_startCameraRunNFrames") 
              startCameraRunNFramesML( argsIn, argsOut);

          else if( name == "LMBCam_stopCamera") 
              stopCameraML( argsIn, argsOut);

          else if( name == "LMBCam_waitForFrameFinished") 
              waitForFrameFinishedML( argsIn, argsOut);

          else if( name == "LMBCam_mostRecentFrameIndex") 
              mostRecentFrameIndexML( argsIn, argsOut);

          else if( name == "LMBCam_getFrame") 
              getFrameML( argsIn, argsOut);

          else if( name == "LMBCam_showLiveWindow") 
              showLiveWindowML( argsIn, argsOut);

          else
          {
            char errmsg[1024];
            sprintf( errmsg, "No method '%s' in LMBCam class!",
                     name.c_str());
            mexErrMsgTxt( errmsg);
          }
        }
  
          

  void resetML( const InputArgs& argsIn, OutputArgs& argsOut)
        {
//          mexPrintf("resetML called\n");
          argsIn.assertNArgs( 2 + 0);
          argsOut.assertNArgs( 0);
          _cam->reset();
        }
          
  void availableModesML( const InputArgs& argsIn, OutputArgs& argsOut)
        {
          argsIn.assertNArgs( 2 + 0);
          argsOut.assertNArgs( 1);
          std::vector<std::string> modes = _cam->availableModes();
          
          int ndims = 1;
          int extents[ndims];
          extents[0] = modes.size();
          mxArray* cellArray = mxCreateCellArray( ndims, extents);

          for( int i = 0; i < modes.size(); ++i)
          {
            mxSetCell( cellArray, i, mxCreateString( modes[i].c_str()));
          }
          argsOut[0] = cellArray;
          
        }
  
  void availableParametersML( const InputArgs& argsIn, OutputArgs& argsOut)
        {
          argsIn.assertNArgs( 2 + 0);
          argsOut.assertNArgs( 1);
          std::vector<std::string> params = _cam->availableParameters();
          
          int ndims = 1;
          int extents[ndims];
          extents[0] = params.size();
          mxArray* cellArray = mxCreateCellArray( ndims, extents);

          for( int i = 0; i < params.size(); ++i)
          {
            mxSetCell( cellArray, i, mxCreateString( params[i].c_str()));
          }
          argsOut[0] = cellArray;
          
        }
  
  void printActualParametersML( const InputArgs& argsIn, OutputArgs& argsOut)
        {
          argsIn.assertNArgs( 2 + 0);
          argsOut.assertNArgs( 0);
          std::vector<std::string> paramNames = _cam->availableParameters();
          
          /*---------------------------------------------------------------
           *  find longest string for formatting
           *---------------------------------------------------------------*/
          int maxNameLen = std::string("most recent frame index").size();
          for( int i = 0; i < paramNames.size(); ++i)
          {
            if( paramNames[i].size() > maxNameLen) 
                maxNameLen = paramNames[i].size();
          }
          char formatString[256];
          sprintf( formatString, "%%%ds", maxNameLen);
          
          /*---------------------------------------------------------------
           *  print each parameter with actual value and range
           *---------------------------------------------------------------*/
          mexPrintf( "\nCamera '%s %s' (GUID = '%s')\n\n", 
                     _cam->vendor().c_str(),
                     _cam->model().c_str(),
                     _cam->guid().c_str());
          
          mexPrintf( formatString, "actual mode");
          mexPrintf( ": '%s'\n", _cam->mode().c_str());

          mexPrintf( formatString, "frame rate");
          mexPrintf( ": %g\n", _cam->framerate());


          
          mexPrintf( formatString, "status");
          if( _cam->isRunning())
          {
            if( _cam->stopAtFrameIndex() == LONG_MAX)
            {
              mexPrintf( ": running continuous\n");
            }
            else
            {
               mexPrintf( ": running until frame %ld\n", 
                          _cam->stopAtFrameIndex());
            }
          }
          else
          {
            mexPrintf( ": stopped\n");
          }
                
          mexPrintf( formatString, "most recent frame index");
          mexPrintf( ": %ld\n", _cam->mostRecentFrameIndex());
          
          mexPrintf( "\nParameters:\n");
          

          for( int i = 0; i < paramNames.size(); ++i)
          {
            mexPrintf( formatString, paramNames[i].c_str());
            liblmbcam::LMBCamParam* param = _cam->param( paramNames[i]);
            
            mexPrintf( ": %5g  (%g - %g)\n", 
                       param->asFloat(),
                       param->minValueAsFloat(),
                       param->maxValueAsFloat());
          }
        }
  

  void modeML( const InputArgs& argsIn, OutputArgs& argsOut)
        {
          argsIn.assertNArgs( 2 + 0);
          argsOut.assertNArgs( 1);
          argsOut.setValueString( 0, _cam->mode());
        }
  
  void setModeML( const InputArgs& argsIn, OutputArgs& argsOut)
        {
          argsIn.assertNArgs( 2 + 1);
          argsOut.assertNArgs( 0);
          _cam->setMode( argsIn.asString(2 ));
        }
  
  void framerateML( const InputArgs& argsIn, OutputArgs& argsOut)
        {
          argsIn.assertNArgs( 2 + 0);
          argsOut.assertNArgs( 1);
          argsOut.setValueDouble( 0, _cam->framerate());
        }
  
  void setFramerateML( const InputArgs& argsIn, OutputArgs& argsOut)
        {
          argsIn.assertNArgs( 2 + 1);
          argsOut.assertNArgs( 0);
          _cam->setFramerate( argsIn.asDouble(2 ));
        }
  
  void setRingBufferSizeML( const InputArgs& argsIn, OutputArgs& argsOut)
        {
          argsIn.assertNArgs( 2 + 1);
          argsOut.assertNArgs( 0);
          _cam->setRingBufferSize( (unsigned int)argsIn.asDouble(2 ));
        }
  
  void paramSetValueML( const InputArgs& argsIn, OutputArgs& argsOut)
        {
          argsIn.assertNArgs( 2 + 2);
          argsOut.assertNArgs( 0);
          liblmbcam::LMBCamParam* p = _cam->param( argsIn.asString( 2));
          p->setValue( float( argsIn.asDouble(3)));
        }
  
  void startCameraRunContinuousML( const InputArgs& argsIn, 
                                   OutputArgs& argsOut)
        {
          argsIn.assertNArgs( 2 + 0);
          argsOut.assertNArgs( 0);
          _cam->startCameraRunContinuous();
        }
          
  void startCameraRunNFramesML( const InputArgs& argsIn, OutputArgs& argsOut)
        {
          argsIn.assertNArgs( 2 + 1);
          argsOut.assertNArgs( 0);
          _cam->startCameraRunNFrames( (long)argsIn.asDouble(2));
        }
          
  void stopCameraML( const InputArgs& argsIn, OutputArgs& argsOut)
        {
          argsIn.assertNArgs( 2 + 0);
          argsOut.assertNArgs( 0);
          _cam->stopCamera();
        }
          

  void mostRecentFrameIndexML( const InputArgs& argsIn, OutputArgs& argsOut)
        {
          argsIn.assertNArgs( 2 + 0);
          argsOut.assertNArgs( 1);
          long frameIndex = _cam->mostRecentFrameIndex();
          argsOut.setValueDouble( 0, frameIndex);
        }

  void waitForFrameFinishedML( const InputArgs& argsIn, OutputArgs& argsOut)
        {
          argsIn.assertNArgs( 2 + 1);
          argsOut.assertNArgs( 0);
          _cam->waitForFrameFinished( (long)argsIn.asDouble(2));
        }
          
  void getFrameML( const InputArgs& argsIn, OutputArgs& argsOut)
        {
          argsIn.assertNArgs( 2 + 1);
          argsOut.assertNArgs( 1);
          long frameIndex = (long)argsIn.asDouble( 2);
          
          // call original function
          const unsigned char* img = _cam->getFrame( frameIndex);
          
          // check if we got an image
          if( img == 0)
          {
            char errmsg[1024];
            sprintf( errmsg, "Frame with index %ld is not "
                     "in Ringbuffer", frameIndex);
            
            mexErrMsgTxt( errmsg);
          }

          // write image into matlab array 
          int width = _cam->width();
          int height = _cam->height();

          if( _cam->colorCoding() == "MONO")
          {
            int dims[3] = { height , width , 1};
            
            argsOut[0] = mxCreateNumericArray(3, dims, mxUINT8_CLASS, mxREAL);
            unsigned char* trgP = (unsigned char*) mxGetData( argsOut[0]);
            for( int col = 0; col < width; ++col)
            {
              const unsigned char* srcP = img + col;
              {
                for( int row = 0; row < height; ++row)
                {
                  *trgP = *srcP;
                  ++trgP;
                  srcP += width;
                }
              }
            }
          }
          else if( _cam->colorCoding() == "RGB")
          {
            int nComponents = 3;
            int nBytesPerComponent = 1;
            int width = _cam->width();
            int height = _cam->height();
            int dims[3] = { height , width , nComponents};
            
            argsOut[0] = mxCreateNumericArray(3, dims, mxUINT8_CLASS, mxREAL);
            unsigned char* trgP = (unsigned char*) mxGetData( argsOut[0]);
            int srcRowOffset = width*nComponents;
            for( int p = 0; p < nComponents; ++p)
            {
              for( int col = 0; col < width; ++col)
              {
                const unsigned char* srcP = img + p + col * nComponents;
                {
                  for( int row = 0; row < height; ++row)
                  {
                    *trgP = *srcP;
                    ++trgP;
                    srcP += srcRowOffset;
                  }
                }
              }
            }
          }
          else if( _cam->colorCoding() == "YUV444")
          {
            int nComponents = 3;
            int nBytesPerComponent = 1;
            int width = _cam->width();
            int height = _cam->height();
            int dims[3] = { height , width , nComponents};
            
            argsOut[0] = mxCreateNumericArray(3, dims, mxUINT8_CLASS, mxREAL);
            unsigned char* trgP = (unsigned char*) mxGetData( argsOut[0]);
            int srcRowOffset = width*nComponents;
            for( int p = 0; p < nComponents; ++p)
            {
              int pixelcomponent;
              if( p == 0) pixelcomponent = 1; // Y-value
              if( p == 1) pixelcomponent = 0; // U-value
              if( p == 2) pixelcomponent = 2; // V-value
               
              for( int col = 0; col < width; ++col)
              {
                const unsigned char* srcP = img + pixelcomponent 
                  + col * nComponents;
                {
                  for( int row = 0; row < height; ++row)
                  {
                    *trgP = *srcP;
                    ++trgP;
                    srcP += srcRowOffset;
                  }
                }
              }
            }
          }
          else
          {
            char errmsg[1024];
            sprintf( errmsg, "Color coding `%s' not yet supported.\n"
                     "Please use a mode with `MONO', `RGB' or 'YUV444' "
                     "color coding", _cam->colorCoding().c_str());
            mexErrMsgTxt( errmsg);
          }
            
        }
  
  void showLiveWindowML( const InputArgs& argsIn, OutputArgs& argsOut)
        {
          if( _threadid == 0)
          {
            _actualCameraForCamWid = _cam;
            int rc = pthread_create( &_threadid, NULL, _showLiveWindowGUIThread, NULL);
            if (rc){
              printf("ERROR; return code from pthread_create() is %d\n", rc);
              exit(-1);
            }
          }
        }
  

private:
  
  liblmbcam::FireCam* _cam;
};

