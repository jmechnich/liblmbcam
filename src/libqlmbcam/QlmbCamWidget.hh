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
    
  private slots:
    void updateFramerateCombo( const QString& mode);
    void updateLineEdits( const QString& mode);
    void changeAbsParamValue();
    void changeParamValue( int value);
    void changeAutoMode( bool autoMode);
    void changeAbsControl( bool absMode);
    
  private:
    void clearControls();
    void createGUI();

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
    std::map<std::string,QSlider*>   _sliders;
    std::map<std::string,QCheckBox*> _checkBoxes;
  };
}

#endif
