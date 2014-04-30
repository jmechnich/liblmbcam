/**************************************************************************
**       Title: 
**    $RCSfile: QlmbCamWidget.hh,v $
**   $Revision: 1.3 $$Name:  $
**       $Date: 2006/05/19 11:29:09 $
**   Copyright: GPL $Author: jhense $
** Description:
**
**    
**
**-------------------------------------------------------------------------
**
**  $Log: QlmbCamWidget.hh,v $
**  Revision 1.3  2006/05/19 11:29:09  jhense
**  Stopping camera when destroyed.
**
**  Revision 1.2  2004/10/19 05:55:38  mechnich
**  added DMA stuff without testing, will probably need future fixes; added absolute control features
**
**  Revision 1.1  2003/10/05 19:27:20  mechnich
**  initial revision
**
**
**
**************************************************************************/

#ifndef QLMBCAMWIDGET_HH
#define QLMBCAMWIDGET_HH

#include <QScrollArea>
#include <QComboBox>
#include <QSlider>
#include <QCheckBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>

#include <map>

#include <LMBCam.hh>
#include <LMBCamParam.hh>

namespace libqlmbcam
{
  class QlmbCamWidget : public QScrollArea
  {
    Q_OBJECT

  public:
    QlmbCamWidget( liblmbcam::LMBCam* camera=0, QWidget* parent=0);

    virtual ~QlmbCamWidget();

  public slots:
    void setCamera( liblmbcam::LMBCam* camera);
    void readSettings();
    void writeSettings();
    void resetCamera();
    void lockControls();
    void unlockControls();
    
  signals:
    void startCamera();
    void stopCamera();
    
  protected slots:
    void updateFramerateCombo( const QString& mode);
    void updateLineEdits( const QString& mode);
    void changeAbsParamValue();
    void changeParamValue( int value);
    void changeAutoMode( bool autoMode);
    void changeAbsControl( bool absMode);
    
  protected:
    virtual void createFireCamGUI();

    QLineEdit* createLineEdit( const QString& name,
                               liblmbcam::LMBCamParam* param,
                               QGridLayout* parent);
  
    QSlider* createSlider( const QString& name,
                           liblmbcam::LMBCamParam* param,
                           QGridLayout* parent);
  
    QCheckBox* createCheckBox( const QString& name,
                               liblmbcam::LMBCamParam* param,
                               QGridLayout* parent);
  
    liblmbcam::LMBCam* _camera;
    QWidget* _controls;
    QComboBox* _modeCombo;
    QComboBox* _framerateCombo;
    QLineEdit* _top;
    QLineEdit* _left;
    QLineEdit* _width;
    QLineEdit* _height;
    QLabel* _maxWidth;
    QLabel* _maxHeight;
    
    std::map<std::string,QLineEdit*> _lineEdits;
    std::map<std::string,QSlider*> _sliders;
    std::map<std::string,QCheckBox*> _checkBoxes;
  
  };
}

#endif
