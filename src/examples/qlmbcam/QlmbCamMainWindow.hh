/**************************************************************************
**       Title: 
**    $RCSfile: QlmbCamMainWindow.hh,v $
**   $Revision: 1.7 $$Name:  $
**       $Date: 2005/05/10 20:57:30 $
**   Copyright: GPL $Author: ckeller $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: QlmbCamMainWindow.hh,v $
**  Revision 1.7  2005/05/10 20:57:30  ckeller
**  Added progress bar
**
**  Revision 1.6  2005/05/04 14:47:49  ckeller
**  added some checks if still saving
**
**  Revision 1.5  2005/05/04 12:09:05  ckeller
**  Now using thread to save images
**
**  Revision 1.4  2005/05/03 14:22:53  ckeller
**  added support for grabbing image series
**
**  Revision 1.3  2003/10/07 05:19:41  mechnich
**  - added menu bat
**  - added grab button and functions
**
**  Revision 1.2  2003/10/06 14:01:50  mechnich
**  - added rescanBus() function
**  - changed calls of static QMessageBox functions for Qt backward compatibility
**
**  Revision 1.1  2003/10/05 19:30:34  mechnich
**  initial revision
**
**
**
**************************************************************************/

#ifndef QLMBCAMMAINWINDOW_HH
#define QLMBCAMMAINWINDOW_HH

#include <qwidget.h>
#include <qpushbutton.h>
#include <QlmbCamBusWidget.hh>
#include <QlmbCamWidget.hh>
#include <QlmbCamVideoWidget.hh>
#include <qspinbox.h>
#include <qlcdnumber.h>
#include <qprogressdialog.h> 

#include <LMBCam.hh>

#include <map>

#include "QlmbCamSaveThread.hh"

using namespace liblmbcam;
using namespace libqlmbcam;


  
class QlmbCamMainWindow : public QWidget
{
  Q_OBJECT

public:
  QlmbCamMainWindow( QWidget* parent=0, const char* name=0, WFlags f=0);
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

  void customEvent(QCustomEvent* cevent);
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
