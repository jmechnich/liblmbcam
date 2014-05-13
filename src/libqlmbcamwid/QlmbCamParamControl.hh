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

#ifndef QLMBCAMPARAMCONTROL_HH
#define QLMBCAMPARAMCONTROL_HH

#include <QGridLayout>
#include <QCheckBox>
#include <QString>

#include <LMBCam.hh>
#include <LMBCamParam.hh>

#include <vector>

using namespace liblmbcam;

class QSlider;

class QlmbCamParamControlWidget : public QWidget
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

  size_t p_ncols;
  QGridLayout* p_grid;
  
  std::vector<QSlider*> p_sliders;
  std::vector<QCheckBox*> p_autoBoxes;
};

#include "QlmbCamParamControl.icc"

#endif
