/**************************************************************************
**       Title: 
**    $RCSfile: QlmbCamParamControl.hh,v $
**   $Revision: 1.3 $$Name:  $
**       $Date: 2003/05/22 08:53:44 $
**   Copyright: GPL $Author: mechnich $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: QlmbCamParamControl.hh,v $
**  Revision 1.3  2003/05/22 08:53:44  mechnich
**  added comments and made short functions inline
**
**  Revision 1.2  2003/05/22 08:27:08  mechnich
**  added signal-and-slot mechanism for controlling the cameras parameters
**
**  Revision 1.1  2003/05/21 08:58:39  mechnich
**  initial revision
**
**
**
**************************************************************************/

#ifndef QLMBCAMPARAMCONTROL_HH
#define QLMBCAMPARAMCONTROL_HH

#include <qgrid.h>
#include <qcheckbox.h>
#include <qstring.h>

#include <LMBCam.hh>
#include <LMBCamParam.hh>

#include <vector>

using namespace liblmbcam;

class QSlider;

class QlmbCamParamControlWidget : public QGrid
{
  Q_OBJECT
  
public:
  QlmbCamParamControlWidget( LMBCam* camera, QWidget* parent);
  
private slots:
  void changeParamValue( int value);
  void changeAutoMode( bool autoMode);
  
private:
  QSlider* createSlider( const QString& name,
                         LMBCamParam* param,
                         QWidget* parent);
  
  QCheckBox* createCheckBox( const QString& name,
                             LMBCamParam* param,
                             QWidget* parent);
  
  LMBCam* p_camera;

  std::vector<QSlider*> p_sliders;
  std::vector<QCheckBox*> p_autoBoxes;
  
};

#include "QlmbCamParamControl.icc"

#endif
