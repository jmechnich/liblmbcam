/**************************************************************************
**       Title: 
**    $RCSfile: MutexLocker.hh,v $
**   $Revision: 1.1 $$Name:  $
**       $Date: 2004/02/20 22:37:24 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: MutexLocker.hh,v $
**  Revision 1.1  2004/02/20 22:37:24  mechnich
**  initial revision
**
**
**
**************************************************************************/

#ifndef MUTEXLOCKER_HH
#define MUTEXLOCKER_HH

#include <pthread.h>

class MutexLocker
{
public:
  MutexLocker( pthread_mutex_t* mutex)
          :_mutex( mutex)
        {
          pthread_mutex_lock( _mutex);
        }
  
  ~MutexLocker()
        {
          pthread_mutex_unlock( _mutex);
        }

private:
  pthread_mutex_t* _mutex;
};

#endif
