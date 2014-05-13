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

#ifndef QLMBCAMMAINWINDOW_HH
#define QLMBCAMMAINWINDOW_HH

#include <QWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QLCDNumber>
#include <QProgressDialog> 

#include <QlmbCamBusWidget.hh>
#include <QlmbCamWidget.hh>
#include <QlmbCamVideoWidget.hh>

#include <LMBCam.hh>

#include <map>

#include "QlmbCamSaveThread.hh"

using namespace liblmbcam;
using namespace libqlmbcam;

class QlmbCamMainWindow : public QWidget
{
  Q_OBJECT

public:
  QlmbCamMainWindow( QWidget* parent=0);
  ~QlmbCamMainWindow();

private slots:
  void setActiveCamera( liblmbcam::LMBCam* camera);
  void loadSettings();
  void saveSettings();
  void rescanBus();
  void grabImage();
  void ngrabImage();
  void startCamera();
  void stopCamera( bool deleteWidget=true);
  void removeWidgetAndStopCamera();
  
protected :

  bool event(QEvent* cevent);
  void closeEvent(QCloseEvent* e);
  
private:
  QlmbCamBusWidget* _busWidget;
  QlmbCamWidget* _camWidget;

  QPushButton* _grabButton;
  QPushButton* _startButton;
  QPushButton* _stopButton;
  QPushButton* _ngrabButton;
  QSpinBox* _grab_spinbox;
  QProgressDialog* _save_progressDialog;

  QlmbCamSaveThread* _saveThread;
  
  
  LMBCam* _camera;
  std::map<LMBCam*,QlmbCamVideoWidget*> _videoWindows;
};

#endif
