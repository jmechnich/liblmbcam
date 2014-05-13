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

#ifndef QLMBCAMBUSWIDGET_HH
#define QLMBCAMBUSWIDGET_HH

#include <map>

#include <QTreeWidget>
#include <QTreeWidgetItem>
#include <QString>

#include <LMBCamBus.hh>

namespace libqlmbcam
{
  class QlmbCamBusWidget : public QTreeWidget
  {
    Q_OBJECT
  public:
    QlmbCamBusWidget( QWidget* parent=0);
    virtual ~QlmbCamBusWidget();

  public slots:
    void rescan();
    void changeSelectedCamera( const QString& guid);
    
  signals:
    void cameraSelectionChanged( liblmbcam::LMBCam*);

  private slots:
    void changeSelectedCamera();
    
  private:
    void clearBusses();
    
    std::map<QString,liblmbcam::LMBCamBus*> _busses;
    QTreeWidgetItem* _variousRoot;
  };
}

#endif
