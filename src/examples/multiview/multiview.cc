/**************************************************************************
 **       Title: 
 **    $RCSfile: multiview.cc,v $
 **   $Revision: 1.4 $$Name:  $
 **       $Date: 2004/10/19 05:49:11 $
 **   Copyright: GPL $Author: mechnich $
 ** Description:
 **
 **    
 **
 **-------------------------------------------------------------------------
 **
 **  $Log: multiview.cc,v $
 **  Revision 1.4  2004/10/19 05:49:11  mechnich
 **  changed LMBError interface
 **
 **  Revision 1.3  2003/10/05 19:32:03  mechnich
 **  removed compiler warnings
 **
 **  Revision 1.2  2003/10/01 14:24:59  pigorsch
 **  - check if there are some cameras
 **
 **  Revision 1.1  2003/10/01 13:40:07  pigorsch
 **  - initial revision
 **
 **
 **
 **************************************************************************/#
 
#include <QApplication>
 
#include <LMBCamBusIndex.hh>
 
#include "DisplayWidget.hh"
 
#include <LMBError.hh>
#include <LMBErrorHandler.hh>
#include <iostream>
#include <vector>
 
int main(int argc, char** argv)
{
  liblmbcam::LMBErrorHandler::ErrorHandler()->setMode(liblmbcam::LMBErrorHandler::THROW);

  try
  {
    liblmbcam::LMBCamBusIndex bus;

    if (bus.nCameras()!=0)
    {

      QApplication a(argc, argv);
      std::vector<DisplayWidget*> w;
   
      for ( unsigned int i=0; i<bus.nCameras(); ++i)
      {
        liblmbcam::LMBCam* cam=bus.cameraByIndex(i);
        cam->reset();
        cam->setMode("640x480 MONO");
        cam->setFramerate( 30);
        DisplayWidget* display=new DisplayWidget(cam);
        display->show();
        w.push_back(display);
      }
          
      // loop until application is closed
      bool closed=false;
      bool newframes=false;
    
      do 
      {
        // if display widget exists and a new frame is availible -> update
        for (std::vector<DisplayWidget*>::iterator p=w.begin();
             p!=w.end();
             ++p)
        {
          if ((*p)->newFrameAvailable()) (*p)->updateImage();
        }
    
        do 
        {
          if (a.hasPendingEvents())
          {
            while (a.hasPendingEvents())
            {
              a.processEvents();
            }
          }
          else
          {
            usleep(1000);
          }
        
          newframes=false;
          for (std::vector<DisplayWidget*>::iterator p=w.begin();
               p!=w.end();
               ++p)
          {
            if ((*p)->newFrameAvailable()) newframes=true;
            if ((*p)->isHidden()) closed=true;
          
          }
        
        
        } while(!closed && !newframes);
      } while (!closed);
    
    
      for (std::vector<DisplayWidget*>::iterator p=w.begin();
           p!=w.end();
           ++p)
      {
        delete (*p);
      }
    }
    else
    {
      std::cerr << "No cameras found\n";
    }
  }
  catch (liblmbcam::LMBError& e)
  {
    std::cerr << "Error: " << e.str() << std::endl;
  }
  catch (...)
  {
    std::cerr << "unknown error!\n";
  }
  
  return 0;
}

