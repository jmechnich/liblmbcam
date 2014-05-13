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

#include "QlmbCamParamControl.hh"

#include <QLabel>
#include <QHBoxLayout>
#include <QSlider>

#include <map>
#include <string>

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
QlmbCamParamControlWidget::QlmbCamParamControlWidget( LMBCam* camera,
                                                      QWidget* parent)
        : QWidget( parent), p_camera( camera), p_ncols( 3)
        , p_grid( new QGridLayout( this))
{
  /*-----------------------------------------------------------------------
   *  Get paramter map from camera and create widgets according to GUIHints
   *-----------------------------------------------------------------------*/
  std::map<std::string,LMBCamParam*> parameters = p_camera->parameterMap();
  for( std::map<std::string,LMBCamParam*>::const_iterator it =
           parameters.begin(); it != parameters.end(); ++it)
  {
    if( it->second->GUIHint() == "slider")
    {
      createSlider( it->first.c_str(), it->second, this);
    }
    
    else if( it->second->GUIHint() == "checkbox")
    {
      createCheckBox( it->first.c_str(), it->second, this);
    }
    else
    {
    }
  }
  
  /*-----------------------------------------------------------------------
   *  Find parameters with auto mode and connect the sliders'
   *  'setDisabled'-slot to the checkbox state
   *-----------------------------------------------------------------------*/
  for( std::vector<QSlider*>::const_iterator sliderIt = p_sliders.begin();
       sliderIt != p_sliders.end(); ++sliderIt)
  {
    for( std::vector<QCheckBox*>::const_iterator boxIt = p_autoBoxes.begin();
         boxIt != p_autoBoxes.end(); ++boxIt)
    {
      QString paramName( (*boxIt)->objectName());
      paramName.truncate( paramName.length() - 5);
      
      if( QString( (*sliderIt)->objectName()).startsWith( paramName))
      {
        (*sliderIt)->setDisabled( (*boxIt)->isChecked());
        connect( (*boxIt), SIGNAL( toggled( bool)),
                 (*sliderIt), SLOT( setDisabled( bool)));
      }
    }
  }
  setLayout( p_grid);
}

/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
QSlider*
QlmbCamParamControlWidget::createSlider( const QString& name,
                                         LMBCamParam* param,
                                         QWidget* parent)
{
  /*-----------------------------------------------------------------------
   *  Create three widgets, as long as the parent grid has three rows
   *-----------------------------------------------------------------------*/
  size_t row = p_grid->rowCount();
  p_grid->addWidget( new QLabel( name, parent), row, 0);
  QSlider* slider = new QSlider( Qt::Horizontal, parent);
  slider->setMinimum( param->minValueAsInt());
  slider->setMaximum( param->maxValueAsInt());
  slider->setPageStep( 10);
  slider->setValue( param->asInt());
  slider->setObjectName( name);
  
  p_grid->addWidget( slider, row, 1);
  QLabel* label = new QLabel( QString::number( param->asInt()), parent);
  p_grid->addWidget( label, row, 2);
  connect( slider, SIGNAL( valueChanged(int)),
           label, SLOT( setNum(int)));
  connect( slider, SIGNAL( valueChanged(int)),
           this, SLOT( changeParamValue(int)));

  p_sliders.push_back( slider);
  
  return slider;
}

  
/*=========================================================================
 *  DESCRIPTION OF FUNCTION:
 *  ==> see headerfile
 *=======================================================================*/
QCheckBox*
QlmbCamParamControlWidget::createCheckBox( const QString& name,
                                           LMBCamParam* param,
                                           QWidget* parent)
{
  /*-----------------------------------------------------------------------
   *  Create three widgets, as long as the parent grid has three rows
   *-----------------------------------------------------------------------*/
  size_t row = p_grid->rowCount();
  p_grid->addWidget( new QLabel( name, parent), row, 0);
  QCheckBox* checkbox = new QCheckBox( parent);
  checkbox->setObjectName( name);
  checkbox->setChecked( param->asInt());
  p_grid->addWidget( checkbox, row, 1);
  p_grid->addWidget( new QLabel( parent), row, 2);

  if( name.endsWith( "auto"))
  {
    p_autoBoxes.push_back( checkbox);
    connect( checkbox, SIGNAL( toggled(bool)),
             this, SLOT( changeAutoMode(bool)));
  }

  /*-----------------------------------------------------------------------
   *  Disable DMA checkbox as long as there is no special handling for it
   *-----------------------------------------------------------------------*/
//  if( name.startsWith( "DMA"))
//  {
//    checkbox->setDisabled( true);
//  }
    
  return checkbox;
}

