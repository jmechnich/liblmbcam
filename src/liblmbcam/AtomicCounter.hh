/**************************************************************************
**       Title: 
**    $RCSfile: AtomicCounter.hh,v $
**   $Revision: 1.5 $$Name:  $
**       $Date: 2004/02/23 13:52:32 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: AtomicCounter.hh,v $
**  Revision 1.5  2004/02/23 13:52:32  mechnich
**  allocate mutex on heap to prevent smp system from hanging
**
**  Revision 1.4  2004/02/20 22:37:54  mechnich
**  added MutexLocker class
**
**  Revision 1.3  2003/06/12 16:13:19  mechnich
**  added comments
**
**  Revision 1.2  2003/05/07 00:05:14  mechnich
**  made the mutex allocated on the stack
**
**  Revision 1.1  2003/04/16 16:38:26  mechnich
**  initial revision
**
**
**
**************************************************************************/

#ifndef ATOMICCOUNTER_HH
#define ATOMICCOUNTER_HH

#include "MutexLocker.hh"

/*======================================================================*/
/*!
 *  \class AtomicCounter AtomicCounter.hh
 *  \brief The AtomicCounter template class provides a simple way of
 *         making a counting variable thread safe.
 *
 *  (description)
 */
/*======================================================================*/
template<typename T>
class AtomicCounter
{
public:
  AtomicCounter();
  
  AtomicCounter( T value);
  
  ~AtomicCounter();

  T operator++();
  
  T operator++(int);
  
  T operator--();
  
  T operator--(int);
  
  T operator+=( T change);
  
  T operator-=(T change);
  
  T operator+(T change);
  
  T operator-(T change);
  
  T operator=( T value);

  operator T();  

  bool operator!();

private:
  T p_counter;
  pthread_mutex_t* p_lock;
};

#include "AtomicCounter.icc"

#endif
