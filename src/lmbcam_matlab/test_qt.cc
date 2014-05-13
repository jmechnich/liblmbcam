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

#include <fstream>
#include <QApplication>
#include <QPushButton>
#include <pthread.h>
#include <unistd.h> // for sleep()
#include <signal.h>
#include "mex.h"

pthread_t     _threadid = 0;
QApplication* _app = NULL;
QPushButton*  _hello = NULL;
int           _i = 0;
bool          _applicationIsLocked = false;
bool          _first_call = true;



void* showButton( void* dummy)
{
  if( _app == NULL)
  {
    std::ofstream os( "debug.txt");
    os << "thread started\n" << std::flush;
    
    char* argv[] = {"hurz"};
    int argc = 1;
    
    _app = new QApplication( argc, argv );
    
    _hello = new QPushButton( "Hello world!", 0 );
    os << "button created started\n" << std::flush;
    _hello->resize( 100, 30 );
    
    _app->setMainWidget( _hello );
    _hello->show();

    os << "starting qapplication\n" << std::flush;
    _app->exec();
    os << "qapplication finished\n" << std::flush;

    delete _app;
    _app = NULL;
    _hello = NULL;
    _threadid = 0;
    pthread_exit((void *) 0);
  }
  else
  {
    printf( "Qt already running");
  }
  
    
  return NULL;
}

static void closeButtonAndApplication()
{
  if( _threadid != 0)
  {
    _app->lock();
    _app->quit();
    _app->unlock();
    
    /*-----------------------------------------------------------------------
     *  wait for thread finishing
     *-----------------------------------------------------------------------*/
    int status;
    int rc = pthread_join( _threadid, (void **)&status);
    if (rc)
    {
      printf("ERROR; return code from pthread_join() is %d\n", rc);
      exit(-1);
    }
  }
}

    



void mexFunction( int nlhs, mxArray *plhs[],
                  int nrhs, const mxArray *prhs[] )
{
   /*-----------------------------------------------------------------------
   *  register exit function
   *-----------------------------------------------------------------------*/
  mexAtExit( closeButtonAndApplication);
  

  /*-----------------------------------------------------------------------
   *  block all signals like CTRL-C
   *-----------------------------------------------------------------------*/
//  sigset_t signalMask;
//  sigfillset (&signalMask);
//  sigprocmask (SIG_BLOCK, &signalMask, NULL);


  if( _threadid == 0)
  {
    int rc = pthread_create( &_threadid, NULL, showButton, NULL);
    if (rc){
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
    }
    return;
    
  }
  

  // try to set Windows button title
  if( _hello != NULL)
  {
    ++_i;
    _app->lock();
    _hello->setText( QString("Hello world %1").arg(_i) );
    _app->unlock();
  }
  
//  for( int i = 0; i < 10; ++i)
//  {
//    mexPrintf( "waiting try CTRL-C %d\n", i);
//    sleep( 1);
//  }
  
  /*-----------------------------------------------------------------------
   *  unblock all signals 
   *-----------------------------------------------------------------------*/
//  sigprocmask (SIG_UNBLOCK, &signalMask, NULL);
  
  
  
}
