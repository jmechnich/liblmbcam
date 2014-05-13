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
