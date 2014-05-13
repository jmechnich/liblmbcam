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

#include <cstdlib>
#include <cstdio>
#include <signal.h>   // block CTRL-C during sensitive operations
#include <mex.h>
#include <LMBCam.hh>
#include <FireCam.hh>
#include <FireCamBus.hh>

#include "InputArgs.hh"
#include "OutputArgs.hh"
#include "LMBCamML.hh"
#include "FireCamBusML.hh"

static const int MAX_FIREWIRE_BOARDS = 8;




/*-------------------------------------------------------------------------
 *  static variables to keep the objects from one call to another
 *-------------------------------------------------------------------------*/
static bool                   _initialized = false;
static FireCamBusML*          _fireCamBusses[MAX_FIREWIRE_BOARDS];
static std::map<unsigned int, LMBCamML* >* _camObjects = NULL;



/*-------------------------------------------------------------------------
 *  class for blocking CTRL-C and unblockin it in destructor (to have
 *  automatic unblocking, when program exit's anywhere)
 *-------------------------------------------------------------------------*/
class MySignalBlocker
{
public:
  MySignalBlocker( sigset_t signalMask)
          : _signalMask( signalMask)
        {
          sigprocmask (SIG_BLOCK, &_signalMask, NULL);
        }
  
  ~MySignalBlocker()
        {
          sigprocmask (SIG_UNBLOCK, &_signalMask, NULL);
        }
private:
  sigset_t _signalMask;
};





/*-------------------------------------------------------------------------
 *  Initialization at first call to mexFunction
 *-------------------------------------------------------------------------*/
static void _initialize()
{
  /*-----------------------------------------------------------------------
   *  throw exceptions on errors
   *-----------------------------------------------------------------------*/
  liblmbcam::LMBErrorHandler::ErrorHandler()->setMode( 
      liblmbcam::LMBErrorHandler::THROW);
  
  /*-----------------------------------------------------------------------
   *  create map for storing camera objects
   *-----------------------------------------------------------------------*/
  _camObjects = new std::map<unsigned int, LMBCamML* >;
  _initialized = true;

  /*-----------------------------------------------------------------------
   *  init vector for storing the firwire FireCamBus objects
   *-----------------------------------------------------------------------*/
  for( int i = 0; i < MAX_FIREWIRE_BOARDS; ++i)
  {
    _fireCamBusses[i] = NULL;
  }
  
}





/*-------------------------------------------------------------------------
 *  cleanup function when Matlab unloads this file
 *-------------------------------------------------------------------------*/
void cleanup()
{
  /*-------------------------------------------------------------------
   *  delete all camera wrapper objects (the cameras itself are 
   *  deleted by the FireCamBus object
   *-------------------------------------------------------------------*/
  if( _camObjects != NULL)
  {
    for( std::map<unsigned int, LMBCamML* >::iterator 
             p = _camObjects->begin(); p!= _camObjects->end(); ++p)
    {
      delete( p->second);
    }
    _camObjects = NULL;
  }
  
  /*-------------------------------------------------------------------
   *  Delete bus objects
   *-------------------------------------------------------------------*/
  for( int i = 0; i < MAX_FIREWIRE_BOARDS; ++i)
  {
    if( _fireCamBusses[i] != NULL)
    {
      delete _fireCamBusses[i];
      _fireCamBusses[i] = NULL;
    }
  }

  /*-------------------------------------------------------------------
   *  be sure, that it will not be cleaned up twice
   *-------------------------------------------------------------------*/
    _initialized = false;
}
  

               


/*-------------------------------------------------------------------------
 *  Matlab entry point to this module
 *-------------------------------------------------------------------------*/
