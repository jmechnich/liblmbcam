 /**************************************************************************
**       Title: 
**    $RCSfile: FilltimeBugCheck.cc,v $
**   $Revision: 1.2 $$Name:  $
**       $Date: 2006/06/28 09:13:25 $
**   Copyright: GPL $Author: jhense $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: FilltimeBugCheck.cc,v $
**  Revision 1.2  2006/06/28 09:13:25  jhense
**  Setting shutter to 10ms, first setup mode, then set width/height.
**
**  Revision 1.1  2006/06/22 14:17:36  jhense
**  Added to repository.
**
**  Revision 1.8  2005/09/13 07:46:23  jhense
**  *** empty log message ***
**
**  Revision 1.6  2004/10/19 05:50:12  mechnich
**  changed LMBError interface
**
**  Revision 1.5  2003/05/21 08:57:22  mechnich
**  changed interface of liblmbcam again
**
**  Revision 1.4  2003/01/20 10:22:24  mechnich
**  *** empty log message ***
**
**  Revision 1.3  2002/12/18 12:55:50  mechnich
**  corrected error in recognition of the PYRO WEBCAM
**
**  Revision 1.2  2002/12/10 02:44:42  mechnich
**  added preliminary support for the Bayer C301fc in test_liveWidget
**  added more tests to test_FireCam
**
**  Revision 1.1  2002/12/04 13:17:46  mechnich
**  initial revision
**
**
**
**************************************************************************/

#include <LMBCamBusIndex.hh>
#include <LMBCamParam.hh>
#include <LMBError.hh>
#include <LMBErrorHandler.hh>

#include <iomanip>
#include <cstdlib>
#include <sys/time.h>

using namespace liblmbcam;
using namespace std;


int main( int argc, char** argv)
{
  LMBErrorHandler::ErrorHandler()->setMode( LMBErrorHandler::THROW);

  int noFrames = 32;
  
  try
  {
    /*---------------------------------------------------------------------
     *  Test bus
     *---------------------------------------------------------------------*/
    std::cout << "Creating bus...";
    
    LMBCamBusIndex bus;

    std::cout << "ok" << std::endl << std::endl;
    
    std::cout << "Number of cameras found: "
              << bus.nCameras() << std::endl << std::endl;
    
    /*---------------------------------------------------------------------
     *  Test camera(s)
     *---------------------------------------------------------------------*/
    for( unsigned int count=0; count < bus.nCameras(); ++count)
    {
      LMBCam* cam = bus.cameraByIndex( count);
      
      cam->reset();
      
      /*-------------------------------------------------------------------
       *  Print camera information
       *-------------------------------------------------------------------*/
      std::cout << "Camera No. " << count << ":" << std::endl << std::endl;
      std::cout << "Vendor: " << cam->vendor() << std::endl;
      std::cout << "Model: " << cam->model() << std::endl;
      std::cout << "GUID: " << cam->guid() << std::endl;

      /*-------------------------------------------------------------------
       *  Setup camera for capture
       *-------------------------------------------------------------------*/
      std::cout << "Trying to set up mode 'Format7Mode0 MONO'" << std::endl;
      if (!cam->setMode( "Format7Mode0 MONO"))
         cout << "Error setting Mode.\n";

      std::cout << "Trying to set width to 1392." << std::endl;
      if(!cam->setWidth(1392))
          cout << "Error setting width.\n";
      
      std::cout << "Trying to set height to 1040." << std::endl;
      if(!cam->setHeight(1040))
          cout << "Error setting height.\n";
      
      std::cout << "Trying to set framerate to 15fps" << std::endl;
      if (!cam->setFramerate(15))
          cout << "Error setting Framerate.\n";

      std::cout << "Setting shutter to 10 ms" << std::endl;
      cam->param("shutter")->setValue(100);

      std::cout << "Setting up DMA" << std::endl;
      cam->param("DMA")->setValue(1);

      
      cam->startCameraRunNFrames( noFrames);
      std::cout << "ok" << std::endl;

   timeval tv;
   gettimeofday(&tv, 0);
  
   cout << "Starting Camera " << tv.tv_sec << "_"
        << std::setfill('0') << std::setw(6)
 	       << tv.tv_usec << "\n";

      /*-------------------------------------------------------------------
       *  Print out filltimes
       *-------------------------------------------------------------------*/
      cout << "Filltimes must be in ascending order.\n";
      
      timeval lastFilltime = { 0, 0};
      timeval diffFilltime;
      
      for (int i = 0; i < noFrames; ++i)
      {
        cam->waitForFrameFinished(i);
        timeval curFilltime = cam->getFrameFilltime(i);
        
        cout << curFilltime.tv_sec << " " 
             << setfill('0') << setw(6) << curFilltime.tv_usec;
        
        timersub(&curFilltime, &lastFilltime, &diffFilltime);
        cout << " Difference: "
             << diffFilltime.tv_sec << " " 
             << setfill('0') << setw(6) 
             << diffFilltime.tv_usec << endl;
        lastFilltime = curFilltime;
        
      }
      
      /*-------------------------------------------------------------------
       *  Stop camera
       *-------------------------------------------------------------------*/
      cam->stopCamera();
      gettimeofday(&tv, 0);
      
      cout << "Stopping Camera " << tv.tv_sec << "_"
           << std::setfill('0') << std::setw(6)
           << tv.tv_usec << "\n";

    }//for
  }
  catch( LMBError& e)
  {
    std::cout << e.str() << std::endl;
    exit(1);
  }
  std::cout << "Exiting..." << std::endl;  
  return 0;
}
