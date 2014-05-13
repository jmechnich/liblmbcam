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
