/**************************************************************************
**       Title: 
**    $RCSfile: QlmbCamModeControl.hh,v $
**   $Revision: 1.3 $$Name:  $
**       $Date: 2003/09/19 14:39:42 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: QlmbCamModeControl.hh,v $
**  Revision 1.3  2003/09/19 14:39:42  mechnich
**  - changed handling of Format7 image geometry controls
**
**  Revision 1.2  2003/08/05 15:35:54  mechnich
**  added lineedits for Format7 image position and changed layout
**
**  Revision 1.1  2003/05/22 08:25:56  mechnich
**  initial revision
**
**
**
**************************************************************************/

#ifndef QLMBCAMMODECONTROL_HH
#define QLMBCAMMODECONTROL_HH

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QValidator>
#include <QMessageBox>
#include <LMBCam.hh>
#include <LMBCamError.hh>

using namespace liblmbcam;

class QComboBox;

class QlmbCamModeControlWidget : public QWidget
{
  Q_OBJECT
public:
  QlmbCamModeControlWidget( LMBCam* camera, QWidget* parent);
  
signals:
  void videoModeChanged( const QString&);
  void framerateChanged( double);
  void widthChanged( unsigned int);
  void heightChanged( unsigned int);
  
private slots:
  void changeVideoMode( const QString& mode);
  void changeFramerate( const QString& fps);
  void updateFormat7Geometry();
  
private:
  void updateFramerateCombo();
  void updateFormat7Controls();
    
  LMBCam* p_camera;
  QLineEdit* p_left;
  QLineEdit* p_top;
  QLineEdit* p_width;
  QLineEdit* p_height;
  QLabel* p_maxWidth;
  QLabel* p_maxHeight;
    
  QComboBox* p_modeCombo;
  QComboBox* p_framerateCombo;
};

#include "QlmbCamModeControl.icc"

#endif