extern "C"
{
void mexFunction( int nlhs, mxArray* plhs[],
                  int nrhs, const mxArray* prhs[])
{
  /*-----------------------------------------------------------------------
   *  block signal CTRL-C until mexFunction returns
   *-----------------------------------------------------------------------*/
  sigset_t signalMask;
  sigemptyset (&signalMask);
  sigaddset (&signalMask, SIGTERM);
  MySignalBlocker blocker( signalMask);

  try
  {
    /*-------------------------------------------------------------------
     *  Do initialization at first call
     *-------------------------------------------------------------------*/
    if( !_initialized)
    {
      _initialize();
      
      /*-------------------------------------------------------------------
       *  register cleanup function
       *-------------------------------------------------------------------*/
      mexAtExit(cleanup);
    }
    
    /*---------------------------------------------------------------------
     *  create argsIn and argsOut object
     *---------------------------------------------------------------------*/
    InputArgs argsIn( nrhs, prhs);
    OutputArgs argsOut( nlhs, plhs);
    argsOut.setMethodName( argsIn.methodName()); // for nicer error messages
    
    /*-------------------------------------------------------------------
     *  static/global method requested ?
     *-------------------------------------------------------------------*/
    if( argsIn.uniqueObjectID() == 0)
    {
      if( argsIn.methodName() == "FireCamBus_new")
      {
        /*-------------------------------------------------------------------
         *  FireCamBus Constructor
         *-------------------------------------------------------------------*/
        argsIn.assertNArgs( 2 + 1);
        argsOut.assertNArgs( 1);
        unsigned int busIndex = (unsigned int) argsIn.asDouble( 2);
        if( busIndex > MAX_FIREWIRE_BOARDS)
        {
          mexErrMsgTxt( "As bus index > 8 is stupid! Or do you have "
                        "a Computer with more than 8 Firewire-Cards "
                        "inside?");
        }
        
        if( _fireCamBusses[busIndex] == NULL)
        {
          // only store bus, if no error was thrown
          _fireCamBusses[busIndex] = new FireCamBusML( busIndex);
        }
        
        /*-----------------------------------------------------------------
         *  return busPointer as unique key for later accessing
         *  of this object
         *-----------------------------------------------------------------*/
        argsOut.setValueUint( 0, (unsigned int)_fireCamBusses[busIndex]);

        return;
      }
      else 
      {
        char errmsg[1024];
        sprintf( errmsg, "No global method '%s' ",
                 argsIn.methodName().c_str());
        mexErrMsgTxt( errmsg);
      }
      
    }
    else
    {
      /*-----------------------------------------------------------------
       *  Check if uniqueObjectID belongs to a FireCamBus object
       *-----------------------------------------------------------------*/
      for( int i = 0; i < MAX_FIREWIRE_BOARDS; ++i)
      {
        if( (unsigned int)_fireCamBusses[i] == argsIn.uniqueObjectID())
        {
          /*---------------------------------------------------------------
           *  call the requested method (special treatment for _delete
           *  method)
           *---------------------------------------------------------------*/
          if( argsIn.methodName() == "FireCamBus_delete")
          {
            argsIn.assertNArgs( 2 + 0);
            argsOut.assertNArgs( 0);
            delete(_fireCamBusses[i]);
            _fireCamBusses[i] = NULL;
          }
          else
          {
            _fireCamBusses[i]->callMethodByName( argsIn.methodName(),
                                                 argsIn, argsOut);
          }

          return;
        }
      }
      
      /*-----------------------------------------------------------------
       *  Check if uniqueObjectID belongs to a camera object
       *-----------------------------------------------------------------*/
      for( int i = 0; i < MAX_FIREWIRE_BOARDS; ++i)
      {
        if( _fireCamBusses[i] != NULL)
        {
//          mexPrintf("searching camera object\n");
          liblmbcam::FireCam* cam = _fireCamBusses[i]->findCameraByPointer( 
              argsIn.uniqueObjectID());

          if( cam != NULL)
          {
            /*-----------------------------------------------------------
             *  camera found, call requested method and return
             *-----------------------------------------------------------*/
//            mexPrintf("found camera object. Trying to call %s\n", 
//                      argsIn.methodName().c_str());
            LMBCamML wrappedCam( cam);
            wrappedCam.callMethodByName( argsIn.methodName(),
                                         argsIn, argsOut);
            return;
          }
          mexPrintf("camera object not found\n");
        }
      }
      
      /*-----------------------------------------------------------------
       *  uniqueObjectID does not belong to any object
       *-----------------------------------------------------------------*/
      char errmsg[1024];
      sprintf( errmsg, "Object with ID=%d does not exist!",
               argsIn.uniqueObjectID());
      mexErrMsgTxt( errmsg);
      
    }
    
  }
  catch( liblmbcam::FireCamBusError& err)
  {
    mexErrMsgTxt( (std::string( "FireCamBusError: ") + err.str()).c_str());
  }
  catch( liblmbcam::LMBError& err)
  {
    mexErrMsgTxt( (std::string( "LMBError: ") + err.str()).c_str());
  }
  catch( ...)
  {
    mexErrMsgTxt( "caught unknown exception!");
  }


}
}

