/**************************************************************************
**       Title: 
**    $RCSfile: QlmbCamUpdateThread.hh,v $
**   $Revision: 1.1 $$Name:  $
**       $Date: 2003/10/05 19:27:20 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: QlmbCamUpdateThread.hh,v $
**  Revision 1.1  2003/10/05 19:27:20  mechnich
**  initial revision
**
**
**
**************************************************************************/

#ifndef QLMBCAMUPDATETHREAD_HH
#define QLMBCAMUPDATETHREAD_HH

#include <QThread>

namespace libqlmbcam
{
  class QlmbCamVideoWidget;
  
  class QlmbCamUpdateThread : public QThread
  {
  protected:
    QlmbCamUpdateThread( QlmbCamVideoWidget* buddy);
    
    void run();
    
    QlmbCamVideoWidget* _buddy;
    
    friend class QlmbCamVideoWidget;
  };
}

#endif
