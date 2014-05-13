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

