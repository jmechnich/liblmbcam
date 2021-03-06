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

  operator T() const;  

  bool operator!();

private:
  T p_counter;
  mutable pthread_mutex_t p_lock;
};

#include "AtomicCounter.icc"

#endif
