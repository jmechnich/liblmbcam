/**************************************************************************
**       Title: 
**    $RCSfile: QlmbCamUpdateThread.cc,v $
**   $Revision: 1.1 $$Name:  $
**       $Date: 2003/10/05 19:27:20 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: QlmbCamUpdateThread.cc,v $
**  Revision 1.1  2003/10/05 19:27:20  mechnich
**  initial revision
**
**
**
**************************************************************************/

#include "QlmbCamUpdateThread.hh"

#include "QlmbCamVideoWidget.hh"

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
libqlmbcam::QlmbCamUpdateThread::QlmbCamUpdateThread(
    QlmbCamVideoWidget* buddy)
        :QThread(), _buddy( buddy)
{}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
void
libqlmbcam::QlmbCamUpdateThread::run()
{
  while( _buddy->_camera->isRunning() && _buddy->_doUpdate)
  {
    _buddy->updateImage();
  }
}

